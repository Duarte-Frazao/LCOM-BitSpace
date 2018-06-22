#include <minix/drivers.h>
#include <minix/driver.h>
#include <minix/com.h>
#include <minix/sysutil.h>
#include <minix/syslib.h>

#include "Mouse.h"
#include "MouseMacros.h"
#include "Sprite.h"
#include "BitSpace.h"
#include "Queue.h"
#include "Timer.h"


#define BIT(n) (0x01<<(n))

static Mouse* mouse = NULL;
static Mouse mousePreviousState;

static Sprite * cross_sprite;
static Sprite * mouse_sprite;

queue * packetsToUpdate;

void enableMouse(){
	//Clear IO buffer
	clear_IN_OUT_BUFF();

	//Enable stream mode and data report
	if (mouseConfiguration(ENABLE_STREAM_MODE) != OK)
		return;

	if (mouseConfiguration(ENABLE_DATA_REPORT) != OK)
		return;

	//Subscribes the mouse
	mouse->IRQ_SET_MOUSE = mouse_subscribe();


}


Mouse* getMouse(){
	return mouse;
}

Mouse* newMouse()
{
	mouse = (Mouse*) malloc(sizeof(Mouse));

	mouse->x = getHorRes()/2;
	mouse->y = getVerRes()/2 - 10;

	mouse->dx=0;
	mouse->dy=0;

	mouse->bytePacketNum =1;

	mouse->leftButtonPressed=0;
	mouse->rightButtonPressed=0;

	mouse->draw = 1;

	mouse->hook_id_mouse = 12;
	cross_sprite = create_sprite_Bitmap("/home/lcom/svn/BitSpace/res/images/crossair_white.bmp", mouse->x, mouse->y, 0,0);
	mouse_sprite = create_sprite_Bitmap("/home/lcom/svn/BitSpace/res/images/cursor.bmp", mouse->x, mouse->y, 0,0);

	packetsToUpdate = createQueue();
	return mouse;
}



void drawMouse(state_t st)
{
	if(st == GAME){
		cross_sprite->x = mouse->x;
		cross_sprite->y = mouse->y;
		correctMargins(cross_sprite);
		 mouse->x = cross_sprite->x;
		 mouse->y = cross_sprite->y;
		draw_sprite(cross_sprite);
	}
	else{
		mouse_sprite->x = mouse->x;
		mouse_sprite->y = mouse->y;
		correctMargins(mouse_sprite);
		mouse->x = mouse_sprite->x;
		mouse->y = mouse_sprite->y;
		draw_sprite(mouse_sprite);
	}
	mouse->draw=0;
}

void updateMouse()
{
	if(isEmptyQueue(packetsToUpdate)) return;

	unsigned char packet0 = popQueue(packetsToUpdate);
	unsigned char packet1 = popQueue(packetsToUpdate);
	unsigned char packet2 = popQueue(packetsToUpdate);

	mouse->leftButtonPressed = (packet0&BIT(0));
	mouse->rightButtonPressed = ((packet0&BIT(1)) >>1);

	//x position
	if (packet0 & BIT(4))
	{
		mouse->x -= (256 - packet1);
		mouse->dx = -(256 - packet1);
	}else
	{
		mouse->x += packet1;
		mouse->dx = packet1;
	}

	//y position
	if (packet0 & BIT(5))
	{
		mouse->y += (256 - packet2);
		mouse->dy = (256 - packet2);
	}else
	{
		mouse->y -= packet2;
		mouse->dy = -packet2;
	}

	cross_sprite->x = mouse->x;
	cross_sprite->y = mouse->y;
	//mouse->draw = 1;
}

void resetMouse()
{
	mouse->x=1024/2;
	mouse->y=768/2;
	mouse->dx=0;
	mouse->dy=0;
	mouse->bytePacketNum = 1;
	mouse->leftButtonPressed = 0;
	mouse->rightButtonPressed = 0;
	mouse->draw = 0;
}

void resetMouseD()
{
	mouse->dx=0;
	mouse->dy=0;
}

void eraseMouse()
{
	//Disable Data report
	mouseConfiguration(DISABLE_DATA_REPORT);
	clear_IN_OUT_BUFF();
	mouse_unsubscribe();
    if (mouse == NULL)
        return;
	free(mouse);
}



int mouse_subscribe(){
	int hook_id_copy = mouse->hook_id_mouse;
	if(sys_irqsetpolicy(MOUSE_IRQ,IRQ_REENABLE|IRQ_EXCLUSIVE,&mouse->hook_id_mouse) != OK)
		return -1;

	return BIT(hook_id_copy);
}

int mouse_unsubscribe()
{
	if(sys_irqrmpolicy(&mouse->hook_id_mouse) != OK) return -1;
	else return 0;
}



void packet_handler()
{
	unsigned long singleBytePacket;

	//reads a byte, while it's still the first one to be read and we didn't yet receive the first byte, repeats
//	do {
//		if (outBufferFull() == OK) sys_inb(OUT_BUFF, &singleBytePacket);
//
//	} while (mouse->bytePacketNum  == 1 && ((singleBytePacket & BIT(3)) == 0));

	if (outBufferFull() == OK) sys_inb(OUT_BUFF, &singleBytePacket);
	if(mouse->bytePacketNum  == 1 && ((singleBytePacket & BIT(3)) == 0)) return;

	//Stores the byte
	mouse->packet[mouse->bytePacketNum-1] = (unsigned char) singleBytePacket;
	mouse->bytePacketNum++;

	//When the three packets are read, reset the bytePacketNum
	if (mouse->bytePacketNum == 4){
		mouse->bytePacketNum = 1;
		pushQueue(packetsToUpdate, mouse->packet[0]);
		pushQueue(packetsToUpdate, mouse->packet[1]);
		pushQueue(packetsToUpdate, mouse->packet[2]);
	}
}


void printPacket(unsigned char *packet) {
	//Display of the bytes information
	printf("B1= 0x%02x B2= 0x%02x B3= 0x%02x  LB= %d MB= %d RB= %d XOV= %d YOV= %d  ",
			packet[0], packet[1], packet[2], (packet[0] & BIT(0)),
			(packet[0] & BIT(2)) >> 2, (packet[0] & BIT(1)) >> 1,
			(packet[0] & BIT(6)) >> 6, (packet[0] & BIT(7)) >> 7);


	//Display of the x position (If it's negative we do 256 - packet , which is the equivalent to transform from 2's complement)
	printf("X= ");
	if (packet[0] & BIT(4))
		printf("%4d  ", -(256 - packet[1]));
	else
		printf("%4d  ", packet[1]);

	//Display of the y position
	printf("Y= ");
	if (packet[0] & BIT(5))
		printf("%4d  ", -(256 - packet[2]));
	else
		printf("%4d ", packet[2]);

	printf("\n");
}


int mouseConfiguration(short cmd)
{

	unsigned long responseByte;
	unsigned char responseByte_char;

	unsigned long status;

	//Reads the status byte from the status register
	if(readStatus(&status)!= OK) return -1;

	int failedTries = 0;

	//Changes the configuration of the mouse
	while (failedTries < 10)
	{

		if ((status & IBF) == OK)
		{
			//Tells the kbc status register that we're going to write to the mouse
			if (sys_outb(STATUS_REGISTER, WRITE_TO_MOUSE) != OK) return 1;

			//Writes the argument command to the output buffer
			if (sys_outb(OUT_BUFF, cmd) != OK) return 1;

			//reads the response at least one time and while it receives a NACK response

			do
			{	//Adds a delay to wait for the ack
				addDelay(2);
				if (sys_inb(OUT_BUFF, &responseByte) != OK) return 1;
				responseByte_char = (unsigned char) responseByte;
			}while(responseByte_char == NACK);

			//If it receives an ACK response returns
			if (responseByte_char == ACK) return 0;
		}
		//If it reaches here it means it received an ERROR byte
		failedTries++;

	}
	return 1;
}
