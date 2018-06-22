#include <minix/drivers.h>
#include <minix/driver.h>
#include <minix/com.h>
#include <minix/sysutil.h>
#include <stdbool.h>
#include <minix/syslib.h>
#include <sys/mman.h>
#include <machine/int86.h>
#include "video_test.h"

#include "video.h"
#include "video_gr.h"


char * read_xpm(char *pic[], int * width, int * height){

	unsigned int i,lastPos;

	/* read width, height, colors */
	int colors;

	for(i = lastPos = 0; i < strlen(pic[0])+1; i++){
		if(pic[0][i] == ' '){
			pic[0][i] = '\0';

			if(lastPos == 0) *width = atoi(pic[0]+lastPos);
			else *height = atoi(pic[0]+lastPos);

			pic[0][i] = ' ';
			lastPos = i+1;
		}

		else if(pic[0][i] == '\0'){
			colors = atoi(pic[0]+lastPos);
		}
	}


	//256 -  Number of ascii characters
	unsigned char colorsMap[256];

	for(i = 0; i < colors; i++){
		char symbol;
		unsigned int col;
		sscanf(pic[i+1], "%c %d", &symbol, &col);
		colorsMap[(unsigned char) symbol] = (unsigned char)col;
		printf("Index:%d, %c\n",symbol,symbol);
		printf("Color:%d\n\n",col);
	}

	// 8 : bits_per_pixel
	char * pixmap = (char *) malloc(((*width) * (*height) * 8)>>3);
	char * beginPixmap = pixmap;

	for(i = 0; i < *height; i++){
		unsigned int j;

		for(j = 0; j < (*width); j++){
			char symbol = pic[i+colors+1][j];
			pixmap[i * (*width) + j] = colorsMap[(unsigned char) symbol];
		}
	}

	return beginPixmap;
}
