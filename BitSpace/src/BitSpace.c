#include <minix/syslib.h>
#include <math.h>

#include "BitSpace.h"
#include "RTC_Macros.h"
#include "TimerMacros.h"
#include "Graphics.h"
#include "Uart.h"
#include "Queue.h"
#include "UartMacros.h"

static BitSpace* bitspaceObj;
BitSpace* getBitSpace(){return bitspaceObj;}

void initGame(BitSpace *bitspace){
	unsigned int i;
	bitspaceObj=bitspace;
	//Loads numbers images
	loadNumbers();
	loadExplosions();

	/*********Background********/
	bitspace->bg = loadBitmap("/home/lcom/svn/BitSpace/res/images/bg.bmp");
	/***************************/


	/**********Spaceship********/
	bitspace->spaceships = (SpaceShip **) malloc(sizeof(SpaceShip*) * NUM_SPACESHIPS);
	bitspace->spaceships[0] = (SpaceShip *) createSpaceShip("/home/lcom/svn/BitSpace/res/images/spaceship/spaceship0.bmp");
	bitspace->spaceships[1] = (SpaceShip *) createSpaceShip("/home/lcom/svn/BitSpace/res/images/spaceship/spaceship1.bmp");
	bitspace->spaceships[2] = (SpaceShip *) createSpaceShip("/home/lcom/svn/BitSpace/res/images/spaceship/spaceship2.bmp");
	bitspace->spaceship = bitspace->spaceships[0];


	/**********Multiplayer********/
	bitspace->spaceship2 = bitspace->spaceships[1];
	bitspace->multiplayer = 0;
	/***************************/


	/**********Asteroids********/
	bitspace->asteroids = (Sprite**) malloc(NUM_ASTEROIRDS * sizeof(Sprite *));
	bitspace->asteroids[0] = create_sprite_Bitmap("/home/lcom/svn/BitSpace/res/images/asteroid.bmp", 0, 0, 0, 0);

	for (i = 1; i < NUM_ASTEROIRDS; i++)
		bitspace->asteroids[i] = copySprite(bitspace->asteroids[0]);
	/***************************/


	/***********Score***********/
	bitspace->score = initialize_score();
	/***************************/


	/*********MainMenu**********/
	bitspace->mainmenu = loadBitmap("/home/lcom/svn/BitSpace/res/images/mainmenu.bmp");
	/***************************/

	/*********Pause Menu********/
	bitspace->pause = create_sprite_Bitmap("/home/lcom/svn/BitSpace/res/images/pausemenu.bmp", getHorRes()/2 , getVerRes()/2, 0, 0);
	bitspace->screen = (char *) malloc(getHorRes() * getVerRes()*getBytsPerPixel());
	/***************************/


	/********Leaderboard*********/
	bitspace->leaderboard = loadBitmap("/home/lcom/svn/BitSpace/res/images/leaderboard.bmp");
	/***************************/


	/***********Alerts**********/
	bitspace->alerts = (Sprite**) malloc(NUM_ALERTS * sizeof(Sprite *));
	char filenameAlert[] = "/home/lcom/svn/BitSpace/res/images/alert/alertX.bmp";
	for (i = 0; i < NUM_ALERTS; i++){
		filenameAlert[strlen(filenameAlert) - 5] = (char) (i + '0');
		bitspace->alerts[i] = create_sprite_Bitmap(filenameAlert, 164 + 8 , 32 + 2, 0, 0);}
	/***************************/




	/**********Devices**********/

	/***********Timer***********/
	newTimer();
	enableTimer();
	bitspace->timer = getTimer();
	/***************************/


	/***********Mouse***********/
	newMouse();
	enableMouse();
	bitspace->mouse = getMouse();
	/***************************/


	/************Kbd************/
	newKeyboard();
	enableKeyboard();
	bitspace->keyboard = getKeyboard();
	/***************************/


	/************RTC************/
	newRtc();
	enableRtc();
	bitspace->rtc = getRtc();
	//Every minute interrupt xx:xx:00
	rtc_set_alarm(RTC_DTC, RTC_DTC, 0);
	/***************************/


	/************UART************/
	newUart();
	enableUart();
	//eraseUart(); //Prevents Uart errors
	newUart();
	enableUart();
	bitspace->uart = getUart();
	/***************************/


	//Initial GameState
	bitspace->state = MAINMENU;
	bitspace->done = 0;
	bitspace->gameover = create_sprite_Bitmap("/home/lcom/svn/BitSpace/res/images/gameover.bmp", getHorRes()/2 , getVerRes()/2, 0, 0);

}

void resetGame(BitSpace *bitspace){

	/**********Devices**********/

	/***********Timer***********/
	resetTimer();


	/***********Mouse***********/
	resetMouse();


	/************Kbd************/
	resetKeyboard();


	/************Uart************/
	//resetUart();

	/**********Spaceship********/
	resetSpaceShip(bitspace->spaceship);

	/**********Multiplayer********/
	if(bitspace->multiplayer)
		resetSpaceShip(bitspace->spaceship2);
	bitspace->multiplayer = 0;

	/**********Asteroids********/
	unsigned int i;
	for (i = 0; i < NUM_ASTEROIRDS; i++)
		resetAsteroid(bitspace->asteroids[i], bitspace->spaceship->image, 1);

	/***********Score***********/
	reset_score();

	//Asteroids same position
	//srand(1);
}

void endGame(BitSpace *bitspace){
	unsigned int i;

	/*********Background********/
	deleteBitmap(bitspace->bg);


	/**********Spaceship********/
	for (i = 0; i < NUM_SPACESHIPS; i++)
		eraseSpaceShip(bitspace->spaceships[i]);


	/**********Asteroids********/
	for (i = 0; i < NUM_ASTEROIRDS; i++)
		destroy_sprite(bitspace->asteroids[i]);
	free(bitspace->asteroids);


	/***********Score***********/
	delete_score();


	/*********MainMenu**********/
	deleteBitmap(bitspace->mainmenu);



	/**********Devices**********/

	/***********Timer***********/
	eraseTimer();


	/***********Mouse***********/
	eraseMouse();


	/************Kbd************/
	eraseKeyboard();


	/************RTC************/
	eraseRtc();

	/***********UART************/
	eraseUart();

	/*********Pause Menu********/
	destroy_sprite(bitspace->pause);
	free(bitspace->screen);
	/***************************/

	free(bitspace);
}

void intGame(BitSpace *bitspace){
	int r = 0;
	message msg;
	int ipc_status;

	resetTimerTickedFlag();

	if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
		printf("driver_receive failed with: %d", r);
	}

	else {
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */

				if(bitspace->multiplayer)
				{
					if (msg.NOTIFY_ARG & bitspace->uart->IRQ_SET_UART)
						uart_handler();
				}

				if (msg.NOTIFY_ARG & bitspace->timer->IRQ_SET_TIMER)
					timer_int_handler();

				if (msg.NOTIFY_ARG & bitspace->keyboard->IRQ_SET_KBD)
					keyboard_handler();

				if (msg.NOTIFY_ARG & bitspace->mouse->IRQ_SET_MOUSE)
					packet_handler();

				if (msg.NOTIFY_ARG & bitspace->rtc->IRQ_SET_RTC)
					rtc_asm_handler();

				break;
			default:
				break; /* no other notifications expected: do nothing */
			}

		}
	}
}

ev_type_t MainMenu(BitSpace *bitspace){
	static int alertIndex = -1;
	static sec = 0;
	static spaceshipIndex = 0;
	static spaceshipUnlocked = 1;
	static int mousePressedBefore = 0;


	//Update

	//Center spaceship selected
	bitspace->spaceship = bitspace->spaceships[spaceshipIndex];
	bitspace->spaceship->image->x = getHorRes()/2;
	bitspace->spaceship->image->y = getVerRes()/2 - 40;
	bitspace->spaceship->angleIndex = 0;

	updateMouse();

	//Determine if the left button was pressed
	int click = !mousePressedBefore  & bitspace->mouse->leftButtonPressed;
	mousePressedBefore =  bitspace->mouse->leftButtonPressed;

	//Position of the mouse
	int x = bitspace->mouse->x;
	int y = bitspace->mouse->y;

	//Time that alert stay on screen
	if(bitspace->timer->numInterruptions % MINIX_FREQ == 0) sec--;

	//RTC Alarm Interrupts
	if(bitspace->rtc->alarm_int){
		resetRtc();
		if(alertIndex < NUM_ALERTS && spaceshipUnlocked < NUM_SPACESHIPS){
			spaceshipUnlocked++;
			alertIndex++;
			sec = 5;
		}
	}



	//Draw
	image(bitspace->mainmenu, 0, 0);
	if(sec > 0) draw_sprite(bitspace->alerts[alertIndex]);
	drawSpaceShip(bitspace->spaceship);
	drawMouse(bitspace->state);
	copyBufferToDisplay();
	resetMouseD();


	//Buttons

	if(bitspace->keyboard->ESC_press) return EX_BUTTON;

	//Calculate index of the selected spaceship
	if(((x-600)*(x-600) + (y-350)*(y-350) < (26/2)*(26/2)) & click)
		spaceshipIndex = (spaceshipIndex + 1) % spaceshipUnlocked;

	else if(((x-430)*(x-430) + (y-350)*(y-350) < (26/2)*(26/2)) & click){
		spaceshipIndex = (spaceshipIndex - 1) % spaceshipUnlocked;
		if(spaceshipIndex < 0 ) spaceshipIndex = -spaceshipIndex;}

	//Menu Buttons
	else if(((x-511)*(x-511) + (y-605)*(y-605) < (224/2)*(224/2)) & click)
		return PLAY_BUTTON;


	else if(((x-744)*(x-744) + (y-605)*(y-605) < (152/2)*(152/2)) & click)
		return MULTIPLAYER_BUTTON;


	else if(((x-278)*(x-278) + (y-605)*(y-605) < (152/2)*(152/2)) & click)
		return LEADERBOARD_BUTTON;

	return NULL_EV;

}

ev_type_t LeaderBoard(BitSpace *bitspace){
	static int mousePressedBefore = 0;

	//Get numbers sprite
	Sprite ** numebers = getNumbers();


	//Mouse update
	updateMouse();
	int x = bitspace->mouse->x;
	int y = bitspace->mouse->y;
	int click = !mousePressedBefore  & bitspace->mouse->leftButtonPressed;
	mousePressedBefore = bitspace->mouse->leftButtonPressed;

	//Draw
	image(bitspace->leaderboard, 0, 0);

	//Draw position, date and score of the best plays
	unsigned int posX;
	unsigned int posY = 290;
	unsigned int i;
	for(i = 0; i < 5; i++){
		Rank *rk = &bitspace->ranks[i];
		if(rk->score == 0) break;

		posX = 171;
		draw_num(i + 1, &posX, &posY,1);

		posX = 630;
		draw_date(rk->date, posX, posY);

		posX = 870;
		draw_num(rk->score, &posX, &posY,6);

		posY += 40;
	}

	drawMouse(bitspace->state);
	copyBufferToDisplay();
	resetMouseD();


	//Button
	if(((x-93)*(x-93) + (y-675)*(y-675) < (150/2)*(150/2)) & click)
		return HOME_BUTTON;

	else return NULL_EV;
}

ev_type_t PauseMenu(BitSpace *bitspace){
	static int mousePressedBefore = 0;

	//Mouse update
	updateMouse();
	int x = bitspace->mouse->x;
	int y = bitspace->mouse->y;
	int click = !mousePressedBefore  & bitspace->mouse->leftButtonPressed;
	mousePressedBefore = bitspace->mouse->leftButtonPressed;

	//Draw pause menu
	//Draw blur screen
	memcpy(getGraphicsBuf(), bitspace->screen, getVerRes()*getHorRes()*getBytsPerPixel());
	draw_sprite(bitspace->pause);
	drawMouse(bitspace->state);
	copyBufferToDisplay();
	resetMouseD();

	//Buttons
	if(((x-getHorRes()/2)*(x-getHorRes()/2) + (y-getVerRes()/2)*(y-getVerRes()/2) < (106/2)*(106/2)) & click)
		return PLAY_BUTTON;

	else if(((x-(getHorRes()/2-122))*(x-(getHorRes()/2-122)) + (y-getVerRes()/2)*(y-getVerRes()/2) < (72/2)*(72/2)) & click)
		return RESET_BUTTON;

	else if(((x-(getHorRes()/2+122))*(x-(getHorRes()/2+122)) + (y-getVerRes()/2)*(y-getVerRes()/2) < (72/2)*(72/2)) & click)
		return HOME_BUTTON;

	else return NULL_EV;
}

ev_type_t SaveScore(BitSpace *bitspace){

	ev_type_t ev = NULL_EV;

	unsigned int i;
	for(i = 0; i < 5; i++){

		//If spot is empty
		if(bitspace->ranks[i].score == 0){
			bitspace->ranks[i].score = bitspace->score->actual_score;
			bitspace->ranks[i].date = getDate();
			return ev;
		}
		//If score is bigger than the one already there
		else if(bitspace->score->actual_score > bitspace->ranks[i].score){
			unsigned int j;
			for(j = 4; j > i; j--) bitspace->ranks[j] = bitspace->ranks[j-1];
			bitspace->ranks[i].date = getDate();
			bitspace->ranks[i].score = bitspace->score->actual_score;
			return ev;
		}

	}
	return ev;
}

ev_type_t Multiplayer(BitSpace *bitspace){

	bitspace->multiplayer = 1;
	//Initial synchronization
	printf("synchronization\n" );
	printReceiveQueue();
	clearReceiveFifo();
	while(!isEmptyQueue(bitspace->uart->receiveQueue))
		popQueue(bitspace->uart->receiveQueue);

	printReceiveQueue();
	sendHostMessage();
	int syncro=0;
	while(!syncro)
	{
		keyboard_handler();
		if(bitspace->keyboard->ESC_press){
			resetKeyboard();
			return EX_BUTTON;
		}
		clearReceiveFifo();
		if(!isEmptyQueue(bitspace->uart->receiveQueue))
		{
			if(firstValueQueue(bitspace->uart->receiveQueue)==HEADER_HOST_SYMBOL){bitspace->host=1;syncro=1;}
			else if(firstValueQueue(bitspace->uart->receiveQueue)==HEADER_ACK_SYMBOL){bitspace->host=0;syncro=1;}
			else popQueue(bitspace->uart->receiveQueue);
		}
	}
	sendAck();
	popQueue(bitspace->uart->receiveQueue);
	printReceiveQueue();
	printf("---------------------\n");

	//Changes spaceships on client
	switchControlledSpaceShip(bitspace);

	//Starting at the same time
	resetTimer();

	return PLAY_BUTTON;
}

ev_type_t Game(BitSpace *bitspace){
	static int bgX = 0;
	static int bgY = 0;

	//Update
	updateBackground(bitspace->bg, bgX, bgY-=5);


	if(bitspace->multiplayer)
		if (!bitspace->spaceship2->initialized)
			initSequence(bitspace->spaceship2);

	if (!bitspace->spaceship->initialized){
			initSequence(bitspace->spaceship);
			copyBufferToDisplay();
			return;}


	update_keyboard();
	updateMouse();
	update_score();

	//Detect collision between spaceship and asteroids
	unsigned int i;
	for (i = 0; i < NUM_ASTEROIRDS; i++) {

		if (collision(bitspace->spaceship->image, bitspace->asteroids[i])) {

			bitspace->spaceship->done = 1;

			if (bitspace->multiplayer)
				sendDoneMessage();
		}

		if (bitspace->multiplayer) {
		if (collision(bitspace->spaceship2->image, bitspace->asteroids[i])) {

			bitspace->spaceship2->done = 1;

			if (bitspace->multiplayer)
				sendDoneMessage();
		}
		}

		Asteroid_update(bitspace->asteroids[i], bitspace->spaceship->image);
		draw_sprite(bitspace->asteroids[i]);
	}


	//Detect collision between shoots and asteroids
	unsigned int j;
	for (i = 0; i < NUM_ASTEROIRDS; i++) {
		for (j = 0; j < bitspace->spaceship->shootsSize; j++) {
			if (collision(bitspace->spaceship->shoots[j], bitspace->asteroids[i])){
				resetAsteroid(bitspace->asteroids[i],bitspace->spaceship->image, 0);
				free(bitspace->spaceship->shoots[j]);
				bitspace->spaceship->shootsSize--;

				//Put elements sequentially
				unsigned int k;
				for(k = j; k < bitspace->spaceship->shootsSize; k++) bitspace->spaceship->shoots[k] = bitspace->spaceship->shoots[k + 1];

				//Resize array
				bitspace->spaceship->shoots  = (Sprite**) realloc(bitspace->spaceship->shoots, sizeof(Sprite*) * bitspace->spaceship->shootsSize);
				break;
			}

		}

		if (bitspace->multiplayer) {

			for (j = 0; j < bitspace->spaceship2->shootsSize; j++) {

				if (collision(bitspace->spaceship2->shoots[j],bitspace->asteroids[i])){
					resetAsteroid(bitspace->asteroids[i],bitspace->spaceship2->image, 0);

					free(bitspace->spaceship2->shoots[j]);

					bitspace->spaceship2->shootsSize--;

					//Put elements sequentially

					unsigned int k;

					for (k = j; k < bitspace->spaceship2->shootsSize; k++)
					bitspace->spaceship2->shoots[k] = bitspace->spaceship2->shoots[k + 1];

					//Resize array

					bitspace->spaceship2->shoots = (Sprite**) realloc(bitspace->spaceship2->shoots,sizeof(Sprite*) * bitspace->spaceship2->shootsSize);
					break;

				}

			}

		}
	}

    if(bitspace->spaceship->done){

            if(lostSequence(bitspace->spaceship) == 1){

                    bitspace->spaceship->done = 0;

                    switchControlledSpaceShip(bitspace);

                    clearReceiveFifo();

                    while(!isEmptyQueue(bitspace->uart->receiveQueue))popQueue(bitspace->uart->receiveQueue);

                    return LOST_EV;

            }

            else {

                    copyBufferToDisplay();

                    clearReceiveFifo();

                    while(!isEmptyQueue(bitspace->uart->receiveQueue))popQueue(bitspace->uart->receiveQueue);

                    return NULL_EV;

            }

    }else if(bitspace->spaceship2->done){

            if(lostSequence(bitspace->spaceship2) == 1){

                    bitspace->spaceship2->done = 0;

                    switchControlledSpaceShip(bitspace);

                    clearReceiveFifo();

                    while(!isEmptyQueue(bitspace->uart->receiveQueue))popQueue(bitspace->uart->receiveQueue);

                    return LOST_EV;

            }

            else {

                    copyBufferToDisplay();

                    clearReceiveFifo();

                    while(!isEmptyQueue(bitspace->uart->receiveQueue))popQueue(bitspace->uart->receiveQueue);

                    return NULL_EV;

            }

    }
	//Calculate spaceship angle
	int dx = bitspace->mouse->x - bitspace->spaceship->image->x;
	int dy = bitspace->mouse->y - bitspace->spaceship->image->y;
	float angle = PI32 - atan2(dy,dx);
	if (angle > PI2) angle -= PI2;
	else if (angle < -PI2) angle += PI2;
	bitspace->spaceship->angleIndex = AngleToIndex(angle);

	updateSpaceShip(bitspace->spaceship);

	if(bitspace->multiplayer){
		updateSpaceShipShoots(bitspace->spaceship2);
		sendPositionEvent(bitspace->spaceship); 
	}

	//Draw
	if(bitspace->multiplayer)
	{
		clearReceiveFifo();
		receiveCommunication();
		drawShoots(bitspace->spaceship2);
		drawSpaceShip(bitspace->spaceship2);
	}

	drawShoots(bitspace->spaceship);
	drawSpaceShip(bitspace->spaceship);

	drawMouse(bitspace->state);
	draw_score();
	displayTimer();
	copyBufferToDisplay();
	resetMouseD();

	return NULL_EV;
}

void GameBrain(BitSpace *bitspace){
	ev_type_t evt = NULL_EV;

	switch (bitspace->state) {

	case MAINMENU:
		switch (MainMenu(bitspace)) {
		case PLAY_BUTTON:
			resetGame(bitspace);
			bitspace->state = GAME;
			break;

		case EX_BUTTON:
			bitspace->done = 1;
			break;

		case LEADERBOARD_BUTTON:
			bitspace->state = LEADERBOARD;
			break;

		case MULTIPLAYER_BUTTON:
			resetGame(bitspace);
			bitspace->state = MULTIPLAYER;
			break;
		}

		break;

	case LEADERBOARD:
		if (LeaderBoard(bitspace) == HOME_BUTTON)
			bitspace->state = MAINMENU;
		break;
	case MULTIPLAYER:
		if (Multiplayer(bitspace) == PLAY_BUTTON)
			bitspace->state = GAME;

		else if(Multiplayer(bitspace) == EX_BUTTON)
			bitspace->state = MAINMENU;


	case GAME:
		if (Game(bitspace) == LOST_EV)
			bitspace->state = GAMEOVER;

		else if (bitspace->keyboard->ESC_press)
			bitspace->state = SCREENBLUR;

		break;

	case GAMEOVER:{
		static int wait = 60;

		if(wait == 60) {
			blurScreen();
			draw_sprite(bitspace->gameover);
			copyBufferToDisplay();
		}

		if (wait-- == 0) {
			bitspace->state = SAVESCORE;
			wait = 60;}

	break;
	}

	case SAVESCORE:
		SaveScore(bitspace);
		bitspace->state = MAINMENU;
		break;

	case PAUSEMENU:
		bitspace->keyboard->ESC_press = false;
		switch (PauseMenu(bitspace)) {
		case RESET_BUTTON:
			resetGame(bitspace);
			bitspace->state = GAME;
			break;

		case HOME_BUTTON:
			bitspace->state = MAINMENU;
			switchControlledSpaceShip(bitspace);
			break;

		case PLAY_BUTTON:
			bitspace->state = GAME;
			clear_IN_OUT_BUFF();
			break;
		}
		break;

	case SCREENBLUR:
		//Blur actual screen and save it in bitspace->screen
		blurScreen();
		memcpy(bitspace->screen, getGraphicsBuf(), getVerRes() * getHorRes() * getBytsPerPixel());
		bitspace->state = PAUSEMENU;
		break;

	default:
		break;
	}
}

void switchControlledSpaceShip(BitSpace* bitspace)
{
	if(!bitspace->multiplayer || bitspace->host)return;

	SpaceShip * temp= bitspace->spaceship;
	bitspace->spaceship=bitspace->spaceship2;
	bitspace->spaceship2=temp;
}
