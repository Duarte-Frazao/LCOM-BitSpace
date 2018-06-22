#include "Queue.h"

#ifndef UART_H
#define UART_H

/** @defgroup
 * @{
 *  Uart
 */


/** @name Uart */
/**@{
 *
 * Uart struct
 */
typedef struct{
	int hook_id_uart;		/**< @brief Hook id */
	int IRQ_SET_UART;		/**< @brief Uart IRQ */
	int comSelected;		/**< @brief Com Port */
	unsigned long bits;		/**< @brief Word Length */
	unsigned long stop;		/**< @brief Number of Stop bits */
	unsigned long parity;		/**< @brief Parity type */
	unsigned long bitRate;		/**< @brief Bit rate (In Decimal) */
	unsigned long triggerLevel;			/**< @brief Receive FIFO trigger level */
	queue * receiveQueue;			/**< @brief Receive Queue of characters */
	queue * transmitQueue;			/**< @brief Transmit Queue of characters */
}Uart;
/** @} end of Uart struct*/




/**
 * @brief Gets receive queue
 *
 * @return Returns the receiveQueue
 */
queue* getReceivedQueue();

/**
 * @brief Gets transmit queue
 *
 * @return Returns the transmitQueue
 */
queue* getTransmittedQueue();

/**
 * @brief Prints receive queue
 *
 *	Debug purposes
 * @return
 */
void printReceiveQueue();

/**
 * @brief Prints transmit queue
 *
 *	Debug purposes
 * @return
 */
void printTransmitQueue();

/**
 * @brief Create Uart object
 *
 * Includes com port, word length, number of stop bits, parity, bit rate and fifo trigger level
 *
 * @return Returns Uart object
 */
Uart* newUart();

/**
 * @brief Gets Uart object
 *
 * @return Returns Uart object
 */
Uart* getUart();


/**
 * @brief Enables Uart device
 *
 * Initialization of the serial port through the best order
 * Set serial port configuration, setting fifo buffers, enabling interrupts,
 * subscribing uart and creating queues
 *
 * @return Returns Uart object
 */
Uart* enableUart();

/**
 * @brief Erases Uart object
 *
 * Unsubscribes Uart, deletes queues and deletes Uart object
 *
 * @return
 */
void eraseUart();

/**
 * @brief Reads desired register information
 *
 *	Takes into account the com port selected
 *	Abstraction from interaction with the uart registers
 *
 * @param info Pointer to return register information
 * @param registerSelected Register to read
 *
 * @return Returns 0 on sucess, 1 on error
 */
int getRegisterInformation(char *info, char registerSelected);


/**
 * @brief Sets new information to desired register
 *
 * @param information Information to write to the register
 * @param registerSelected Register to write to
 *
 * @return Returns 0 on sucess, 1 on error
 */
int setRegisterInformation(unsigned long information, char registerSelected);

/**
 * @brief Sets the configuration of the LCR based on the Uart object
 *
 * Configures LCR with parity, number of stop bits and word length from the Uart object
 *
 * @return Returns 0 on sucess, 1 on error
 */
int setLCRInformation();

/**
 * @brief Configures Divisor Latch with the Uart object bit rate
 *
 *	This function leaves the DLAB deactivated for reusability
 *
 * @return Returns 0 on sucess, 1 on error
 */
int setLatchInformation();

/**
 * @brief Gets the DLAB status
 *
 * @return Returns 1 if DLAB is active, 0 if not, -1 on error
 */
int getDLAB();

/**
 * @brief Activates DLAB
 *
 * @return Returns 0 on sucess, 1 on error
 */
int activateDLAB();

/**
 * @brief Deactivates DLAB
 *
 * @return Returns 0 on sucess, 1 on error
 */
int deactivateDLAB();

/**
 * @brief Gets the number of stop bits
 *
 * @return Returns number of stop bits, -1 on error
 */
int getLengthStopBit();

/**
 * @brief Gets the current bit-rate
 *
 * This function leaves the DLAB deactivated for reusability
 *
 * @return Returns bitrate, -1 on error
 */
unsigned short getBitRate();

/**
 * @brief Displays the current bit-rate
 *
 * Debug purposes
 *
 * @return
 */
void displayBitRate();

/**
 * @brief Sets UART configuration
 *
 * Sets word length, number of stop bits, parity and bit rate
 *
 * @return Returns 0 on sucess, 1 on error
 */
int setSerialPortConfiguration();

/**
 * @brief Displays information about the uart, registers and configuration
 *
 * Debug purposes
 *
 * @return
 */
void displayInformation();

/**
 * @brief Disables Uart Interrupts
 *
 * Preserves information in the IER not related to interrupts
 *
 * @return Returns 0 on sucess, 1 on error
 */
int disableUartInterrupts();

/**
 * @brief Enables Uart interrupts
 *
 * Preserves information in the IER not related to interrupts
 *
 * @return Returns 0 on sucess, 1 on error
 */
int enableUartInterrupts();

/**
 * @brief Configures UART to support FIFO buffers with the specified trigger level
 *
 * @return Returns 0 on sucess, 1 on error
 */
int setFifoBuffers();

/**
 * @brief Initialization of the UART
 *
 * Generic function to initialize the Uart device
 * Takes into account the best order to initialize the UART
 * Starts disabling uart interrupts to prevent uart interrupts while initializing
 * Sets fifo buffers, queues and subscribes uart
 *
 * @return Returns 0 on sucess, 1 on error
 */
int programUartInitialization();

/**
 * @brief Subscribes uart interrupts to Uart object's hook id
 *
 * @return Returns bit of hook id, -1 on error
 */
int uart_subscribe();

/**
 * @brief Unsubscribes uart interrupts from Uart object's hook id
 *
 * @return Returns 0 on sucess, 1 on error
 */
int uart_unsubscribe();

/**
 * @brief Sends a character to the FIFO transmiter buffer
 *
 * @param c Character to transmit
 *
 * @return Returns 0 on sucess, 1 on error
 */
int charTx(unsigned char c);

/**
 * @brief Sends string to the transmitQueue
 *
 *
 * @param s String to push to the transmitQueue
 *
 * @return Returns 0 on sucess, 1 on error
 */
int stringTx(unsigned char *s);

/**
 * @brief Prints a string
 *
 * Debug purposes
 *
 * @param word String to print
 *
 * @return
 */
void printWord(char* word);

/**
 * @brief Prints a string with decimal values
 *
 * Debug purposes
 *
 * @param message String to print
 *
 * @return
 */
void printDecimalWord(char* message);



/**
 * @brief Checks if there is data to be read in the receive FIFO buffer
 *
 * @return Returns 1 if there is data in Receive Fifo buffer, 0 if not, -1 on error
 */
int dataInFifo();

/**
 * @brief Clears the receive FIFO buffer to the receive queue
 *
 * @return Returns 0 on sucess, 1 on error
 */
int clearReceiveFifo();


/**
 * @brief Checks if it's possible to process another character
 *
 * There are two ways the uart signals the possibility to transfer another character (THR empty)
 * 1- Throught an interrupt (Read in IIR)
 * 2- Through a flag in the LSR (bit 5)
 * This function tests the second way, the other is in the handler
 *
 * @return Returns 1 if possible to transmit, 0 if not, -1 on error
 */
int transmissionPossible();


/**
 * @brief Sends characters in the transmit queue to the transmit FIFO buffer

 *
 * @return Returns 0 on sucess, 1 on error
 */
int processTransmitQueue();

/**
 * @brief Extracts string from the receive queue, untill it reaches a message trail
 *
 * @return Returns string, Null on error
 */
char* extractString();


/**
 * @brief Handles LSR related interrupts
 *
 * @return
 */
void lsrInterruptHandler();

/**
 * @brief Handles Uart interrupts
 *
 * @return
 */
void uart_handler();


///////////////////////////////////
////////Protocol functions/////////
///////////////////////////////////


///////////Protocol////////////////
//Size-9, except hoes and ack messages, which have 2
////Header
////// 'message type symbol'

////Event (if it's a message)
////// 'event type symbol'
////// info
////// info
////// info
////// info

////Trailer
////// '.'

///////////////////////////////////

/**
 * @brief Sends event to the transmit queue with game protocol
 *
 * @param event String containing event informaton
 *
 * @return Returns 0 on sucess, 1 on error
 */
int sendEvent(char* event);

/**
 * @brief Sends ACK to transmit queue with game protocol
 *
 * @return Returns 0 on sucess, 1 on error
 */
int sendAck();

/**
 * @brief Waits for an ack
 *
 * Leaves the receive queue empty(begin of program)
 *
 * @return Returns 0 on sucess, 1 on error
 */
int waitForAck();

/**
 * @brief Sends Host message, signaling other machine is host
 *
 *	Used in the synchronization function
 *
 * @return Returns 0 on sucess, 1 on error
 */
int sendHostMessage();

/**
 * @brief Receives and identifies communication
 *
 * @return Returns 0 on sucess, 1 on error
 */
int receiveCommunication();

/**
 * @brief Receives and identifies event message
 *
 * @return Returns 0 on sucess, 1 on error
 */
int receiveMessage();

/**
 * @brief Handles received events related to position
 *
 * @return Returns 0 if message has been read, 1 if not
 */
int uart_position_handler();



/**
 * @brief Executes communication protocol on Host receiving
 *
 * @return Returns 0 on sucess, 1 on error
 */
int protocolHostReceive();

/** @} end of Uart*/

#endif
