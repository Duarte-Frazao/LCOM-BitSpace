#include <minix/syslib.h>
#include <minix/drivers.h>
#include "i8254.h"
#include "timer.h"
#include <minix/com.h>

//Global variables declaration
int g_num_Interruptions = 0;
int g_hook_id = 0;


int timer_set_frequency(unsigned char timer, unsigned long freq) {


	if (timer < 0 || timer > 2){
		printf("Invalid timer selected, it has to be either 0, 1 or 2");
		return 1;
	}

	//Calculates the count value and store in MSB and LSB
	unsigned long valueToWrite = TIMER_FREQ / freq;
	unsigned char LSB = (unsigned char) valueToWrite;
	unsigned char MSB = (unsigned char) (valueToWrite >> 8); //Shift MSB to the last 8 bits

	//Read the timer configuration
	unsigned char st;
	if(timer_get_conf(timer, &st) != OK) return 1;

	//Save the last 4 bits of the timer configuration
	unsigned char mask = BIT(3)|BIT(2)|BIT(1)|BIT(0);


	//Writes timer configuration
	unsigned long writeCommand = (st & mask) | TIMER_LSB_MSB | TIMER_SEL(timer);
	//Creates a new mask and preserves the last four bits, selecting a new timer


	if(sys_outb(TIMER_CTRL, writeCommand)!=OK){
		printf("Error writing to control register \n");
		return 1;
	}

	//TIMER_ADDR (new MACRO declared in i8254.h)
	if(sys_outb(TIMER_ADDR(timer), LSB)!= OK | sys_outb(TIMER_ADDR(timer), MSB) != OK){
		printf("Error writing to timer %d\n",timer);
		return 1;
	}

	return 0;
}

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

int timer_get_conf(unsigned char timer, unsigned char *st)
{
	//Checking if the timer selected is valid
	if (timer < 0 || timer > 2){
		printf("Invalid timer selected, it has to be either 0, 1 or 2");
		return 1;
	}

	//Writing read back command to the control register
	unsigned long outC = TIMER_RB_CMD | TIMER_RB_COUNT_  | TIMER_RB_SEL(timer) ;
	if (sys_outb(TIMER_CTRL, outC)!=OK){
		printf("Error writing readback command to control register \n");
		return 1;
	}

	//Reading status information about the timer
	unsigned long res;
	unsigned char timerAddress = TIMER_0 + timer;
	if(sys_inb(timerAddress, &res) != OK){
		printf("Error reading from the timer\n");
		return 1;
	}

	*st=(unsigned char) res;

	return 0;
}

int timer_display_conf(unsigned char conf) {

	//Output
	printf("\nOutput = %d\n", (conf & BIT(7)) ? 1 : 0 );

	//Null Count
	printf("Null Count = %d\n", (conf & BIT(6)) ? 1 : 0);

	//Counter Initialization
	if (((conf & (BIT(5) | BIT(4))) >>4) == (BIT(1) | BIT(0)))
		 printf("Type of access: LSB followed by MSB  \n");
	 else if ((conf & BIT(5)) == BIT(5))
		 printf("Type of access: MSB  \n");
	 else
		 printf("Type of access: LSB  \n");

	//Operating Mode
	unsigned char modValue = (conf & (BIT(3)|BIT(2)|BIT(1))) >> 1; //mode value

	// Don't care bit,
	if(modValue > 5) modValue -= 4;
	printf("Operating mode = %d\n", modValue);

	//Counting mode
	printf("Counting in %s\n", (conf & BIT(0)) ? "BCD" : "Binary");

	return 0;
}

int timer_test_time_base(unsigned long freq) {
	if(timer_set_frequency(0,freq)) return 1;
	return 0;
}

int timer_test_int(unsigned long time) {

	//Bit mask with hook_id bit position set to 1
	int irq_set = timer_subscribe_int();

	int r;
	int ipc_status;
	message msg;


	while (g_num_Interruptions <= time*MINIX_FREQ) { //Terminates when
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
					timer_int_handler();
					if (g_num_Interruptions%MINIX_FREQ == 0){
						printf("\n%d Seconds",g_num_Interruptions/MINIX_FREQ);
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

	 timer_unsubscribe_int();
	 printf("\n"); //For formatting purposes of the previous prints
	return 0;
}

int timer_test_config(unsigned char timer) {
	unsigned char st;
	if(timer_get_conf(timer, &st))return 1;
	timer_display_conf(st);
	return 0;
}
