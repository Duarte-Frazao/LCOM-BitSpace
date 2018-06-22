#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <string.h>
#include "video_test.h"

#include "vbe.h"
#include "lmlib.h"
#include "video.h"
#include "stdbool.h"


/* Private global variables */

static char *video_mem;		/* Process (virtual) address to which VRAM is mapped */
static unsigned h_res;		/* Horizontal screen resolution in pixels */
static unsigned v_res;		/* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */



void *vg_init(unsigned short mode){
	struct reg86u reg;

	reg.u.b.ah = VBE_FUNCTION; // VBE call,
	reg.u.b.al = SET_VBE_MODE; //Set VBE mode
	reg.u.w.bx = (BIT(14) | mode); // set bit 14: linear framebuffer
	reg.u.b.intno = BIOS_VIDEO_SERVICE;

	if (sys_int86(&reg) != OK) {
		printf("set_vbe_mode: sys_int86() failed \n");
		return NULL;
	}


	vbe_mode_info_t vbe_mode;
	vbe_get_mode_info(mode, &vbe_mode);


	int r;
	struct mem_range mr;
	unsigned int vram_base = vbe_mode.PhysBasePtr;	//VRAM’s physical address
	unsigned int vram_size = (vbe_mode.XResolution * vbe_mode.YResolution * vbe_mode.BitsPerPixel) / 8; //VRAM’s size, but you can use the frame-buffer size, instead

	h_res = vbe_mode.XResolution;
	v_res = vbe_mode.YResolution;
	bits_per_pixel = vbe_mode.BitsPerPixel;

	//Allow memory mapping
	mr.mr_base = (phys_bytes) vram_base;
	mr.mr_limit = mr.mr_base + vram_size;
	if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("sys_privctl (ADD_MEM) failed: %d\n", r);


	//Map memory
	video_mem = (char *) vm_map_phys(SELF, (void *) mr.mr_base, vram_size);

	if (video_mem == MAP_FAILED){
		panic("Couldn’t map video memory");
		return NULL;
	}

	return video_mem;
}


int vg_exit() {
  struct reg86u reg86;

  reg86.u.b.intno = 0x10; /* BIOS video services */
  reg86.u.b.ah = 0x00;    /* Set Video Mode function */
  reg86.u.b.al = 0x03;    /* 80x25 text mode*/

  if( sys_int86(&reg86) != OK ) {
      printf("\tvg_exit(): sys_int86() failed \n");
      return 1;
  }
  else return 0;
}

//Tests if coordinates are inside the resolution
int inside_res(unsigned short x, unsigned short y)
{

	if (x>= h_res|| y>= v_res || x<0 ||y<0){
		//printf("Coordinates out of screen\n");
		return 1;
	}
	return 0;
}



//Paints a pixel, with the given coordinates and color
int paint_pixel(unsigned short x, unsigned short y, unsigned long color)
{
	//Test if x and y are inside our resolution
	if (inside_res(x,y) != OK){ return  1;}

	//Locating the pixel (char=? or long)
	char * pixel_to_change = video_mem; //base video memory address

	pixel_to_change += y*h_res*bits_per_pixel/8; //adding y position

	pixel_to_change += x*bits_per_pixel/8; //adding x position

	//Painting the pixel with the given color
	*pixel_to_change = (char)color;

	return 0;
}




//draws a square with the given coordinates size and color
int draw_square(unsigned short x, unsigned short y, unsigned short size, unsigned long color)
{


	unsigned long i;
	unsigned long j;

	bool pixelOutOfRes=false;

	//if size is even we had 0.5 to round up the coordinates
	if(size%2 == 1)
	{
		y+= 0.5;
		x+= 0.5;
	}

	for(i =y - size/2 +v_res/2 ; i < y + size/2 +v_res/2; i++){
		for (j=  x - size/2 +h_res/2; j < x + size/2 +h_res/2 ; j++){
			//if pixel is out of boundaries, it doesn't paint but proceeds painting the rest of the pixels that may be inside the boundaries
			if(paint_pixel(j,i,color) != OK) pixelOutOfRes = true;
		}
	}
	video_dump_fb();
	if (pixelOutOfRes) return 1;
	return 0;
}


int draw_line(unsigned short xi, unsigned short yi, unsigned short xf,
		unsigned short yf, unsigned long color)
{


	bool pixelOutOfRes=false;

	//Bresenham's line algorithm

	float deltax = xf - xi;
	float deltay = yf - yi;

	float deltaerr = (deltay / deltax);
	if (deltaerr < 0) deltaerr = -deltaerr;

	float error = 0.0;
	int y = yi;
	int x;

	for (x = xi; x < xf; x++) {

		if (paint_pixel(x, y, color)!= OK) pixelOutOfRes =true;
		error += deltaerr;

		while (error >= 0.5) {
			if (deltay > 0)
				y += 1;
			else
				y -= 1;
			error -= 1.0;
		}

	}


	//Added another cycle in y axis to improve the draw of high slope lines
	deltaerr = 1 / deltaerr;
	error = 0.0;
	x = xi;

	for (y = yi; y < yf; y++) {

		if (paint_pixel(x, y, color)!= OK) pixelOutOfRes =true;
		error += deltaerr;

		while (error >= 0.5) {
			if (deltax > 0)
				x += 1;
			else
				x -= 1;
			error -= 1.0;
		}

	}

	video_dump_fb();

	if (pixelOutOfRes) return 1;

	return 0;

}



