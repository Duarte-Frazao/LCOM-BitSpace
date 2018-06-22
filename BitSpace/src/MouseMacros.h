/** @defgroup
 * @{
 *  MouseMacros
 */

#define MOUSE_IRQ	    12   /**< @brief Mouse IRQ line */

/* Input/Output buffer addresses */

#define STATUS_REGISTER		0x64			/**< @brief status register address to read the KBC state */

#define IN_BUFFER_COMMANDS	0x64 			/**< @brief input buffer address for commands for the KBC */
#define IN_BUFFER_ARGS		0x60 			/**< @brief input buffer address for arguments of KBC commands */
#define OUT_BUFF			0x60 			/**< @brief output buffer address to read scancodes and return values from KBC commands */

#define WRITE_TO_MOUSE		0xD4 			/**< @brief Command to write Byte to Mouse  */

#define ENABLE_STREAM_MODE	0xEA 			/**< @brief Command used to set stream mode on  */
#define ENABLE_DATA_REPORT 	0xF4 			/**< @brief Command used to set data reporting on  */
#define DISABLE_DATA_REPORT 0xF5 			/**< @brief Command used to set data reporting off  */
#define ENABLE_REMOTE_MODE	0xF0 			/**< @brief Command used to set remote mode on  */


#define OUT_BUFFER_MASK		BIT(5) | BIT(0) /**< @brief mask used to check the OBF and AUX from the command byte */
#define OUT_BUFFER_READY 	0x01 			/**< @brief Result when comparing information read from the status register and necessary bits to check if input buffer is ready to be read */

#define KBC_MESSAGE_READ	0x20			/**< @brief Message to the kbc, for a read command byte operation */
#define KBC_MESSAGE_WRITE	0x60			/**< @brief Message to the kbc, for a write command byte operation */

#define DISABLE_MOUSE_INTER	0xFE			/**< @brief Number used to set bit(0) from the command byte to 0  */
#define ENABLE_MOUSE_INTER	0x01			/**< @brief Number used to set bit(0) from the command byte to 1  */

#define KBC_DISABLE_INTERRUPTS	0xFC		/**< @brief Number used to set bit(1) and bit(0) from the command byte to 0, disabling interrupts  */
#define KBC_ENABLE_INTERRUPTS	0x03		/**< @brief Number used to set bit(1) and bit(0) from the command byte to 1 , enable interrupts */

#define KBC_DISABLE_KEYBOARD 0x10			/**< @brief Number used to set bit(4) from the command byte to 1, disabling keyboard  */
#define KBC_ENABLE_KEYBOARD 0xEF			/**< @brief Number used to set bit(4) from the command byte to 0, enabling keyboard  */


#define DELAY_US     		85000			/**< @brief  Constant for delay*/

#define READ_DATA_REQUEST   0xEB			/**< @brief  Command to request packet in remote mode*/

#define ESC_BREAK  			0x81			/**< @brief Breakcode from the esc key */

#define ERROR_READ			0xC0			/**< @brief  Constant use to check if there was an error in the status register*/

#define TWO_CODES 			0xe0 			/**< @brief  Standard code for special keycodes*/

#define OBF					BIT(0)			/**< @brief  Output buffer bit in the byte from the status register*/
#define IBF					BIT(1)			/**< @brief  Input buffer bit in the byte from the status register*/
#define TYPE_MASK			BIT(7)			/**< @brief  Parity bit in the byte from the status register*/

#define ACK 				0xFA 			/**< @brief  ACK byte*/
#define NACK				0xFE 			/**< @brief  NACK byte*/
#define ERROR				0xFC 			/**< @brief  NACK byte*/


/** @} end of MouseMacros*/
