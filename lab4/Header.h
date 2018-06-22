#include <minix/drivers.h>
#include <minix/driver.h>
#include <minix/com.h>
#include <minix/sysutil.h>
#include "test4.h"
#include "i8042.h"
#include "i8254.h"
#include <stdbool.h>
#include <minix/syslib.h>

int mouse_test_packet(unsigned short cnt);
int mouse_test_async(unsigned short idle_time);
int mouse_test_remote(unsigned long period, unsigned short cnt);
int mouse_test_gesture(short length);
void packet_handler();
int readPacket();


int disableInterruptsMouse();
int enableInterruptsMouse();
int mouse_subscribe();
int mouse_unsubscribe();
int mouseConfiguration(short cmd);
void addDelay();
int clear_IN_OUT_BUFF();
void printPacket(unsigned char *packet);


int timer_subscribe_int(void);
int timer_unsubscribe_int(void);
void timer_int_handler();


int disableIH();
int enableIH();

int kbd_subscribe();
int kbd_unsubscribe();

void packet_handler_poll();

int outBufferFull();

