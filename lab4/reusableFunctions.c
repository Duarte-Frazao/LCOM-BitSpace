#include "Header.h"

extern int g_hook_id_mouse;
extern int g_hook_id_kbc;
extern unsigned char g_packet[];
extern unsigned char g_bytePacketNum;
extern bool g_packetCompleted;


int disableIH()
{

	//Subscribes the keyboard
	kbd_subscribe();

	unsigned long command;
	//Tells the kbc status register that we're going to read from the output buffer the command byte
	if (sys_outb(STATUS_REGISTER, KBC_MESSAGE_READ) != OK) return 1;

	//Reads the outputbuffer the command byte
	if (sys_inb(OUT_BUFF, &command) != OK)return 1;

	//First changes the command byte to disable interrupts and the to disable the keyboard
	command = (command & KBC_DISABLE_INTERRUPTS) | KBC_DISABLE_KEYBOARD;

	//Tells the kbc status register that we're going to write to the output buffer the command byte
	if (sys_outb(STATUS_REGISTER, KBC_MESSAGE_WRITE) != OK)return 1;

	//Reads the outputbuffer the command byte
	if (sys_outb(OUT_BUFF, command) != OK)return 1;

	//Unsubscribes the keyboard
	kbd_unsubscribe();
	return 0;

}

int enableIH()
{
	unsigned long command;
	//Tells the kbc status register that we're going to read from the output buffer the command byte
	if (sys_outb(STATUS_REGISTER, KBC_MESSAGE_READ) != OK)return 1;

	//Reads the outputbuffer the command byte
	if (sys_inb(OUT_BUFF, &command)!= OK)return 1;

	//First changes the command byte to reenable interrupts and the to reenable the keyboard
	command = (command | KBC_ENABLE_INTERRUPTS) & KBC_ENABLE_KEYBOARD;

	//Tells the kbc status register that we're going to write to the output buffer the command byte
	if ( sys_outb(STATUS_REGISTER, KBC_MESSAGE_WRITE)!= OK)return 1;

	//Reads the outputbuffer the command byte
	if (sys_outb(OUT_BUFF, command) != OK)return 1;
	return 0;

}


void packet_handler_poll() {
	unsigned long singleBytePacket;

	//Signal to send packets in remote mode
	mouseConfiguration(READ_DATA_REQUEST);

	//If the output buffer is not full, reads the three packets
	if (outBufferFull() == 0) {
		if (sys_inb(OUT_BUFF, &singleBytePacket) != OK) return;
		g_packet[g_bytePacketNum - 1] = (unsigned char) singleBytePacket;
		g_bytePacketNum++;

		if (sys_inb(OUT_BUFF, &singleBytePacket) != OK) return;
		g_packet[g_bytePacketNum - 1] = (unsigned char) singleBytePacket;
		g_bytePacketNum++;

		if (sys_inb(OUT_BUFF, &singleBytePacket) != OK) return;
		g_packet[g_bytePacketNum - 1] = (unsigned char) singleBytePacket;
		g_bytePacketNum++;
	}
	//When the three packets are read, reset the bytePacketNum
	if (g_bytePacketNum == 4) {
		g_bytePacketNum = 1;
		g_packetCompleted = true;
	}

}


int mouse_subscribe(){
	int hook_id_copy = g_hook_id_mouse;
	if(sys_irqsetpolicy(MOUSE_IRQ,IRQ_REENABLE|IRQ_EXCLUSIVE,&g_hook_id_mouse) != OK)
		return -1;

	return BIT(hook_id_copy);
}

int mouse_unsubscribe()
{
	if(sys_irqrmpolicy(&g_hook_id_mouse) != OK) return -1;
	else return 0;
}



int kbd_subscribe(){
	int hook_id_copy = g_hook_id_kbc;
	if(sys_irqsetpolicy(KB_IRQ,IRQ_REENABLE|IRQ_EXCLUSIVE,&g_hook_id_kbc) != OK)
		return -1;

	return BIT(hook_id_copy);
}

int kbd_unsubscribe(){
	if(sys_irqrmpolicy(&g_hook_id_kbc) != OK) return 1;
	return 0;
}

void packet_handler()
{
	unsigned long singleBytePacket;


	//reads a byte, while it's still the first one to be read and we didn't yet receive the first byte, repeats
	do {
		if (outBufferFull() == OK) sys_inb(OUT_BUFF, &singleBytePacket);

	} while (g_bytePacketNum == 1 && ((singleBytePacket & BIT(3)) == 0));

	//Stores the byte
	g_packet[g_bytePacketNum-1] = (unsigned char) singleBytePacket;
	g_bytePacketNum++;

	//When the three packets are read, reset the bytePacketNum
	if (g_bytePacketNum == 4){
		g_bytePacketNum = 1;
		g_packetCompleted = true;
	}




}

int readStatus(unsigned long *status){
	if(sys_inb(STATUS_REGISTER, status) != OK) return 1;
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

		if ((status & IBF) == 0)
		{
			//Tells the kbc status register that we're going to write to the mouse
			if (sys_outb(STATUS_REGISTER, WRITE_TO_MOUSE) != OK) return 1;

			//Writes the argument command to the output buffer
			if (sys_outb(OUT_BUFF, cmd) != OK) return 1;

			//Adds a delay to wait for the ack
			addDelay(2);

			//reads the response at least one time and while it receives a NACK response
			do
			{
				if (sys_inb(OUT_BUFF, &responseByte) != OK) return 1;
				responseByte_char = (unsigned char) responseByte;

			}while(responseByte_char==NACK);
			//If it receives an ACK response returns
			if (responseByte_char == ACK) return 0;
		}
		//If it reaches here it means it received an ERROR byte
		failedTries++;

	}
	return 1;
}



void addDelay(unsigned long period)
{
	//Adds a delay in seconds
	tickdelay(micros_to_ticks(1000 * period));
}



int clear_IN_OUT_BUFF(){
	unsigned long clear;
	unsigned long statusByte;

	//Reads the status byte from the status register
	readStatus(&statusByte);

	//Reads the output buffer untill it's empty
	if (statusByte & OBF){
		return sys_inb(OUT_BUFF,&clear);
	}
	else return 0;
}














