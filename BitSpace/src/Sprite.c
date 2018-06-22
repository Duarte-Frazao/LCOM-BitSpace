#include <minix/syslib.h>
#include <minix/drivers.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#include "Sprite.h"
#include "Graphics.h"
#include "Bitmap.h"



//Functions
Sprite *create_sprite_Bitmap(const char *filename, int x, int y, int xspeed, int yspeed) {

	//allocate space for the "object"
	Sprite *sp = (Sprite *) malloc(sizeof(Sprite));
	if (sp == NULL)
		return (Sprite *) NULL;

	// read the sprite pixmap and dimensions
	Bitmap * bmp =  loadBitmap(filename);

	sp->map = bmp->bitmapData;
	sp->width = bmp->bitmapInfoHeader.width;
	sp->height = bmp->bitmapInfoHeader.height;

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

Sprite *copySprite(Sprite *spOri) {

	Sprite * sp = (Sprite *) malloc(sizeof(Sprite));
	memcpy(sp, spOri, sizeof(Sprite));

	unsigned int sizeImage = spOri->width * spOri->height * getBytsPerPixel();
	sp->map = (char *) malloc(sizeImage);
	memcpy(sp->map, spOri->map, sizeImage);

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

int draw_sprite(Sprite *sp) {

	unsigned short color_ignore;
	int width = sp->width;
	int drawWidth = width;
	int height = sp->height;
	int x = sp->x;
	int y = sp->y;
	 x -= width / 2;
	 y-= height/2;

	int xCorrection = 0;
	if (x < 0) {
		xCorrection = -x;
		drawWidth -= xCorrection;
		x = 0;

		if (drawWidth > getHorRes())
			drawWidth = getHorRes();
	}

	else if (x + drawWidth >= getHorRes()) {
		drawWidth = getHorRes() - x;
	}

	char* bufferStartPos;
	char* imgStartPos;

	int i;
	for (i = 0; i < height; i++) {

		int pos = y + height - 1 - i;

		if (pos < 0 || pos >= getVerRes())
			continue;

		bufferStartPos = getGraphicsBuf();
		bufferStartPos += x * 2 + pos * getHorRes() * 2;

		imgStartPos = sp->map + xCorrection * 2 + i * width * 2;


		unsigned short * imgPtr = (unsigned short *) imgStartPos;
		unsigned short * BuffPtr = (unsigned short *) bufferStartPos;
		unsigned int j;
		for(j = 0;  j< drawWidth; j++){
			unsigned short imgPix = *(imgPtr + j);

			if( (((imgPix & 0x07E0) >> 5) >= 0x10) && (((imgPix & 0x07E0) >> 5) <= 0x3F) &&
					((imgPix & 0x001F) <= 0x06) && (((imgPix & 0xF800) >> 11) <= 0x05)
			) continue;

			*(BuffPtr + j) = *(imgPtr + j);
		}
	}
}



int create_rotate_sprite(Sprite *sp, float angle, unsigned short * picBack) {

	unsigned short * pic = (unsigned short *) sp->map;

	int newX, newY;
	float cosA = cos(angle);
	float sinA = sin(angle);
	int midx = sp->width/2;
	int midy = sp->height/2;

	int x, y;
	for (y = 0; y < sp->height; y++) {
		for (x = 0; x < sp->width; x++) {
			unsigned short * dst = picBack + y * sp->width + x;


			newX = (x - midx) * cosA + (y - midy) * sinA;
			newY = -(x - midx) * sinA + (y - midy) * cosA;
			newX = floor(newX + 0.5) + midx;
			newY = floor(newY + 0.5) + midy;


			if (newX < 0 || newY < 0 || newX >= sp->width || newY >= sp->height){
				*dst = 0x07E0;
				continue;
			}

			unsigned short * src = pic + newY* sp->width + newX;
			(*dst) = (*src);
		}
	}
	return 0;
}


void drawMainSprite(Sprite *sp, char *testing_ground)
{
	unsigned short color_ignore;
	int width = sp->width;
	int drawWidth = width;
	int height = sp->height;
	int x = sp->x;
	int y = sp->y;
	 x -= width / 2;
	 y-= height/2;

	int xCorrection = 0;
	if (x < 0) {
		xCorrection = -x;
		drawWidth -= xCorrection;
		x = 0;

		if (drawWidth > getHorRes())
			drawWidth = getHorRes();
	}

	else if (x + drawWidth >= getHorRes()) {
		drawWidth = getHorRes() - x;
	}

	char* bufferStartPos;
	char* imgStartPos;

	int i;
	for (i = 0; i < height; i++) {

		int pos = y + height - 1 - i;

		if (pos < 0 || pos >= getVerRes())
			continue;

		bufferStartPos = testing_ground;
		bufferStartPos += x * 2 + pos * getHorRes() * 2;

		imgStartPos = sp->map + xCorrection * 2 + i * width * 2;


		unsigned short * imgPtr = (unsigned short *) imgStartPos;
		unsigned short * BuffPtr = (unsigned short *) bufferStartPos;
		unsigned int j;
		for(j = 0;  j< drawWidth; j++){
			unsigned short imgPix = *(imgPtr + j);

			if( (((imgPix & 0x07E0) >> 5) >= 0x10) && (((imgPix & 0x07E0) >> 5) <= 0x3F) &&
					((imgPix & 0x001F) <= 0x06) && (((imgPix & 0xF800) >> 11) <= 0x05)
			) continue;

			*(BuffPtr + j) = *(imgPtr + j);
		}
	}

}

int checkSpriteCollision(Sprite *sp, char *testing_ground)
{
	unsigned short color_ignore;
	int width = sp->width;
	int drawWidth = width;
	int height = sp->height;
	int x = sp->x;
	int y = sp->y;
	 x -= width / 2;
	 y-= height/2;

	int xCorrection = 0;
	if (x < 0) {
		xCorrection = -x;
		drawWidth -= xCorrection;
		x = 0;

		if (drawWidth > getHorRes())
			drawWidth = getHorRes();
	}

	else if (x + drawWidth >= getHorRes()) {
		drawWidth = getHorRes() - x;
	}

	char* bufferStartPos;
	char* imgStartPos;

	int i;
	for (i = 0; i < height; i++) {

		int pos = y + height - 1 - i;

		if (pos < 0 || pos >= getVerRes())
			continue;

		bufferStartPos = testing_ground;
		bufferStartPos += x * 2 + pos * getHorRes() * 2;

		imgStartPos = sp->map + xCorrection * 2 + i * width * 2;


		unsigned short * imgPtr = (unsigned short *) imgStartPos;
		unsigned short * BuffPtr = (unsigned short *) bufferStartPos;
		unsigned int j;
		for(j = 0;  j< drawWidth; j++){
			unsigned short imgPix = *(imgPtr + j);

			if( (((imgPix & 0x07E0) >> 5) >= 0x10) && (((imgPix & 0x07E0) >> 5) <= 0x3F) &&
					((imgPix & 0x001F) <= 0x06) && (((imgPix & 0xF800) >> 11) <= 0x05)
			) continue;
			if(*(BuffPtr + j) != 0x0000 && *(BuffPtr + j) != 0x07E0) return 1;
		}
	}
	return 0;

}

//Checks for a collision between 2 sprites, returns 0 when no collision is detected
int collision(Sprite *obj1, Sprite *obj2)
{
	int obj1Left = obj1->x- obj1->width/2 ;
	int obj1Right = obj1->x+ obj1->width/2;

	int obj2Left = obj2->x - obj2->width/2;
	int obj2Right= obj2->x + obj2->width/2;

	int obj1Top= obj1->y- obj1->height/2 ;
	int obj1Bot=  obj1->y + obj1->height/2;

	int obj2Top= obj2->y - obj2->height/2;
	int obj2Bot= obj2->y + obj2->height/2;

	//Checks if there is no collision by horizontal borders
	if(obj1Right < obj2Left|| obj1Left > obj2Right) return 0;
	//Checks if there is no collision by vertical borders
	if(obj1Bot < obj2Top || obj1Top> obj2Bot) return 0;

	int leftMargin,rightMargin, topMargin, botMargin;

	leftMargin= (obj1Left <obj2Left) ? obj1Left : obj2Left;
	rightMargin= (obj1Right >obj2Right) ? obj1Right : obj2Right;
	topMargin= (obj1Top <obj2Top) ? obj1Top : obj2Top;
	botMargin=  (obj1Bot >obj2Bot) ? obj1Bot : obj2Bot;

	int height=(botMargin-topMargin);
	int width= (rightMargin-leftMargin);

	char *testing_ground= (char *) malloc(getVerRes()*getHorRes()*getBytsPerPixel());
	memset(testing_ground,0,getVerRes()*getHorRes()*getBytsPerPixel());


	drawMainSprite(obj1, testing_ground);


	int res = checkSpriteCollision(obj2,testing_ground);
	free(testing_ground);
	return res;
}

void correctMargins(Sprite *image)
{
	//Corrigir margens
	if (image->x < image->width/2) image->x= image->width/2;
	if (image->x >getHorRes() - image->width/2) image->x = getHorRes() -image->width/2 ;

	if (image->y < image->height/2) image->y = image->height/2;
	if (image->y >getVerRes() - image->height/2 ) image->y = getVerRes() - image->height/2;
}



void resetAsteroid(Sprite *sp, Sprite * spaceship, int resetSpeed) {
	static speed = 5;
	static waitIncrease = 5;

	if (resetSpeed)
		speed = 5;

	sp->x = rand() % (getHorRes() - sp->width / 2) + sp->width / 2;
	sp->y = - rand() % (getVerRes() - sp->height / 2) - getVerRes();
	int dx = rand() % 50 - 25;//(spaceship->x - sp->x);
	int dy = rand() % 50; //(spaceship->y - sp->y);

	if (--waitIncrease < 0) {
		waitIncrease = 20;
		speed++;
	}
	sp->xspeed = dx * speed / sqrt(dx * dx + dy * dy);
	sp->yspeed = dy * speed / sqrt(dx * dx + dy * dy);
}


void Asteroid_update(Sprite * sp,Sprite * spaceship){
	sp->x += sp->xspeed;
	sp->y += sp->yspeed;

	if(sp->x < 0 || sp->x > 1024 || sp->y > 768)
		resetAsteroid(sp, spaceship, 0);
}
