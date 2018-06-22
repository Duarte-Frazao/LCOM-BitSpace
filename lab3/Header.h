//Library
#include <minix/drivers.h>
#include <minix/driver.h>
#include <minix/com.h>
#include <minix/sysutil.h>
#include "i8042.h"
#include "test3.h"
#include <stdbool.h>
#include "i8254.h"
#include <minix/syslib.h>
#include <minix/types.h>

//Functions
void printCode();
void kbd_handler_C();
void kbd_handler_ASM();

int readStatus(unsigned long *status);
int outBufferFull();
int readScancode(unsigned char *scancode);
int kbd_subscribe();
int kbd_unsubscribe();
void addDelay();
int enableInterrupts();
int disableInterrupts();

int timer_subscribe_int(void);
int timer_unsubscribe_int(void);
void timer_int_handler();


void addDelay();
int enableInterrupts();
int disableInterrupts();

void printCode();

