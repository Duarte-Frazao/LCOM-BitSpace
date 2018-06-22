#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>     //printf
#include <minix/sef.h> //sef_startup
#include <stdlib.h>    //strtoul
#include <minix/syslib.h>

#include "test4.h"


static int proc_args(int argc, char **argv);
static unsigned short parse_ushort(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char **argv)
{
	sef_startup();

	if (argc == 1) {					/* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);
}


static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"packet <decimal no. - Number of packets to receive>\"\n"
			"\t service run %s -args \"async <decimal no. - Number of seconds with no packets before exiting>\"\n"
			"\t service run %s -args \"remote <decimal no. - Period in milliseconds> <decimal no. - Number packets>\"\n",
			argv[0], argv[0], argv[0]);
}


static int proc_args(int argc, char **argv)
{
	unsigned short cnt_packet, idle_time, cnt_remote, period, length;
	if (strncmp(argv[1], "packet", strlen("packet")) == 0) {
		if (argc != 3) {
			printf("mouse: wrong no. of arguments for mouse_test_packet()\n");
			return 1;
		}
		cnt_packet = parse_ushort(argv[2], 10);						/* Parses string to unsigned short */
		if (cnt_packet == USHRT_MAX)
			return 1;
		printf("mouse::mouse_test_packet(%lu)\n", cnt_packet);
		return mouse_test_packet(cnt_packet);
	}
	else if (strncmp(argv[1], "async", strlen("async")) == 0) {
		if (argc != 3) {
			printf("mouse: wrong no. of arguments for mouse_test_async()\n");
			return 1;
		}
		idle_time = parse_ushort(argv[2], 10);						/* Parses string to unsigned short */
		if (idle_time == USHRT_MAX)
			return 1;
		printf("mouse::mouse_test_async(%lu)\n", idle_time);
		return mouse_test_async(idle_time);
	}
	else if (strncmp(argv[1], "remote", strlen("remote")) == 0) {
		if (argc != 4) {
			printf("mouse: wrong no of arguments for mouse_test_remote()\n");
			return 1;
		}
		period = parse_ushort(argv[2], 10);						/* Parses string to unsigned short */
		cnt_remote = parse_ushort(argv[3], 10);						/* Parses string to unsigned short */
		if (period == USHRT_MAX || cnt_remote == USHRT_MAX)
			return 1;
		printf("mouse::mouse_test_remote(%lu,%lu)\n", period, cnt_remote);
		return mouse_test_remote(period, cnt_remote);
	}
	else if (strncmp(argv[1], "gesture", strlen("gesture")) == 0) {
		if (argc != 3) {
			printf("mouse: wrong no of arguments for mouse_test_gesture()\n");
			return 1;
		}
		length = parse_ushort(argv[2], 10);						/* Parses string to unsigned short */
		if (length == USHRT_MAX )
			return 1;
		printf("mouse::mouse_test_gesture(%lu,%lu)\n", length);
		return mouse_test_gesture(length);
	}
	else {
		printf("mouse: %s - no valid function!\n", argv[1]);
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
		printf("kbd: parse_ushort: no digits were found in %s\n", str);
		return USHRT_MAX;
	}

	/* Successful conversion */
	return val;
}

