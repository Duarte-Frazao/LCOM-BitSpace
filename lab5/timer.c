#include "i8254.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>
#include <minix/com.h>
#include "video_test.h"


//Global variables declaration

extern int g_num_Interruptions;
extern int g_hook_id;


int timer_subscribe_int(void) {

	//Creates a copy of g_hook_id because it's going to be modified by the kernel
	int hook_id= g_hook_id;

	//Subscribe TIMER0_IRQ notifications
	if(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &g_hook_id) != OK)
		return -1;

	return BIT(hook_id);
}

int timer_unsubscribe_int(void) {
	if(sys_irqrmpolicy(&g_hook_id) != OK) return 1;
	return 0;
}

void timer_int_handler() {
	//Increments the global counter variable g_num_Interruptions
	g_num_Interruptions++;
}

