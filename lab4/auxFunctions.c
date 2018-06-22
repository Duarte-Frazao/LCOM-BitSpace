#include "Header.h"

void printPacket(unsigned char *packet) {
	//Display of the bytes information
	printf("B1= 0x%02x B2= 0x%02x B3= 0x%02x  LB= %d MB= %d RB= %d XOV= %d YOV= %d  ",
			packet[0], packet[1], packet[2], (packet[0] & BIT(0)),
			(packet[0] & BIT(2)) >> 2, (packet[0] & BIT(1)) >> 1,
			(packet[0] & BIT(6)) >> 6, (packet[0] & BIT(7)) >> 7);


	//Display of the x position (If it's negative we do 256 - packet , which is the equivalent to transform from 2's complement)
	printf("X= ");
	if (packet[0] & BIT(4))
		printf("%4d  ", -(256 - packet[1]));
	else
		printf("%4d  ", packet[1]);

	//Display of the y position
	printf("Y= ");
	if (packet[0] & BIT(5))
		printf("%4d  ", -(256 - packet[2]));
	else
		printf("%4d ", packet[2]);

	printf("\n");
}
