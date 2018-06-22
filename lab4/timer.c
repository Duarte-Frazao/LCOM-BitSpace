#include "Header.h"

//Global variables declaration
extern int g_num_Interruptions;
extern int g_hook_id_timer;


int timer_subscribe_int(void) {

	//Creates a copy of g_hook_id_timer because it's going to be modified by the kernel
	int hook_id= g_hook_id_timer;

	//Subscribe TIMER0_IRQ notifications
	if(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &g_hook_id_timer) != OK)
		return -1;

	return BIT(hook_id);
}

int timer_unsubscribe_int(void) {
	if(sys_irqrmpolicy(&g_hook_id_timer) != OK) return 1;
	return 0;
}

void timer_int_handler() {
	//Increments the global counter variable g_num_Interruptions
	g_num_Interruptions++;
}
