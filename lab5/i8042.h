#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_


#define BIT(n) (0x01<<(n))

#define KB_IRQ	        1    /**< @brief Keyboard IRQ line */

/* Input/Output buffer addresses */

#define STATUS_REGISTER		0x64			/**< @brief status register address to read the KBC state */

#define IN_BUFFER_COMMANDS	0x64 			/**< @brief input buffer address for commands for the KBC */
#define IN_BUFFER_ARGS		0x60 			/**< @brief input buffer address for arguments of KBC commands */
#define OUT_BUFF			0x60 			/**< @brief output buffer address to read scancodes and return values from KBC commands */

#define OUT_BUFFER_MASK		BIT(5) | BIT(0) /**< @brief mask used to check the OBF and AUX from the command byte */
#define OUT_BUFFER_READY 	0x01 			/**< @brief Result when comparing information read from the status register and necessary bits to check if input buffer is ready to be read */

#define KBC_MESSAGE_READ	0x20			/**< @brief Message to the kbc, for a read command byte operation */
#define KBC_MESSAGE_WRITE	0x60			/**< @brief Message to the kbc, for a write command byte operation */

#define KBC_DISABLE_CODE	0xFE			/**< @brief Number used to set bit(0) from the command byte to 0  */
#define KBC_ENABLE_CODE		0x01			/**< @brief Number used to set bit(0) from the command byte to 0  */

#define DELAY_US    		20000			/**< @brief  Constant for delay*/

#define ESC_BREAK  			0x81			/**< @brief Breakcode from the esc key */

#define ERROR_READ			0xC0			/**< @brief  Constant use to check if there was an error in the status register*/

#define TWO_CODES 			0xe0 			/**< @brief  Standard code for special keycodes*/

#define OBF					BIT(0)
#define IBF					BIT(1)
#define TYPE_MASK			BIT(7)

//#define PIC1_CMD			0x01

#endif /* _LCOM_I8042_H */
