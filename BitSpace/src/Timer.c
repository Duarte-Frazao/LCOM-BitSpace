#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include "TimerMacros.h"
#include "Score.h"
#include "Sprite.h"
#include "Timer.h"


static Timer* timer;

Timer* resetTimerTickedFlag()
{
	timer->timerTickedFlag=0;
	return timer;
}

Timer* newTimer()
{
	timer= (Timer*) malloc(sizeof(Timer));
	timer->numInterruptions=0;
	timer->timerTickedFlag=0;
	timer->hook_id_timer=0;
	return timer;
}

Timer* enableTimer()
{
	timer->IRQ_SET_TIMER = timer_subscribe_int();
	return timer;
}


Timer* getTimer(){
	return timer;
}

void resetTimer()
{
	timer->numInterruptions = 0;
	timer->timerTickedFlag = 0;
}

void eraseTimer()
{
	timer_unsubscribe_int();
	if(timer != NULL) free(timer);
}

void displayTimer(){
	unsigned sec = (timer->numInterruptions / MINIX_FREQ)% 60;
	unsigned min = (timer->numInterruptions / MINIX_FREQ) / 60;

	Sprite **numbers = getNumbers();

	unsigned int num = sec + min*100;
	unsigned int i = 0;

	for (i = 0; i < 5; i++) {
		int dig;
		if(i == 2) dig = 10; //Two points symbol
		else{
			dig = num % 10;
			num /= 10;
		}

		int delta = numbers[dig]->width + 3;
		numbers[dig]->x = 1024/2 + 10 + (-i + 2)*delta;
		numbers[dig]->y =  delta;
		draw_sprite(numbers[dig]);
	}

}

int timer_subscribe_int() {

	//Creates a copy of hook_id because it's going to be modified by the kernel
	int hook_id= timer->hook_id_timer;
	//Subscribe TIMER0_IRQ notifications
	if(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer->hook_id_timer) != OK)
		return -1;

	return BIT(hook_id);
}

int timer_unsubscribe_int() {
	if(sys_irqrmpolicy(&timer->hook_id_timer) != OK) return 1;
	return 0;
}

void timer_int_handler() {
	//Increments the global counter variable g_num_Interruptions
	timer->numInterruptions++;
	timer->timerTickedFlag=1;
}


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
