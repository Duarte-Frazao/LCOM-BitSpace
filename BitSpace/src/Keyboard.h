#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <stdbool.h>

/** @defgroup Keyboard Keyboard
 * @{
 *  Keyboard
 */


/** @name Keyboard States */
/**@{key_state_t*/
typedef enum {MOVE, DECIDE, STOP} key_state_t;
/** @} end of key_state_t*/


/** @name Keyboard */
/**@{
 *
 * Keyboard struct
 */
typedef struct {

	key_state_t state_A; 		/**< @brief Keeps state of key A */
	key_state_t state_S; 		/**< @brief Keeps state of key S */
	key_state_t state_D; 		/**< @brief Keeps state of key D */
	key_state_t state_W; 		/**< @brief Keeps state of key W */

	bool ESC_press; 		/**< @brief True when esc is pressed */

	int IRQ_SET_KBD; 		/**< @brief Keyboard IRQ */
	int hook_id_kbd; 		/**< @brief Keyboard hook id */
	unsigned char scancode; 		/**< @brief Last read scancode (0xFF on reset) */
}Keyboard;
/** @} end of Keyboard struct*/


/**
 * @brief Creates a new keyboard object
 *
 * Initializes keyboard states in the STOP state
 *
 * @return Returns keyboard object
 */
Keyboard* newKeyboard();

/**
 * @brief Enables Keyboard
 *
 * Subscribes keyboard an assigns the correct IRQ to the keyboard struct
 *
 * @return Returns the keyboard object
 */
Keyboard* enableKeyboard();

/**
 * @brief Gets the keyboard object
 *
 * @return Returns the keyboard object
 */
Keyboard* getKeyboard();

/**
 * @brief Resets keyboard
 *
 * Resets ESC_press flag, states and scancode
 *
 * @return
 */
void resetKeyboard();

/**
 * @brief Erases keyboad object
 *
 * Unsubscribes keyboard and free keyboard object
 *
 * @return
 */
void eraseKeyboard();

/**
 * @brief Handles keyboard related interrupts
 *
 * Responsible for movement of spaceship and pause state
 *
 * @return
 */
void keyboard_handler();

/**
 * @brief Updates keyboard states
 *
 * @return
 */
void update_keyboard();


/**
 * @brief Reads Status register information
 *
 * @param status Pointer to return informationo of the status register
 * @return Returns 0 on sucess, 1 on error
 */
int readStatus(unsigned long *status);

/**
 * @brief Checks if output buffer is full
 *
 * @return Returns 0 if is full, 1 if not, -1 if an error occured
 */
int outBufferFull();

/**
 * @brief Reads scancode from the output buffer
 *
 * @param scancode Pointer to return scancode
 *
 * @return Returns 0 on sucess, 1 on error
 */
int readScancode(unsigned char *scancode);

/**
 * @brief Subscribes keyboard to keyboard's object hook id
 *
 * @return Returns bit of hook id
 */
int kbd_subscribe();

/**
 * @brief Unsubscribes keyboard on keyboard object's hook id
 *
 * @return Returns 0 on sucess, 1 on error
 */
int kbd_unsubscribe();

/**
 * @brief Stays idle for specified time (in seconds)
 *
 * @return
 */
void addDelay(unsigned long time);

/**
 * @brief Clears the output buffer
 *
 * @return Returns 0 on sucess, 1 on error
 */
int clear_IN_OUT_BUFF();

/** @} end of Keyboard*/

#endif
