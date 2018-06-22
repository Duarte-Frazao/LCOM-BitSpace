#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>     //printf
#include <minix/sef.h> //sef_startup
#include <stdlib.h>    //strtoul
#include <minix/syslib.h>

#include "test3.h"


static int proc_args(int argc, char **argv);
static unsigned short parse_ushort(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char **argv)
{
	sef_startup();

    /* Enable IO-sensitive operations for ourselves */
	sys_enable_iop(SELF);

	if (argc == 1) {					/* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);
}


static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"scan <decimal no.- ASM = 0  C != 0>\"\n"
			"\t service run %s -args \"poll\"\n"
			"\t service run %s -args \"timed_scan <decimal no. - limit>\"\n",
			argv[0], argv[0], argv[0]);
}


static int proc_args(int argc, char **argv)
{
	unsigned short assembly, limit;
	if (strncmp(argv[1], "scan", strlen("scan")) == 0) {
		if (argc != 3) {
			printf("kbd: wrong no. of arguments for kbd_test_scan()\n");
			return 1;
		}
		assembly = parse_ushort(argv[2], 10);						/* Parses string to unsigned short */
		if (assembly == USHRT_MAX)
			return 1;
		printf("kbd::kbd_test_scan(%lu)\n", assembly);
		return kbd_test_scan(assembly);
	}
	else if (strncmp(argv[1], "poll", strlen("poll")) == 0) {
		if (argc != 2) {
			printf("kbd: wrong no. of arguments for kbd_test_poll()\n");
			return 1;
		}
		printf("kbd::kbd_test_poll()\n");
		return kbd_test_poll();
	}
	else if (strncmp(argv[1], "timed_scan", strlen("timed_scan")) == 0) {
		if (argc != 3) {
			printf("kbd: wrong no of arguments for kbd_test_timed_scan()\n");
			return 1;
		}
		limit = parse_ushort(argv[2], 10);						/* Parses string to unsigned short */
		if (limit == USHRT_MAX)
			return 1;
		printf("kbd::kbd_test_timed_scan(%lu)\n", limit);
		return kbd_test_timed_scan(limit);
	}
	else {
		printf("kbd: %s - no valid function!\n", argv[1]);
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
