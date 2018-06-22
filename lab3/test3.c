#include "Header.h"

//Global variables declaration
int g_hook_id = 1;
unsigned char g_keycode = 0;
bool g_twoCommands = false;
int g_counter;
int g_num_Interruptions= 0;

//Functions
void kbc_asm_handler();


#ifdef LAB3
int sys_inb_cnt(port_t port, unsigned long *byte){
	g_counter++;
	return sys_inb(port, byte);
}
#else
#define sys_inb_cnt(p,q) sys_inb(p,q)
#endif




int kbd_test_scan(unsigned short assembly) {
	//Bit mask with hook_id bit position set to 1
	g_counter = 0;
	int irq_set = kbd_subscribe();

	int r;
	int ipc_status;
	message msg;

	while (g_keycode != ESC_BREAK) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
					if (assembly == 0)
						kbd_handler_C();

					else {
						kbc_asm_handler();

						if(g_keycode == 0xe0)
							g_twoCommands = true;
						else{
							printCode();
							g_twoCommands = false;
						}
					}
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	kbd_unsubscribe();
	if(assembly == 0) printf("Number of sys_inb calls: %d\n", g_counter);
	return 0;
}

int kbd_test_poll() {

	g_counter = 0;
	g_keycode = 0x00;

	if(disableInterrupts() != OK) return 1;

	while((g_keycode != ESC_BREAK))
	{
		//Read status register, check if output buffer is ready to be read
		//If it's ready, reads output buffer and resets the delays counter
		//Else increments the delays counter

		kbd_handler_C();

		//Adds a delay to wait for command
		addDelay();
	}

	if(enableInterrupts() != OK) return 1;

	printf("Number of sys_inb calls: %d\n", g_counter);

	return 0;
}


int kbd_test_timed_scan(unsigned short n) {
//Bit mask with hook_id bit position set to 1
	int irq_set_kbd = kbd_subscribe();
	int irq_set_timer = timer_subscribe_int();

	int r;
	int ipc_status;
	message msg;

	while ((g_num_Interruptions / MINIX_FREQ) < n) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set_kbd) { /* subscribed interrupt */
					g_num_Interruptions = 0;
					kbd_handler_C();
					}
				if (msg.NOTIFY_ARG & irq_set_timer)
					timer_int_handler();
				break;

			default: break; /* no other notifications expected: do nothing */
			}

		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	kbd_unsubscribe();
	timer_unsubscribe_int();
	printf("\n"); //For formatting purposes of the previous prints
	return 0;
}
