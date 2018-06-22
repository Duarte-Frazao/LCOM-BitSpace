#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdbool.h>
#include <math.h>

#include "Keyboard.h"
#include "Utilities.h"
#include "SpaceShip.h"
#include "Uart.h"
#include "UartMacros.h"

inline int AngleToIndex(float angle){
	return n_rot / PI2 * angle;
}

inline float IndexToAngle(int index){
	return PI2 / n_rot * (float) index;
}

static Sprite * explosions[5];


void loadExplosions(){
	explosions[0] = create_sprite_Bitmap("/home/lcom/svn/BitSpace/res/images/explosion/explosion0.bmp", 0, 0, 0, 0);
	explosions[1] = create_sprite_Bitmap("/home/lcom/svn/BitSpace/res/images/explosion/explosion1.bmp", 0, 0, 0, 0);
	explosions[2] = create_sprite_Bitmap("/home/lcom/svn/BitSpace/res/images/explosion/explosion2.bmp", 0, 0, 0, 0);
	explosions[3] = create_sprite_Bitmap("/home/lcom/svn/BitSpace/res/images/explosion/explosion3.bmp", 0, 0, 0, 0);
	explosions[4] = create_sprite_Bitmap("/home/lcom/svn/BitSpace/res/images/explosion/explosion4.bmp", 0, 0, 0, 0);
}

SpaceShip* createSpaceShip(char * filename)
{
	SpaceShip* spaceship = (SpaceShip*) malloc(sizeof(SpaceShip));

	spaceship->done = 0;

	spaceship->image = create_sprite_Bitmap(filename, getHorRes() / 2, getVerRes(), 0, 0);

	spaceship->SpaceShipRot  =(unsigned short **) malloc(sizeof(unsigned short *) * n_rot);

	unsigned int i;
	for(i = 0; i < n_rot; i++){
		spaceship->SpaceShipRot[i] = (unsigned short *) malloc(spaceship->image->width * spaceship->image->height * getBytsPerPixel());
		create_rotate_sprite(spaceship->image,IndexToAngle(i), spaceship->SpaceShipRot[i]);
	}

	spaceship->image->xace = 0;
	spaceship->image->yace = 0;
	spaceship->angleIndex=0;
	spaceship->initialized = false;
	spaceship->laser = create_sprite_Bitmap("/home/lcom/svn/BitSpace/res/images/laser.bmp", 0, 0, 0, 0);

	spaceship->shootsSize =0;
	spaceship->shoots = NULL;
	return spaceship;

}

void newSpaceShipShoot(SpaceShip* spaceship){
	//Add shoot
	spaceship->shootsSize++;

	//Allocate memory in the array for the new shoot
	spaceship->shoots  = (Sprite **) realloc(spaceship->shoots, sizeof(Sprite*) * spaceship->shootsSize);
	spaceship->shoots[spaceship->shootsSize - 1] = (Sprite *) malloc(sizeof(Sprite));
	spaceship->shoots[spaceship->shootsSize - 1]->map = (char *) malloc(spaceship->laser->width * spaceship->laser->height * getBytsPerPixel());
	//Create image rotated
	unsigned short * mem = (unsigned short *)spaceship->shoots[spaceship->shootsSize - 1]->map;
	create_rotate_sprite(spaceship->laser, IndexToAngle(spaceship->angleIndex),mem);

	//Information for animation
	spaceship->shoots[spaceship->shootsSize - 1]->x = spaceship->image->x;
	spaceship->shoots[spaceship->shootsSize - 1]->y = spaceship->image->y;
	spaceship->shoots[spaceship->shootsSize - 1]->width = spaceship->laser->width;
	spaceship->shoots[spaceship->shootsSize - 1]->height =spaceship->laser->height;
	spaceship->shoots[spaceship->shootsSize - 1]->xspeed = -10*cos(IndexToAngle(spaceship->angleIndex) - PI_2);
	spaceship->shoots[spaceship->shootsSize - 1]->yspeed = 10*sin(IndexToAngle(spaceship->angleIndex) - PI_2);
}

void updateSpaceShipShoots(SpaceShip* spaceship){ 
	unsigned i;
	for(i = 0; i < spaceship->shootsSize; i++){
		spaceship->shoots[i]->x += spaceship->shoots[i]->xspeed;
		spaceship->shoots[i]->y += spaceship->shoots[i]->yspeed;

		if(spaceship->shoots[i]->x < 0 || spaceship->shoots[i]->x >= 1024 || spaceship->shoots[i]->y >= 768 || spaceship->shoots[i]->y < 0){
			free(spaceship->shoots[i]->map);
			free(spaceship->shoots[i]);
			spaceship->shootsSize--;

			//Put elements sequentially
			unsigned j;
			for(j = i; j < spaceship->shootsSize; j++) spaceship->shoots[j] = spaceship->shoots[j + 1];

			//Resize array
			spaceship->shoots  = (Sprite**) realloc(spaceship->shoots, sizeof(Sprite*) * spaceship->shootsSize);
		}
	}
}


void updateSpaceShipPos(SpaceShip* spaceship){
	spaceship->image->xspeed = constrain(spaceship->image->xspeed + spaceship->image->xace,-4,4);
	spaceship->image->yspeed = constrain(spaceship->image->yspeed + spaceship->image->yace,-4,4);

	spaceship->image->x+= spaceship->image->xspeed;
	spaceship->image->y+= spaceship->image->yspeed;
}

void updateSpaceShip(SpaceShip* spaceship)
{
	Keyboard *keyboard = getKeyboard();

	if(keyboard->state_A != STOP)
		spaceship->image->xace = -1;

	if(keyboard->state_S != STOP)
		spaceship->image->yace = 1;

	if(keyboard->state_D != STOP)
		spaceship->image->xace = 1;

	if(keyboard->state_W != STOP)
		spaceship->image->yace = -1;

	if (keyboard->state_W == STOP && keyboard->state_S == STOP){
		spaceship->image->yace = 0;
		spaceship->image->yspeed = 0;
	}

	if(keyboard->state_A == STOP && keyboard->state_D == STOP){
		spaceship->image->xace = 0;
		spaceship->image->xspeed = 0;
	}

	//Update position values
	updateSpaceShipPos(spaceship);

	//Shoots
	static int mousePressedBefore = 0;
	int click = !mousePressedBefore  &  getMouse()->leftButtonPressed;
	mousePressedBefore = getMouse()->leftButtonPressed;

	if(click) newSpaceShipShoot(spaceship);
	updateSpaceShipShoots(spaceship);
}

void drawShoots(SpaceShip *spaceship){
	unsigned i;
	for(i = 0; i < spaceship->shootsSize; i++)
		draw_sprite(spaceship->shoots[i]);
}

void drawSpaceShip(SpaceShip* spaceship)
{
	char * backup = spaceship->image->map;

	spaceship->image->map = (char *) spaceship->SpaceShipRot[spaceship->angleIndex];

	correctMargins(spaceship->image);
	draw_sprite(spaceship->image);

	spaceship->image->map = backup;


}

void initSequence(SpaceShip* spaceship){
	if (spaceship->image->y < 768 / 2 + 50){
		spaceship->initialized=true;
		return;
	}
	spaceship->image->y -= 3;
	drawSpaceShip(spaceship);
}

int lostSequence(SpaceShip* spaceship){
	static i = 0;
	static wait = 0;
	static x,y;
	if(i == 5) {i = 0; return 1;}
	if(i == 0){x = spaceship->image->x; y = spaceship->image->y;}



	Sprite * sp = explosions[i];
	sp->x=x; sp->y=y;
	if(wait++ == 4){
		i++;
		wait = 0;
	}

	draw_sprite(sp);

	return 0;
}


void resetSpaceShip(SpaceShip* spaceship){
	spaceship->done = spaceship->angleIndex = 0;
	spaceship->image->x = getHorRes()/2;
	spaceship->image->y = getVerRes();
	spaceship->image->xace = 0;
	spaceship->image->yace = 0;
	spaceship->initialized = false;

	unsigned int i;
	for(i = 0; i < spaceship->shootsSize; i++)
		free(spaceship->shoots[i]);

	spaceship->shootsSize = 0;
}

void eraseSpaceShip(SpaceShip* spaceship)
{
	unsigned int i ;
	if(spaceship->SpaceShipRot != NULL){
		for(i = 0; i < n_rot; i++)
			free(spaceship->SpaceShipRot[i]);
		free(spaceship->SpaceShipRot);
	}

	if (spaceship!=NULL) free(spaceship);
}

int sendPositionEvent(SpaceShip* spaceship)
{
	//Message allocate space for events and message type
	char* event = (char*) malloc(8*sizeof(char));

	//Header symbol for spaceship position
	event[0]=HEADER_SPACESHIP_POSITION_SYMBOL;

	event[1]=(char)((spaceship->image->x) >>8);
	event[2]=(char)(spaceship->image->x);
	event[3]=(char)((spaceship->image->y)>>8);
	event[4]=(char)(spaceship->image->y);
	event[5]=(char)((spaceship->angleIndex) >>8);
	event[6]=(char)(spaceship->angleIndex);

	static int mousePressedBefore = 0;
	int click = !mousePressedBefore  &  getMouse()->leftButtonPressed;
	mousePressedBefore = getMouse()->leftButtonPressed;

	if(click)event[7]=1;
	else event[7]=0;

	sendEvent(event);

	return 0;
	//event[2]= (char)(bitspace->spaceship->image->x);
}
