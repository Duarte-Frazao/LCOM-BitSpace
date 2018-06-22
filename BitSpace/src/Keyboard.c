#include <minix/syslib.h>
#include <minix/drivers.h>
#include <stdbool.h>
#include "KeyboardMacros.h"

#include "Keyboard.h"

#define BIT(n)	1<<(n)

static Keyboard* keyboard;

Keyboard* newKeyboard()
{
	keyboard= (Keyboard*) malloc(sizeof(Keyboard));
	keyboard->hook_id_kbd = 1;
	keyboard->state_A = STOP;
	keyboard->state_S = STOP;
	keyboard->state_D = STOP;
	keyboard->state_W = STOP;
	keyboard->ESC_press = 0;
	return keyboard;
}

Keyboard* enableKeyboard()
{
	keyboard->IRQ_SET_KBD = kbd_subscribe();
	return keyboard;
}


Keyboard* getKeyboard(){
	return keyboard;
}

void resetKeyboard(){
	keyboard->state_A = STOP;
	keyboard->state_S = STOP;
	keyboard->state_D = STOP;
	keyboard->state_W = STOP;

	keyboard->ESC_press = false;

	keyboard->scancode = 0xFF;
}

void eraseKeyboard(){
	kbd_unsubscribe();
	if(keyboard != NULL) free(keyboard);
}

void keyboard_handler()
{
	readScancode(&keyboard->scancode);

	switch(keyboard->scancode){
	case KEY_ESC: keyboard->ESC_press = true; break;

	case KEY_A: keyboard->state_A = MOVE; break;
	case KEY_S: keyboard->state_S = MOVE; break;
	case KEY_D: keyboard->state_D = MOVE; break;
	case KEY_W: keyboard->state_W = MOVE; break;


	case KEY_A| RELEASE: keyboard->state_A = DECIDE; break;
	case KEY_S| RELEASE: keyboard->state_S = DECIDE; break;
	case KEY_D| RELEASE: keyboard->state_D = DECIDE; break;
	case KEY_W| RELEASE: keyboard->state_W = DECIDE; break;
	}

}

void update_keyboard(){
	if(keyboard->state_A == DECIDE) keyboard->state_A = STOP;
	if(keyboard->state_S == DECIDE) keyboard->state_S = STOP;
	if(keyboard->state_D == DECIDE) keyboard->state_D = STOP;
	if(keyboard->state_W == DECIDE) keyboard->state_W = STOP;

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

int readScancode(unsigned char *scancode){
	unsigned long scancodeLong;
	if(outBufferFull() == OK){
		if(sys_inb(OUT_BUFF, &scancodeLong) != OK)
			return 1;
	}
	else return 1;
	*scancode = (unsigned char) scancodeLong;
	return 0;
}

int kbd_subscribe(){
	int hook_id_copy = keyboard->hook_id_kbd;
	if(sys_irqsetpolicy(KB_IRQ,IRQ_REENABLE|IRQ_EXCLUSIVE,&keyboard->hook_id_kbd) != OK)
		return -1;

	return BIT(hook_id_copy);
}

int kbd_unsubscribe(){
	if(sys_irqrmpolicy(&keyboard->hook_id_kbd) != OK) return 1;
	return 0;
}

void addDelay(unsigned long time)
{
	tickdelay(micros_to_ticks(time*1000));
}


int clear_IN_OUT_BUFF(){
	unsigned long clear;
	if(sys_inb(OUT_BUFF,&clear)!=OK) return 1;

	return 0;
}
