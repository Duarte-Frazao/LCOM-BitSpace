#include "Header.h"

extern unsigned char g_keycode;
extern bool g_twoCommands;
extern int g_hook_id;



void printCode(){
	//Filters the scancodes, so that it recognizes when a key has two scancodes (0xe0) and prints accordingly
	if(g_twoCommands)
		printf((g_keycode & TYPE_MASK) ? "Breakcode = 0x%x 0x%x\n" : "Makecode  = 0x%x 0x%x\n", 0xe0, g_keycode);
	else
		printf((g_keycode & TYPE_MASK) ? "Breakcode = 0x%x\n\n\n" : "Makecode  = 0x%x\n", g_keycode);

}


void kbd_handler_C() {
	//Reads scancode, if not a valid read it exits
	if (readScancode(&g_keycode) != OK)
		return;

	//If scancode read is 0xe0 activates the flag g_twoCommands
	if (g_keycode == TWO_CODES)
		g_twoCommands = true;

	//If keycode is different than 0xe0 prints the complete scancode
	if(g_keycode != TWO_CODES){
		printCode();
		g_twoCommands = false;
	}
}



