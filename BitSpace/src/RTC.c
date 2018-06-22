#include <minix/drivers.h>
#include <minix/driver.h>
#include <stdbool.h>
#include <stdio.h>
#include "RTC_Macros.h"
#include "RTC.h"

#define BIT(n)	1<<(n)

static Rtc *rtc;


inline void enable_int() {asm("sti");}
inline void disable_int() {asm("cli");}

inline unsigned long BCD_to_BIN(unsigned long BCD){
	return ((BCD & 0xF0) >> 4)*10 + (BCD & 0x0F);}

inline unsigned long BIN_to_BCD(unsigned long BIN){
	return	(BIN % 10) | ((BIN/10) << 4);}



Rtc* newRtc(){
	rtc = (Rtc *) malloc(sizeof(Rtc));
	rtc->hook_id_rtc = 8;
	rtc->alarm_int = false;
}

Rtc* enableRtc(){
	rtc->IRQ_SET_RTC = rtc_subscribe_int();
}

Rtc* getRtc(){
	return rtc;
}

void resetRtc(){
	rtc->alarm_int = false;
}

void eraseRtc(){
	rtc_unsubscribe();
	if(rtc != NULL){
		free(rtc);
	}
}

char * getDate(){
	unsigned long sec, min, hrs, wkd, day, mth, yrs;
	char* buffer = NULL;

	disable_int();

	if(sys_outb(RTC_ADDR_REG, RTC_SEC) != OK) return buffer;
	if(sys_inb(RTC_DATA_REG, &sec) != OK) return buffer;

	if(sys_outb(RTC_ADDR_REG, RTC_MIN) != OK) return buffer;
	if(sys_inb(RTC_DATA_REG, &min) != OK) return buffer;

	if(sys_outb(RTC_ADDR_REG, RTC_HRS) != OK) return buffer;
	if(sys_inb(RTC_DATA_REG, &hrs) != OK) return buffer;

	if(sys_outb(RTC_ADDR_REG, RTC_WKD) != OK) return buffer;
	if(sys_inb(RTC_DATA_REG, &wkd) != OK) return buffer;

	if(sys_outb(RTC_ADDR_REG, RTC_DAY) != OK) return buffer;
	if(sys_inb(RTC_DATA_REG, &day) != OK) return buffer;

	if(sys_outb(RTC_ADDR_REG, RTC_MTH) != OK) return buffer;
	if(sys_inb(RTC_DATA_REG, &mth) != OK) return buffer;

	if(sys_outb(RTC_ADDR_REG, RTC_YRS) != OK) return buffer;
	if(sys_inb(RTC_DATA_REG, &yrs) != OK) return buffer;

	enable_int();
	//"%02d:%02d:%02d %02d/%02d/20%02d"
	int size = asprintf(&buffer, "%02d %02d %02d %02d %02d %02d",
			BCD_to_BIN(hrs), BCD_to_BIN(min), BCD_to_BIN(sec),
			BCD_to_BIN(day), BCD_to_BIN(mth), BCD_to_BIN(yrs));
	return buffer;
}

int rtc_set_alarm(unsigned int hrs, unsigned int min, unsigned int sec){
	if(hrs != RTC_DTC) hrs = BIN_to_BCD(hrs);
	if(min != RTC_DTC)min = BIN_to_BCD(min);
	if(sec != RTC_DTC) sec = BIN_to_BCD(sec);

	if(sys_outb(RTC_ADDR_REG, RTC_SEC_A) != OK) return 1;
	if(sys_outb(RTC_DATA_REG, sec) != OK) return 1;

	if(sys_outb(RTC_ADDR_REG, RTC_MIN_A) != OK) return 1;
	if(sys_outb(RTC_DATA_REG, min) != OK) return 1;

	if(sys_outb(RTC_ADDR_REG, RTC_HRS_A) != OK) return 1;
	if(sys_outb(RTC_DATA_REG, hrs) != OK) return 1;

	return 0;
}


int rtc_subscribe_int(){
	int hook_id_bckp = rtc->hook_id_rtc;

	//Subscribe interrupts
	if (sys_irqsetpolicy(RTC_IRQ_LINE, IRQ_EXCLUSIVE | IRQ_REENABLE, &rtc->hook_id_rtc) != OK) return 1;

	//Read Register_B in order to change interrupt bits
	unsigned long regB;
	unsigned char regBb;
	sys_outb(RTC_ADDR_REG, RTC_REG_B);
	sys_inb(RTC_DATA_REG, &regB);

	regBb = (unsigned char) regB;

	//Enable all interrupts
	regBb = regBb | RTC_UIE | RTC_AIE; //| RTC_PIE;

	//Write Register_B modified
	sys_outb(RTC_ADDR_REG, RTC_REG_B);
	sys_outb(RTC_DATA_REG, regBb);

	return BIT(hook_id_bckp);
}

int rtc_unsubscribe(){
	if(sys_irqrmpolicy(&rtc->hook_id_rtc) != OK) return 1;

	//Read Register_B in order to change interrupt bits
	unsigned long regB;
	if(sys_outb(RTC_ADDR_REG, RTC_REG_B) !=OK) return 1;
	if(sys_inb(RTC_DATA_REG, &regB) !=OK) return 1;

	//Disable all interrupts
	regB = regB & ~RTC_UIE & ~RTC_AIE & ~RTC_PIE;

	//Write Register_B modified
	if(sys_outb(RTC_ADDR_REG, RTC_REG_B) !=OK) return 1;
	if(sys_outb(RTC_DATA_REG, regB) !=OK) return 1;

	return 0;
}
