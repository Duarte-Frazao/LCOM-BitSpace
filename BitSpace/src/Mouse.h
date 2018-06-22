#ifndef __MOUSE_H
#define __MOUSE_H

/** @defgroup Mouse Mouse
 * @{
 *  Mouse
 */

 /** @name Mouse */
 /**@{
  *
  * Mouse struct
  */
typedef struct {
	int x, y, dx,dy; 		/**< @brief  Mouse position and variation  */

	unsigned char packet[3]; 		/**< @brief  Array to hold packets */
	unsigned char bytePacketNum; 		/**< @brief Current pack number */

	int leftButtonPressed; 		/**< @brief Set if left mouse button is pressed  */
	int rightButtonPressed; 		/**< @brief  Set if left mouse button is pressed */

	int draw;						/**< @brief  Set if mouse needs to be draw */

	int IRQ_SET_MOUSE;	/**< @brief  Mouse IRQ line */
	int hook_id_mouse;	/**< @brief  Mouse hook id*/
}Mouse;

/**
 * @brief Enables mouse
 *
 * Enables stream mode and data report
 * Subscribes mouse interrupts
 *
 * @return
 */
void enableMouse();

/**
 * @brief Creates a new mouse object
 *
 * Initalizes member variables and states
 * Loads mouse sprites
 * Creates packet queue
 *
 * @return Returns mouse object
 */
Mouse* newMouse();

/**
 * @brief Gets mouse object
 *
 * @return Returns mouse object
 */
Mouse* getMouse();

/**
 * @brief Erases mouse object
 *
 * Disables data report
 * Unsubscribes mouse from mouse object's hook id
 * free mouse
 *
 * @return
 */
void eraseMouse();

/**
 * @brief Resets Mouse
 *
 * Resets position, variation, current packet number, state of buttons and draw flag
 *
 * @return
 */
void resetMouse();

/**
 * @brief Resets Mouse variation
 *
 * Resets mouse variation
 *
 * @return
 */
 void resetMouseD();

 /**
  * @brief Processes packets in the packets queue, updating mouse
  *
  * @return
  */
void updateMouse();

/**
 * @brief Reads and sends packet to packets queue
 *
 * @return
 */
void packet_handler();

/**
 * @brief Subscribes mouse with mouse object's hook id
 *
 * @return Returns hook id bit, -1 on error
 */
int mouse_subscribe();

/**
 * @brief Unsubscribes mouse with mouse object's hook id
 *
 * @return Returns 0 on sucess, -1 on error
 */
int mouse_unsubscribe();

/**
 * @brief Configures mouse with specified command
 *
 * @param cmd Command to send to the kbc
 *
 * @return Returns 0 on sucess, 1 on error reading status register, -1 on error communicating with kbc
 */
int mouseConfiguration(short cmd);

/**
 * @brief Clears the output buffer
 *
 * @return Returns 0 on sucess, 1 on error
 */
int clear_IN_OUT_BUFF();

/**
 * @brief Prints packet
 *
 *	Debug purposes
 *
 * @param packet Packet to read
 *
 * @return
 */
void printPacket(unsigned char *packet);

/**
 * @brief Prints packet
 *
 *	Debug purposes
 *
 * @param packet Packet to read
 *
 * @return
 */

 /**
  * @brief Checks if output buffer is full
  *
  * @return Returns 0 if is full, 1 if not, -1 if an error occured
  */
int outBufferFull();

/** @} end of Mouse*/

#endif
