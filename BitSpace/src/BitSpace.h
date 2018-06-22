#ifndef BITSPACE_H
#define BITSPACE_H

#include "Bitmap.h"
#include "Sprite.h"
#include "SpaceShip.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Timer.h"
#include "Score.h"
#include "RTC.h"
#include "Uart.h"
#include "Queue.h"

#define NUM_SPACESHIPS 3
#define NUM_ALERTS	   3
#define NUM_ASTEROIRDS 6

#define PI 3.14159265	 /* PI */
#define PI2 6.2831853	 /* 2*PI */
#define PI32 4.712388975 /* PI/2*/


/** @defgroup BitSpace BitSpace
 * @{
 *  BitSpace
 */


/** @name Game States */
/**@{state_t*/
typedef enum {INIT, MAINMENU, LEADERBOARD, GAME, SAVESCORE, PAUSEMENU,SCREENBLUR, GAMEOVER, MULTIPLAYER} state_t;
/** @} end of state_t*/


/** @name Game Events */
/**@{ev_type_t*/
typedef enum {PLAY_BUTTON, EX_BUTTON, HOME_BUTTON, LEADERBOARD_BUTTON, RESET_BUTTON, LOST_EV, MULTIPLAYER_BUTTON,NULL_EV} ev_type_t;
/** @} end of ev_type_t*/



/** @name BitSpace */
/**@{
 *
 * BitSpace struct
 */
typedef struct {
	char * date;
	int score;
} Rank;

typedef struct {
	int done; 		/**< @brief Set to exit the game */
	state_t state;  /**< @brief Game state */

	/* Devices */
	Keyboard * keyboard; /**< @brief Keyboard struct */
	Mouse * mouse;		 /**< @brief Mouse struct */
	Timer * timer;		 /**< @brief Timer struct */
	Rtc * rtc;			 /**< @brief RTC struct */
	Uart * uart;			 /**< @brief UART struct */



	/* Gameplay */
	SpaceShip* spaceship; 	/**< @brief Spaceship chosen */
	SpaceShip** spaceships; /**< @brief Spaceships available in the game */
	Sprite ** asteroids;	/**< @brief Array of asteroids present on screen*/
	Bitmap *bg;				/**< @brief Space background image*/
	Score * score;			/**< @brief Score struct */


	/*Multiplayer*/
	int multiplayer;		/**< @brief Set when playing in multiplayer mode*/
	SpaceShip* spaceship2; 	/**< @brief Second player spaceship*/
	int host;		/**< @brief Set when it's the host, client if not set*/

	/* Main Menu */
	Bitmap *mainmenu;		/**< @brief Main Menu image*/
	Sprite ** alerts;		/**< @brief Array of alerts new achievements*/


	/* Leaderboard */
	Bitmap *leaderboard;	/**< @brief Leaderboard image*/
	Rank ranks[5];			/**< @brief Array top 5 best scores with respective date*/

	/* Pause Menu */
	char * screen;			/**< @brief Blurred screen image*/
	Sprite * pause;			/**< @brief Pause menu image*/

	Sprite * gameover;		/**< @brief Game Over image*/

} BitSpace;
/** @} end of BitSpace struct*/

/**
 * @brief Switches controlled spaceship when it's the client
 *
 * Useful to keep the singleplayer code in the multiplayer mode without many changes
 * Changes when it's in multiplayer and it's the client
 *
 * @param bitspace Bitspace object
 * @return
 */
void switchControlledSpaceShip(BitSpace *bitspace);

/**
 * @brief Initiate Game
 *
 * Subscribe Timer, Keyboard, Mouse and RTC interrupts and create respective objects
 * Loads main menu, leaderboard, pause menu and background images;
 * Create spaceship and asteroids
 *
 * @param bitspace Bitspace object where the game will be initialized
 * @return
 */
void initGame(BitSpace *bitspace);


/**
 * @brief Reset Game
 *
 * Reset Timer, Keyboard, Mouse and RTC objects
 * Reset Spaceship and Asteroids position
 * Reset Score
 * @param bitspace Bitspace object to be reset
 * @return
 */
void resetGame(BitSpace *bitspace);

/**
 * @brief End Game
 *
 * Unsubscribe Timer, Keyboard, Mouse and RTC interrupts and free respective objects
 * Free spaceship and asteroids
 * Free score object
 * @param bitspace Bitspace object to be erased
 * @return
 */
void endGame(BitSpace *bitspace);

/**
 * @brief Game Interrupts
 *
 * Detect and identify interrupts
 * Call respective handlers
 * @param bitspace Bitspace object that provides information of each device IRQ_SET
 * @return
 */
void intGame(BitSpace *bitspace);

/**
 * @brief Main Menu
 *
 * Display Main Menu information
 * @param bitspace Bitspace object that provides the images needed
 * @return ev_type_t Event occurred
 */
ev_type_t MainMenu(BitSpace *bitspace);

/**
 * @brief Leaderboard
 *
 * Display Leaderboard information
 * @param bitspace Bitspace object that provides the images and information about score
 * @return ev_type_t Event occurred
 */
ev_type_t LeaderBoard(BitSpace *bitspace);

/**
 * @brief Pause Menu
 *
 * Blur current game image and display pause menu
 * @param bitspace Bitspace object that provides the necessary images
 * @return ev_type_t Event occurred
 */
ev_type_t PauseMenu(BitSpace *bitspace);

/**
 * @brief Save score after lost
 *
 * Determine if the play score is in the top 5
 * If so, the new score is added to the rank array
 * @param bitspace Bitspace object that provides information about score
 * @return ev_type_t Event occurred
 */
ev_type_t SaveScore(BitSpace *bitspace);

/**
 * @brief Game
 *
 * Process information received by the devices' interrupts
 * Update game mechanics
 * @param bitspace Bitspace object that provides information about necessary objects
 * @return
 */
ev_type_t Game(BitSpace *bitspace);



/**
 * @brief Game State Machine
 *
 * Call functions related to the current game state
 * @param bitspace Bitspace object that provides information about the current state of game
 * @return
 */
void GameBrain(BitSpace *bitspace);

void addDelay(unsigned long time);

BitSpace* getBitSpace();

/** @} end of BitSpace*/

#endif
