#include <minix/drivers.h>
#include <minix/driver.h>
#include <minix/com.h>
#include <minix/sysutil.h>
#include <stdbool.h>
#include <minix/syslib.h>
#include <sys/mman.h>
#include <machine/int86.h>
#include "video_test.h"

#include "lmlib.h"
#include "video.h"
#include "vbe.h"
#include "video_gr.h"
#include "test5.h"
#include "timer.h"
#include "kbd.h"
#include "i8254.h"
#include "sprite.h"

int g_hook_id;
int g_num_Interruptions;

int sys_int86(struct reg86u *reg86p);
char * read_xpm(char *pic[], int * width, int * height);


void addDelay(unsigned long period)
{
	//Adds a delay in seconds
	tickdelay(micros_to_ticks(1000 * 1000 * period));
}


int video_test_init(unsigned short mode, unsigned short delay) {
	void* video_mem = vg_init(mode);
	addDelay(delay);
	vg_exit();
	
	//Print VRAM's physical address
	if(video_mem != NULL){
		printf("VRAM physical address: 0x%x", video_mem);
		return 1;
	}


	return 0;
}


//Draws a square in the screen, with the given coordinates and color
int video_test_square(unsigned short x, unsigned short y, unsigned short size, unsigned long color) {

	//Change to video mode
	vg_init(VIDEO_MODE);


	if (draw_square(x, y, size,color) != OK)
	{
		wait_esc_press();
		vg_exit();
		printf("In draw_square: At least one pixel wasn't able to be painted in the screen");
		return 1;
	}


	wait_esc_press();
	vg_exit();
	return 0;

}

int video_test_line(unsigned short xi, unsigned short yi, unsigned short xf,
		unsigned short yf, unsigned long color) {


	//Change to video mode
	vg_init(VIDEO_MODE);

	if (draw_line(xi,yi,xi, yi,color) != OK)
	{
		wait_esc_press();
		vg_exit();
		printf("In draw_line: At least one pixel wasn't able to be painted in the screen");
		return 1;
	}

	wait_esc_press();
	vg_exit();
	return 0;


}




int test_xpm(char *xpm[], unsigned short xi, unsigned short yi) {

	//Change to video mode
	vg_init(VIDEO_MODE);

	Sprite * sprite = create_sprite(xpm, xi,yi, 0,0);
	draw_sprite(sprite);
	video_dump_fb();

	wait_esc_press();
	vg_exit();
	return 0;
}

int test_move(char *xpm[], unsigned short xi, unsigned short yi, unsigned short xf, unsigned short yf, short s, unsigned short f) {

	if(f>MINIX_FREQ) {
		printf("Framerate has to be less than 60\n");
		return 1;
	}

	if(xi + s > xf && yi + s > yf){
		printf("Speed is too high\n");
		return 1;
	}

	bool speedNeg = false;

	if(s<0){
		speedNeg = true;
		s = -s;
	}

	int dx = xf-xi;
	int dy = yf-yi;

	double speedX = s * dx/(dx+dy);
	double speedY = s * dy/(dx+dy);

	int sX = (int) speedX + 0.5;
	int sY = (int) speedY + 0.5;

	if(sX != 0 && sY != 0){
		printf("Only works for vertical and horizontal lines\n");
		return 1;
	}

	//Change to video mode
	vg_init(VIDEO_MODE);


	//Test if line is inside the given resolution
	if(inside_res(xi,yi) != OK || inside_res(xf,yf)){
		vg_exit();
		printf("Coordinates out of screen\n");
		return 1;
	}

	if(speedNeg){
		if(sX != 0)
			sX /= sX;
		if(sY != 0)
			sY /=sY;
	}

	Sprite * sprite = create_sprite(xpm, xi, yi, sX, sY);

	//Bit mask with hook_id bit position set to 1
	int irq_set_timer = timer_subscribe_int();
	int irq_set_kbd = kbd_subscribe();


	int r;
	int ipc_status;
	message msg;
	bool esc_pressed = false;

	g_num_Interruptions= 0;
	g_hook_id = 0;


	int frameCounter = 0;

	while ((!esc_pressed)) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */

				if (msg.NOTIFY_ARG & irq_set_timer)
					timer_int_handler();

				if (msg.NOTIFY_ARG & irq_set_kbd) { /* subscribed interrupt */
					unsigned char keycode;
					if (readScancode(&keycode) ==OK)
						if (keycode == ESC_BREAK)
							esc_pressed = true;
				}

				break;

			default:
				break; /* no other notifications expected: do nothing */
			}

		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}


		if(f*g_num_Interruptions >= MINIX_FREQ ){

			if(speedNeg){
				frameCounter++;
			}

			if(!(frameCounter >= s) && speedNeg){
				continue;
			}

			frameCounter = 0;
			g_num_Interruptions = 0;
			int xn = sprite->x;
			int yn = sprite->y;
			int width = sprite->width;
			int height = sprite->height;

			if(!(xn > xf - width/2 && yn > yf-height/2)) {

				animate_sprite(sprite);
				unsigned int i,j;

				for (i = yn-sY; i < yn; i++)
					for (j = xn-sX; j < xn-sX+width; j++)
						paint_pixel(j, i, 0);

				for (i = yn-sY; i < yn-sY+height; i++)
					for (j = xn-sX; j < xn; j++)
						paint_pixel(j, i, 0);


			}
		}


	}

	destroy_sprite(sprite);
	timer_unsubscribe_int();
	kbd_unsubscribe();
	vg_exit();
	return 0;
}

int test_controller() {


	//Getting information
	vbe_info_block_t information;

	if(vbe_get_info_block(&information) != OK){printf("In test_controller(): Error in vbe_get_info_block() call");return 1;}

	///VBE Version

	printf("%d.%d\n",information.VbeVersion[1],information.VbeVersion[0]);

	//MODE NUMBERS
	//Using the reserved section of info block, that retains the modes information
	unsigned short *mode = (unsigned short *) &information.Reserved;

	unsigned short * pMode = (unsigned short *) mode;

	//While the code (-1) doesn't appear
	while(*pMode != END_VBE_MODES_CODE)
	{
		printf("0x%x:", *pMode);
		pMode++;
	}

	//Size of VRAM memory in KB

	printf("\n%d\n", information.TotalMemory *64);

	return 0;

}

	
