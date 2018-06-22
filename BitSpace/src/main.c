#include <time.h>
#include <minix/drivers.h>
#include "BitSpace.h"

void updateBackground(Bitmap * bg, int worldX, int worldY){
	int worldXMod = worldX % bg->bitmapInfoHeader.width;
	if (worldXMod < 0)
		worldXMod += bg->bitmapInfoHeader.width;   // handle negative numbers

	int worldYMod = worldY % bg->bitmapInfoHeader.height;
	if (worldYMod < 0)
		worldYMod += bg->bitmapInfoHeader.height;   // handle negative numbers

	int tileLeft = -worldXMod;
	int tileTop = -worldYMod;

	int spriteTopLeftX = tileLeft;
	int spriteTopLeftY = tileTop;

	int spriteTopRightX = spriteTopLeftX + bg->bitmapInfoHeader.width;
	int spriteTopRightY = spriteTopLeftY;

	int spriteBottomLeftX = spriteTopLeftX;
	int spriteBottomLeftY = spriteTopLeftY + bg->bitmapInfoHeader.height;

	int spriteBottomRightX = spriteTopRightX;
	int spriteBottomRightY = spriteBottomLeftY;


	image(bg, spriteTopLeftX, spriteTopLeftY);
	//image(bg, spriteTopRightX, spriteTopRightY);
	image(bg, spriteBottomLeftX, spriteBottomLeftY);
	//image(bg, spriteBottomRightX, spriteBottomRightY);
}



int main(int argc, char **argv) {
	sef_startup();
    /* Enable IO-sensitive operations for ourselves */
    sys_enable_iop(SELF);
	vg_init(0x117);

	BitSpace *bitspace = (BitSpace *) malloc(sizeof(BitSpace));
	initGame(bitspace);
	do{
		intGame(bitspace);
		updateMouse();
		if (bitspace->timer->timerTickedFlag)
			GameBrain(bitspace);
	}while(!bitspace->done);


	endGame(bitspace);
	vg_exit();
	return 0;
}

