/** @defgroup
 * @{
 *  UartMacros
 */

#define COM1 0x3F8 //Port COM1 address
#define COM2 0x2F8 //Port COM2 address

//////////UART Acessible (8-bit) registers///////////////
#define RBR 0 //Receiver Buffer Register R
#define THR 0 //Transmitter Holding Register W
#define IER 1 //Interrupt Enable Register R/W
#define IIR 2 //Interrupt Identification Reg. R
#define FCR 2 //FIFO Control Register W
#define LCR 3 //Line Control Register R/W
#define MCR 4 //MCR Modem Control Register R/W
#define LSR 5 //Line Status Register R
#define MSR 6 //Modem Status Register R
#define SR 7  //Scratchpad Register R/W
#define DLL 0 //Divisor Latch LSB R/W
#define DLM 1 //Divisor Latch MSB R/W

//////////UART Parity options///////////
#define NOPARITY 0x0
#define ODDPARITY 0x1
#define EVENPARITY 0x3
#define HIGHPARITY 0x5
#define LOWPARITY 0x7


///////////REGISTER RELATED MACROS//////////////

//////LCR
#define LCR_PARITY_BITS 0x7
#define LCR_WORD_LENGTH_VALID_BITS 0x3
#define LCR_DLAB_BIT BIT(7)
#define DEACTIVATE_DLAB_CODE 0x7F
#define LCR_STOP_BIT BIT(2)

//////BIT-RATE RELATED
#define DLL_BITS_VALID 0x00FF
#define DLM_BITS_VALID 0xFF00
#define BIT_RATE_VALID_BITS 0xFF

//////FCR
#define FCR_ENABLE_FIFO_BIT BIT(0)
#define FCR_CLEAR_RECEIVE_FIFO_BIT BIT(1)
#define FCR_CLEAR_TRANSMIT_FIFO_BIT BIT(2)

//////LSR
#define LSR_DATA_READY_BIT BIT(0)
#define LSR_OVERRUN_ERROR_BIT BIT(1)
#define LSR_PARITY_ERROR_BIT BIT(2)
#define LSR_FRAMING_ERROR_BIT BIT(3)
#define LSR_BREAK_INTERRUPT_BIT BIT(4)
#define LSR_EMPTY_TRANSMITTER_HOLDING_REGISTER_BIT BIT(5)
#define LSR_EMPTY_DATA_HOLDING_REGISTER BIT(6)
#define LSR_ERROR_FIFO_BIT BIT(7)

///////IIR
#define IIR_INTERRUPT_BITS 0x0E
#define IIR_NO_INTERRUPT_PENDING_BIT BIT(0)

///////IER
#define DISABLE_INTERRUPTS_CODE 0xFFF8
#define ENABLE_INTERRUPTS_CODE 0x7


/////////////////////PROTOCOL/////////////////////

#define HEADER_ACK_SYMBOL '+'
#define HEADER_MESSAGE_SYMBOL '*'
#define HEADER_SPACESHIP_POSITION_SYMBOL '|'
#define HEADER_HOST_SYMBOL 'H'
#define HEADER_DONE_SYMBOL '{'
#define KEYBOARD_SYMBOL 'K'
#define POSITION_SYMBOL '|'
#define TRAIL_UART_PROTOCOL '}'
#define PROTOCOL_MESSAGE_SIZE 9
#define PROTOCOL_HOST_SIZE 2
#define PROTOCOL_ACK_SIZE 2
#define PREVENT_NEGATIVE_CONVERSION 0xFF

/** @} end of UartMacros*/
