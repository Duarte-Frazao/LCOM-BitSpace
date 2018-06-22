#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>     //printf
#include <minix/sef.h> //sef_startup
#include <stdlib.h>    //strtoul
#include <minix/syslib.h>

#include "test5.h"
#include "pixmap.h"
#include "video_test.h"

static int proc_args(int argc, char **argv);
static unsigned short parse_ushort(char *str, int base);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char **argv)
{
	sef_startup();

	if (argc == 1) {/* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);
}


static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"init  <HEX no. - 16 bit VBE Mode> <decimal no. - delay in seconds>\"\n"
			"\t service run %s -args \"square <decimal no. - x coordinate> <decimal no. - y coordinate> <decimal no. - size in pixels> <HEX no. - color>\"\n"
			"\t service run %s -args \"line <decimal no. - xi coordinate> <decimal no. - yi coordinate> <decimal no. - xf coordinate> <decimal no. - yf coordinate> <HEX no. - color>\"\n",
			argv[0], argv[0], argv[0]);
}


static int proc_args(int argc, char **argv)
{
	unsigned short mode, x,y,xf,yf,size,framerate;
	unsigned long delay,color;
	short speed;

	if (strncmp(argv[1], "init", strlen("init")) == 0) {
		if (argc != 4) {
			printf("video: wrong no. of arguments for video_test_init()\n");
			return 1;
		}

		mode = parse_ushort(argv[2], 16);						/* Parses string to unsigned short */
		delay = parse_ulong(argv[3], 10);

		if (mode == USHRT_MAX)
			return 1;

		if (delay == ULONG_MAX)
			return 1;

		printf("video::video_test_init(%lu, %lu)\n", mode, delay);
		return video_test_init(mode, delay);
	}

	else if (strncmp(argv[1], "square", strlen("square")) == 0) {
		if (argc != 6) {
			printf("video: wrong no. of arguments for video_test_square()\n");
			return 1;
		}
		x = parse_ushort(argv[2], 10);						/* Parses string to unsigned short */
		if (x == USHRT_MAX)
			return 1;

		y = parse_ushort(argv[3], 10);
		if (y == USHRT_MAX)
			return 1;

		size = parse_ushort(argv[4], 10);
		if (size == USHRT_MAX)
			return 1;

		color = parse_ulong(argv[5], 16);
		if (color == ULONG_MAX)
			return 1;

		printf("video::video_test_square(%lu, %lu, %lu, %lu)\n", x,y,size,color);
		return video_test_square(x,y,size,color);
	}


	else if (strncmp(argv[1], "line", strlen("line")) == 0) {
		if (argc != 7) {
			printf("video: wrong no of arguments for mouse_test_remote()\n");
			return 1;
		}

		x = parse_ushort(argv[2], 10);						/* Parses string to unsigned short */
		if (x == USHRT_MAX)
			return 1;

		y = parse_ushort(argv[3], 10);
		if (y == USHRT_MAX)
			return 1;

		xf = parse_ushort(argv[4], 10);
		if (xf == USHRT_MAX)
			return 1;

		yf = parse_ushort(argv[5], 10);
		if (yf == USHRT_MAX)
			return 1;

		color = parse_ulong(argv[6], 16);
		if (color == ULONG_MAX)
			return 1;

		printf("video::video_test_line(%lu, %lu, %lu, %lu, %lu)\n", x,y,xf,yf,color);
		return video_test_line(x,y,xf,yf,color);
	}





	else if (strncmp(argv[1], "xpm", strlen("xpm")) == 0) {
		if (argc != 5) {
			printf("video: wrong no. of arguments for test_xpm()\n");
			return 1;
		}


		x = parse_ushort(argv[3], 10);						/* Parses string to unsigned short */
		if (x == USHRT_MAX)
			return 1;

		y = parse_ushort(argv[4], 10);
		if (y == USHRT_MAX)
			return 1;

		printf("video::test_xpm(%s, %lu, %lu)\n", argv[2], x,y);


		if (!strcmp(argv[2], "pic1"))
			return test_xpm(pic1, x, y);
		else if (!strcmp(argv[2], "pic2"))
			return test_xpm(pic2, x, y);
		else if (!strcmp(argv[2], "cross"))
			return test_xpm(cross, x, y);
		else if (!strcmp(argv[2], "pic3"))
			return test_xpm(pic3, x, y);
		else if (!strcmp(argv[2], "penguin"))
			return test_xpm(penguin, x, y);
		else{
			printf("XPM: %s not available\n",argv[2]);
			return 1;
		}

	}

	else if (strncmp(argv[1], "controller", strlen("controller")) == 0) {
		if (argc != 2) {
			printf("video: wrong no of arguments for test_controller()\n");
			return 1;
		}
		return test_controller();
	}

	else if (strncmp(argv[1], "move", strlen("move")) == 0) {

		if (argc != 9) {
			printf("video: wrong no. of arguments for test_move()\n");
			return 1;
		}


		x = parse_ushort(argv[3], 10);						/* Parses string to unsigned short */
		if (x == USHRT_MAX)
			return 1;

		y = parse_ushort(argv[4], 10);
		if (y == USHRT_MAX)
			return 1;

		xf = parse_ushort(argv[5], 10);						/* Parses string to unsigned short */
		if (xf == USHRT_MAX)
			return 1;

		yf = parse_ushort(argv[6], 10);
		if (yf == USHRT_MAX)
			return 1;

		speed = (short) parse_ushort(argv[7], 10);
		if (speed == USHRT_MAX)
			return 1;

		framerate = parse_ushort(argv[8], 10);
		if (framerate == USHRT_MAX)
			return 1;

		printf("video::test_move(%s, %lu, %lu, %lu, %lu, %lu, %lu)\n", argv[2], x,y,xf,yf,speed,framerate);


		if (!strcmp(argv[2], "pic1"))
			return test_move(pic1, x, y,xf,yf,speed,framerate);
		else if (!strcmp(argv[2], "pic2"))
			return test_move(pic2, x, y,xf,yf,speed,framerate);
		else if (!strcmp(argv[2], "cross"))
			return test_move(cross, x, y,xf,yf,speed,framerate);
		else if (!strcmp(argv[2], "pic3"))
			return test_move(pic3, x, y,xf,yf,speed,framerate);
		else if (!strcmp(argv[2], "penguin"))
			return test_move(penguin, x, y,xf,yf,speed,framerate);
		else{
			printf("XPM: %s not available\n",argv[2]);
			return 1;
		}

	}


	else {
		printf("video: %s - no valid function!\n", argv[1]);
		return 1;
		}
}

static unsigned short parse_ushort(char *str, int base)
{
	char *endptr;
	unsigned short val;

	/* Convert string to unsigned short */
	val = (unsigned short) strtoul(str, &endptr, base);

	/* Check for conversion errors */
	if ((errno == ERANGE && val == USHRT_MAX) || (errno != 0 && val == 0)) {
		perror("strtoul");
		return USHRT_MAX;
	}

	if (endptr == str) {
		printf("video: parse_ushort: no digits were found in %s\n", str);
		return USHRT_MAX;
	}

	/* Successful conversion */
	return val;
}


static unsigned long parse_ulong(char *str, int base)
{
	char *endptr;
	unsigned long val;

	/* Convert string to unsigned long */
	val = strtoul(str, &endptr, base);

	/* Check for conversion errors */
	if ((errno == ERANGE && val == ULONG_MAX) || (errno != 0 && val == 0)) {
		perror("strtoul");
		return ULONG_MAX;
	}

	if (endptr == str) {
		printf("video: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}
