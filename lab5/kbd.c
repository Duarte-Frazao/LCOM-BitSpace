#include "kbd.h"
#include "video_test.h"

//Global variables declaration
unsigned char g_keycode = 0;
bool g_twoCommands = false;
int g_counter;
extern int g_num_Interruptions;
extern int g_hook_id;

#define sys_inb_cnt(p,q) sys_inb(p,q)

void wait_esc_press() {
	g_num_Interruptions= 0;
	g_hook_id = 1;

	//Bit mask with hook_id bit position set to 1
	int irq_set = kbd_subscribe();

	int r;
	int ipc_status;
	message msg;

	int no_esc_key_pressed = 0;

	while (no_esc_key_pressed == 0) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
					unsigned char keycode;
					if (readScancode(&keycode) ==OK)
					{
						if (keycode == ESC_BREAK) no_esc_key_pressed = 1;
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
}


