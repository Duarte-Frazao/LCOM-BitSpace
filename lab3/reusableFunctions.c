#include "Header.h"

extern int g_counter;
extern int g_hook_id;

int readStatus(unsigned long *status){
	if(sys_inb_cnt(STATUS_REGISTER, status) != OK) return 1;
	if(*status & ERROR_READ) return 1; //If parity bit and timeout bit are set there is an error
	return 0;
}


int outBufferFull(){
	unsigned long status;
	if(readStatus(&status)!= OK) return -1;
	if(status & OBF){
		return 0;
	}
	return 1;
	/*
	 * 0 - isFull
	 * 1 - not Full
	 *-1 - error reading status
	 */
}

int readScancode(unsigned char *scancode){
	unsigned long scancodeLong;
	if(outBufferFull() == OK){
		if(sys_inb_cnt(OUT_BUFF, &scancodeLong) != OK)
			return 1;
	}
	else return 1;
	*scancode = (unsigned char) scancodeLong;
	return 0;
}

int kbd_subscribe(){
	int hook_id_copy = g_hook_id;
	if(sys_irqsetpolicy(KB_IRQ,IRQ_REENABLE|IRQ_EXCLUSIVE,&g_hook_id) != OK)
		return -1;

	return BIT(hook_id_copy);
}

int kbd_unsubscribe(){
	if(sys_irqrmpolicy(&g_hook_id) != OK) return 1;
	return 0;
}

int disableInterrupts()
{
	unsigned long command;
	//Tells the kbc status register that we're going to read from the output buffer the command byte
	if (sys_outb(STATUS_REGISTER, KBC_MESSAGE_READ) != OK) return 1;

	//Reads the outputbuffer the command byte
	if (sys_inb_cnt(OUT_BUFF, &command) != OK)return 1;

	command = command & KBC_DISABLE_CODE;

	//Tells the kbc status register that we're going to write to the output buffer the command byte
	if (sys_outb(STATUS_REGISTER, KBC_MESSAGE_WRITE) != OK)return 1;

	//Reads the outputbuffer the command byte
	if (sys_outb(OUT_BUFF, command) != OK)return 1;

	return 0;

}

int enableInterrupts()
{
	unsigned long command;
	//Tells the kbc status register that we're going to read from the output buffer the command byte
	if (sys_outb(STATUS_REGISTER, KBC_MESSAGE_READ) != OK)return 1;

	//Reads the outputbuffer the command byte
	if (sys_inb_cnt(OUT_BUFF, &command)!= OK)return 1;

	command = command | KBC_ENABLE_CODE;

	//Tells the kbc status register that we're going to write to the output buffer the command byte
	if ( sys_outb(STATUS_REGISTER, KBC_MESSAGE_WRITE)!= OK)return 1;

	//Reads the outputbuffer the command byte
	if (sys_outb(OUT_BUFF, command) != OK)return 1;
	return 0;

}

void addDelay()
{
	tickdelay(micros_to_ticks(DELAY_US));
}



