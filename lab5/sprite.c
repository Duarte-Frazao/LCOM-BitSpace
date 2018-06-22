#include <minix/syslib.h>
#include <minix/drivers.h>
#include "sprite.h"
#include "video_gr.h"
#include "video_test.h"

//Functions
char * read_xpm(char *pic[], int * width, int * height);


Sprite *create_sprite(char *pic[], int x, int y, int xspeed, int yspeed) {

	//allocate space for the "object"
	Sprite *sp = (Sprite *) malloc(sizeof(Sprite));
	if (sp == NULL)
		return (Sprite *) NULL;

	// read the sprite pixmap and dimensions
	sp->map = read_xpm(pic, &(sp->width), &(sp->height));
	if (sp->map == NULL) {
		free(sp);
		return NULL;
	}

	// current position
	sp->x = x;
	sp->y = y;

	// current speed
	sp->xspeed= xspeed;
	sp->yspeed= yspeed;

	return sp;
}

void destroy_sprite(Sprite *sp) {
	if (sp == NULL)
		return;
	if (sp->map)
		free(sp->map);
	free(sp);
	sp = NULL; // XXX: pointer is passed by value
	// should do this @ the caller
}

int animate_sprite(Sprite *sp) {

	if(draw_sprite(sp) != OK)
		return 1;
	else{
	sp->x += sp->xspeed;
	sp->y += sp->yspeed;
	return 0;
	}
}

int draw_sprite(Sprite *sp) {
	int isOK = OK;

	unsigned int i;
	for (i = sp->y; i < sp->height + sp->y; i++) {
		unsigned int j;
		for (j = sp->x; j < sp->width + sp->x; j++) {
			if(paint_pixel(j, i, sp->map[(i - sp->y) * sp->width + (j - sp->x)]) != OK)
				isOK =  !OK;
		}
	}
	return isOK;
}


//
//
//
///* Some useful non-visible functions */

//
//static int check_collision(Sprite *sp, char *base) {
//	return 1;
//}
