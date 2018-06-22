COMPILER_TYPE = gnu

CC = gcc

PROG = BitSpace
SRCS = main.c Bitmap.c Graphics.c vbe.c Sprite.c SpaceShip.c Timer.c Mouse.c Keyboard.c Utilities.c Score.c BitSpace.c RTC.c Queue.c Uart.c rtc_asm.S

CCFLAGS= -Wall -Ofast -ftree-loop-distribution -fprefetch-loop-arrays

DPADD += ${LIBDRIVER} ${LIBSYS}
LDADD += -llm -ldriver -lsys

LDFLAGS += -L.

MAN=
BINDIR? = /usr/sbin

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
