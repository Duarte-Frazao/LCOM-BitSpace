//Library
#include <minix/drivers.h>
#include <minix/driver.h>
#include <minix/com.h>
#include <minix/sysutil.h>
#include "i8042.h"
#include <stdbool.h>
#include <minix/syslib.h>
#include <minix/types.h>


void wait_esc_press();
int kbd_unsubscribe();
int kbd_subscribe();
int readScancode(unsigned char *scancode);
int outBufferFull();
int readStatus(unsigned long *status);
