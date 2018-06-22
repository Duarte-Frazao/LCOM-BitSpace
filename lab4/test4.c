#include "Header.h"


int g_hook_id_mouse = 12;
int g_hook_id_kbc = 1;
bool g_packetCompleted = false;
unsigned char g_packet[3];
unsigned char g_bytePacketNum;

int g_hook_id_timer= 0;
int g_num_Interruptions =0;


int mouse_test_packet(unsigned short cnt) {

	int r;
	int ipc_status;
	message msg;
	g_bytePacketNum = 1;

	//Clear IO buffer
	clear_IN_OUT_BUFF();

	//Enable stream mode and data report
	if (mouseConfiguration(ENABLE_STREAM_MODE) != OK)
		return 1;

	if (mouseConfiguration(ENABLE_DATA_REPORT) != OK)
		return 1;

	//Subscribes the mouse
	int irq_set_mouse = mouse_subscribe();

	while (cnt > 0) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notificaion */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set_mouse) { /* subscribed interrupt */
					packet_handler();
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}


		if (msg.NOTIFY_ARG & irq_set_mouse) {

			//if a full packet is read print the information and decrement the number of packets to read
			if (g_packetCompleted) {
				cnt--; //decrements count for each packet read
				printPacket(g_packet);
				g_packetCompleted = false;
			}
		}
	}

	//Disable Data report
	if (mouseConfiguration(DISABLE_DATA_REPORT) != OK)
		return 1;

	//Unsibscribes mouse
	mouse_unsubscribe();

	//Clear IO buffer
	clear_IN_OUT_BUFF();

	return 0;
}

int mouse_test_async(unsigned short idle_time){

	int r;
	int ipc_status;
	message msg;
	g_bytePacketNum = 1;

	//Clear IO buffer
	clear_IN_OUT_BUFF();


	//Enable stream mode and data report of the mouse
	if (mouseConfiguration(ENABLE_STREAM_MODE) != OK)
		return 1;

	if (mouseConfiguration(ENABLE_DATA_REPORT) != OK)
		return 1;

	//Subscribes the mouse
	int irq_set_mouse = mouse_subscribe();

	//Subscribes the timer
	int irq_set_timer = timer_subscribe_int();


	while ((g_num_Interruptions / MINIX_FREQ) < idle_time) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notificaion */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set_mouse) /* subscribed interrupt */
					packet_handler();

				if (msg.NOTIFY_ARG & irq_set_timer)
					timer_int_handler();
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}


		if (msg.NOTIFY_ARG & irq_set_mouse) {

			//if a full packet is read print the information and decrement the number of packets to read
			if (g_packetCompleted) {
				printPacket(g_packet);
				g_packetCompleted = false;
				g_num_Interruptions = 0;
			}
		}
	}

	printf("Time is over!\n");

	//Disable Data report
	if (mouseConfiguration(DISABLE_DATA_REPORT) != OK)
		return 1;

	//Unsubscribes both mouse and timer
	mouse_unsubscribe();
	timer_unsubscribe_int();

	//Clear IO buffer
	clear_IN_OUT_BUFF();

	return 0;
}

int mouse_test_remote(unsigned long period, unsigned short cnt){
	g_bytePacketNum = 1;

	//Disable Data report of the mouse, first we enable it as a trick to receive displacements
	if (mouseConfiguration(ENABLE_DATA_REPORT) != OK)
		return 1;
	if (mouseConfiguration(DISABLE_DATA_REPORT) != OK)
		return 1;

	//Enable Remote mode
	if (mouseConfiguration(ENABLE_REMOTE_MODE) != OK)
		return 1;

	//Disables the both keyboard and mouse interrupts and also disables keyboard
	if (disableIH() != OK)
		return 1;

	//Clear IO buffer
	if (clear_IN_OUT_BUFF() != OK)
		return 1;

	while (cnt > 0) {
		packet_handler_poll();

		//if a full packet is read print the information and decrement the number of packets to read
		if (g_packetCompleted) {
			cnt--; //decrements count for each packet read
			printPacket(g_packet);
			g_packetCompleted = false;
			addDelay(period);
		}
	}

	//Set stream mode
	if (mouseConfiguration(ENABLE_STREAM_MODE) != OK)
		return 1;

	//Clear IO buffer
	clear_IN_OUT_BUFF();

	//Reenable interrupts on both keyboard and mouse, and also reenables keyboard
	if (enableIH() != OK)
		return 1;

	//Clear IO buffer
	clear_IN_OUT_BUFF();

	return 0;
}




typedef enum {INIT, DRAW, COMP} state_t;
typedef enum {RDOWN, RUP, MOVE} ev_type_t;

typedef struct{
	int hor;
	int ver;
}mouse_t;

typedef struct{
	ev_type_t type;
	mouse_t directionFinal;
	mouse_t directionPacket;
	bool exitCycle;
} event_t;

event_t* construct_event_t() {
	event_t* evt = malloc(sizeof(event_t));
	evt->exitCycle = false;
	return evt;
}

event_t* update_event_t(event_t *evt, unsigned char packet[]) {

	evt->directionPacket.hor = g_packet[1];
	evt->directionPacket.ver = g_packet[2];

	if (g_packet[0] & BIT(4)) evt->directionPacket.hor -= 256;
	if (g_packet[0] & BIT(5)) evt->directionPacket.ver -= 256;

	if (g_packet[0] & BIT(1)) evt->type = RDOWN;
	else evt->type = RUP;

	return evt;
}

void check_hor_line(event_t *evt, int length) {
	static state_t st = INIT; // initial state; keep state

	//Initially the positive slope curve did not occur.
	evt->exitCycle = false;

	switch (st) {
	case INIT:
		if (evt->type == RDOWN)
			st = DRAW;
		break;

	case DRAW:

		if (evt->type == RDOWN) { //if the right button is being pressed

			//Updates the displacement of the mouse since the right button was pressed
			evt->directionFinal.hor += evt->directionPacket.hor;
			evt->directionFinal.ver += evt->directionPacket.ver;


			//Vertical and Horizontal displacement in the same direction
			if (evt->directionFinal.ver * evt->directionFinal.hor > 0
					//Horizontal displacement is as indicated by the length
					&& evt->directionFinal.hor *  length > 0
					&& abs(evt->directionFinal.hor) > abs(length)) {

				evt->exitCycle = true;
				printf("Line draw completed.\n");

				evt->directionFinal.hor = 0;
				evt->directionFinal.ver = 0;
			}
		}

		else if (evt->type == RUP){
			//Restart the event accumulated distance
			evt->directionFinal.hor = 0;
			evt->directionFinal.ver = 0;

			//Set the state back to the initial state
			st = INIT;
		}

	break;

default: break;
	}
}


int mouse_test_gesture(short length) {

	int r;
	int ipc_status;
	message msg;
	g_bytePacketNum = 1;
	event_t* evt = construct_event_t();

	//Clear IO buffer
	clear_IN_OUT_BUFF();

	//Enable Data report of the mouse
	if (mouseConfiguration(ENABLE_STREAM_MODE) != OK)
		return 1;

	if (mouseConfiguration(ENABLE_DATA_REPORT) != OK)
		return 1;

	//Subscribes the mouse
	int irq_set_mouse = mouse_subscribe();


	while (!evt->exitCycle) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notificaion */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set_mouse) { /* subscribed interrupt */
					packet_handler();
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}

		if (msg.NOTIFY_ARG & irq_set_mouse) {

			if (g_packetCompleted) {
				update_event_t(evt, g_packet);
				printPacket(g_packet);
				check_hor_line(evt,length);
				g_packetCompleted = false;
			}
		}
	}

	//Disable Data report
	if (mouseConfiguration(DISABLE_DATA_REPORT) != OK)
		return 1;

	mouse_unsubscribe();

	//Clear IO buffer
	clear_IN_OUT_BUFF();

	return 0;
}
