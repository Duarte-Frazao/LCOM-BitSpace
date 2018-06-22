#include "Uart.h"
#include "UartMacros.h"
#include "Keyboard.h"
#include "BitSpace.h"
#include "Utilities.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <stdbool.h>
#include "Sprite.h"


static Uart* uart;

static int UART_IRQ_COM1=4;
static int UART_IRQ_COM2=3;

//Debug
void printReceiveQueue(){printQueue(uart->receiveQueue);}
void printTransmitQueue(){printQueue(uart->transmitQueue);}

static bool discardString = false;

Uart* newUart()
{
	uart= (Uart*) malloc(sizeof(Uart));
	uart->comSelected=1;
	uart->bits=8;
	uart->stop=1;
	uart->parity= EVENPARITY;
	uart->bitRate= 1;
	uart->triggerLevel= 8;
	return uart;
}

Uart* getUart()
{
	return uart;
}

Uart* enableUart()
{
	programUartInitialization();
	return uart;
}

void eraseUart()
{
	uart_unsubscribe();
	deleteQueue(uart->transmitQueue);deleteQueue(uart->receiveQueue);
	if(uart!= NULL) free(uart);
}



//This function gets the configuration from the desired register and port
int getRegisterInformation(char *info, char registerSelected)
{
	unsigned long information;
	if(uart->comSelected==1)
	{
		if(sys_inb(COM1 + registerSelected, &information) != OK) return 1;
	}else if(uart->comSelected==2)
	{
		if(sys_inb(COM2 + registerSelected, &information) != OK) return 1;
	}else return 1;
	* info =(char) information;

	return 0;
}

//This function sets a new configuration to the desired port and register
int setRegisterInformation(unsigned long information, char registerSelected)
{

	if(uart->comSelected==1)
	{
		if(sys_outb(COM1 + registerSelected, information) != OK) return 1;
	}else if(uart->comSelected==2)
	{
		if(sys_outb(COM2 + registerSelected, information) != OK) return 1;
	}else return 1;


	return 0;
}

//This function sets the configuration of the Line Control Register
int setLCRInformation()
{
	char lcrInfo;

	//To preserve the non affected bits
	if(getRegisterInformation(&lcrInfo,LCR)!= OK)
	{
		printf("In setLCRInformation():: getting LCR configuration\n");
		return 1;
	}

	//New configuration
	unsigned long information = (unsigned long)lcrInfo | ((uart->parity & LCR_PARITY_BITS) <<3) | (( (uart->stop-1) & BIT(0)) <<2) | ((uart->bits -5) & LCR_WORD_LENGTH_VALID_BITS);

	//Writing to LCR
	if(setRegisterInformation(information,LCR) != OK)
	{
		printf("In setLCRInformation():: setting LCR configuration\n");
		return 1;
	}

	return 0;
}

//Sets the selected bit-rate
//This function leaves the DLAB deactivated for reusability
int setLatchInformation()
{
	//If DLAB not activated, activate to acess Divisor Latch Bytes
	if(!getDLAB()) activateDLAB();
	unsigned long dll,dlm;

	dll = ((unsigned long)uart->bitRate & DLL_BITS_VALID);
	dlm = ((unsigned long)uart->bitRate & DLM_BITS_VALID);

	if(uart->comSelected==1)
	{
		if(sys_outb(COM1 + DLL, dll) != OK) return 1;
		if(sys_outb(COM1 + DLM, dlm) != OK) return 1;
	}else if(uart->comSelected==2)
	{
		if(sys_outb(COM2 + DLL, dll) != OK) return 1;
		if(sys_outb(COM2 + DLM, dlm) != OK) return 1;
	}else return 1;

	//If DLAB not activated, activate to acess Divisor Latch Bytes
	deactivateDLAB();

	return 0;
}

//Gets the DLAB status
//1 if activated
int getDLAB()
{
	char information;

	if(getRegisterInformation(&information,LCR)!=OK) return 1;

	return ((information & LCR_DLAB_BIT) >>7);
}

int activateDLAB()
{
	char information;
	unsigned long nextInfo;

	if(getRegisterInformation(&information,LCR)!=OK) return 1;

	nextInfo = (information | LCR_DLAB_BIT);


	if(setRegisterInformation(nextInfo,LCR)!=OK) return 1;

	return 0;

}

int deactivateDLAB()
{
	char information;
	unsigned long nextInfo;

	if(getRegisterInformation(&information,LCR)!=OK) return 1;

	nextInfo= (information & DEACTIVATE_DLAB_CODE);

	if(setRegisterInformation(nextInfo,LCR)!=OK) return 1;

	return 0;
}

//Gets the number of stop bits
int getLengthStopBit()
{
	char information;

	if(getRegisterInformation(&information,LCR)!=OK) return -1;

	//Missing 1.5 length but we're not gonna need it
	return (((information & LCR_STOP_BIT) >>2)+1);
}

//Gets the current bit-rate
//This function leaves the DLAB deactivated for reusability
unsigned short getBitRate()
{

	unsigned short bitrate;
	char information;

	//If DLAB not activated, activate to acess Divisor Latch Bytes
	if(!getDLAB())
	{
		if(activateDLAB()!=OK) return -1;
	}

	if(getRegisterInformation(&information,DLM)!=OK) return -1;

	bitrate= ((unsigned short)information<<8);

	if(getRegisterInformation(&information,DLL)!=OK) return -1;

	bitrate=bitrate | ((unsigned short)information & BIT_RATE_VALID_BITS);

	if(deactivateDLAB()!=OK) return -1;

	return bitrate;
}

//Displays the current bit-rate for debug purposes
void displayBitRate()
{
	unsigned short bitRate = getBitRate();

	printf("Bit Rate Code: 0x%x\n", bitRate);
	printf("Bit Rate Value: %d\n",  115200/bitRate);

}

//Configures the UART
int setSerialPortConfiguration()
{
	//Configure LCR
	if(setLCRInformation(uart->bits,uart->stop,uart->parity) != OK)
	{
		printf("Error in setSerialPortConfiguration()\n");
		return 1;
	}


	//Configure bit rate
	if(setLatchInformation(uart->bitRate) != OK)
	{
		printf("Error in setSerialPortConfiguration()\n");
		return 1;
	}
	return 0;
}

//Displays information about the uart, registers and configuration
//for debug purposes
void displayInformation()
{
	char info;
	if(getRegisterInformation(&info,LCR) != OK) return;

	printf("LCR conf: Latch: %d  \nBreak Enable: %d \nParity: %d %d %d \nLength Stop Bit :%d \nWord Length(5-8): %d\n\n\n"
		,(info & BIT(7))>> 7,(info & BIT(6))>> 6
		,(info & BIT(5))>> 5,(info & BIT(4))>> 4
		,(info & BIT(3))>> 3
		,((info & BIT(2))>> 2)+ 1
		,5+ ((info & BIT(1))>> 1) +2*((info & BIT(0))>> 0)
	);

	char parity =((info & BIT(5))>> 3 |(info & BIT(4))>> 3|(info & BIT(3)) >>3);


	switch(parity)
	{
	case(1):
			printf("Odd Parity\n");
			break;
	case(3):
			printf("Even Parity\n");
			break;
	case(4):
			printf("High Parity\n");
			break;
	case(7):
			printf("Low Parity\n");
			break;
	default:
			printf("No parity\n");
			break;

	}
	if(getRegisterInformation(&info,IER) != OK)
	{
		printf("Error in displayInformation()\n");
		return;
	}
	printf("\n\n\tIER :%d %d %d %d %d %d %d %d\n",(info & BIT(7)) >> 7,(info & BIT(6)) >> 6,(info & BIT(5)) >> 5,(info & BIT(4)) >> 4
				,(info & BIT(3)) >> 3,(info & BIT(2)) >> 2,(info & BIT(1)) >> 1,
				(info & BIT(0)));


	if(getRegisterInformation(&info,LCR) != OK)
	{
		printf("Error in displayInformation()\n");
		return;
	}
	printf("\n\n\tLCR :%d %d %d %d %d %d %d %d\n",(info & BIT(7)) >> 7,(info & BIT(6)) >> 6,(info & BIT(5)) >> 5,(info & BIT(4)) >> 4
				,(info & BIT(3)) >> 3,(info & BIT(2)) >> 2,(info & BIT(1)) >> 1,
				(info & BIT(0)));


	if(getRegisterInformation(&info,FCR) != OK)
	{
		printf("Error in displayInformation()\n");
		return;
	}
	printf("\n\n\tFCR :%d %d %d %d %d %d %d %d\n",(info & BIT(7)) >> 7,(info & BIT(6)) >> 6,(info & BIT(5)) >> 5,(info & BIT(4)) >> 4
				,(info & BIT(3)) >> 3,(info & BIT(2)) >> 2,(info & BIT(1)) >> 1,
				(info & BIT(0)));

	if(getRegisterInformation(&info,LSR) != OK)
	{
		printf("Error in displayInformation()\n");
		return;
	}
	printf("\n\n\tLSR :%d %d %d %d %d %d %d %d\n",(info & BIT(7)) >> 7,(info & BIT(6)) >> 6,(info & BIT(5)) >> 5,(info & BIT(4)) >> 4
				,(info & BIT(3)) >> 3,(info & BIT(2)) >> 2,(info & BIT(1)) >> 1,
				(info & BIT(0)));

	if(getRegisterInformation(&info,IIR) != OK)
	{
		printf("Error in displayInformation()\n");
		return;
	}
	printf("\n\n\tIIR :%d %d %d %d %d %d %d %d\n",(info & BIT(7)) >> 7,(info & BIT(6)) >> 6,(info & BIT(5)) >> 5,(info & BIT(4)) >> 4
				,(info & BIT(3)) >> 3,(info & BIT(2)) >> 2,(info & BIT(1)) >> 1,
				(info & BIT(0)));
}

//Disables Uart Interrupts
int disableUartInterrupts()
{
	//Get current IER configuration
	char information;
	if(getRegisterInformation(&information,IER) != OK)
	{
		printf("Error in disableUartInterrupts()\n");
		return 1;
	}

	//Disables interrupt related bits (except modem that we don't use)
	unsigned long command = ((unsigned long)(information & DISABLE_INTERRUPTS_CODE));
	if(setRegisterInformation(command,IER) != OK) return 1;

	return 0;
}

//Enables Uart interrupts
int enableUartInterrupts()
{
	//Get current IER configuration
	char information;
	if(getRegisterInformation(&information,IER) != OK)
	{
		printf("Error in enableUartInterrupts()\n");
		return 1;
	}

	//Enables interrupt related bits (except modem that we don't use)
	unsigned long command = ((unsigned long)(information | ENABLE_INTERRUPTS_CODE));

	if(setRegisterInformation(command,IER) != OK)
	{
		printf("Error in enableUartInterrupts()\n");
		return 1;
	}

	return 0;
}

//Configures UART to support FIFO buffers with the specified triger level
int setFifoBuffers()
{
	//We set the clear flags to remove leftover information from last programs
	unsigned long command = ((uart->triggerLevel <<6) | FCR_CLEAR_TRANSMIT_FIFO_BIT | FCR_CLEAR_RECEIVE_FIFO_BIT | FCR_ENABLE_FIFO_BIT);

	if(setRegisterInformation(command,FCR) != OK)return 1;

	return 0;
}

//Initialization of the UART, for the beggining of a program
//Takes into account the best order to configure the UART
int programUartInitialization()
{
	//Turn off the interrupt generation on the uart, to prevent initialization
	//from being interrupted by the uart
	if(disableUartInterrupts() != OK)
	{
		printf("Error in programUartInitialization(), disableUartInterrupts()\n");
		return 1;
	}


	//Configure line control register (including bit rate)
	if(setSerialPortConfiguration() != OK)
	{
		printf("Error in programUartInitialization(), setSerialPortConfiguration()\n");
		return 1;
	}

	//Enable FIFO buffers
	//Clear FIFO buffers (good practise on initialization, to remove leftover information from a last program)
	//FCR bit 1 e 2 and select DMA mode bit 3
	if(setFifoBuffers() != OK)
	{
		printf("Error in programUartInitialization(), setFifoBuffers()\n");
		return 1;
	}

	//Create queues
	uart->transmitQueue= createQueue();
	uart->receiveQueue= createQueue();
	cleanQueue(uart->receiveQueue);

	//Enable interrupts (Doing last to prevent interruptions on the initialization)
	if(enableUartInterrupts() != OK)
	{
		printf("Error in programUartInitialization(), enableUartInterrupts()\n");
		return 1;
	}


	//Subscribes UART
	uart->IRQ_SET_UART=uart_subscribe();

}

//Subscribes uart interrupts
int uart_subscribe()
{

	int hook_id_copy = uart->hook_id_uart;
	if(uart->comSelected==1)
	{
		if(sys_irqsetpolicy(UART_IRQ_COM1,IRQ_REENABLE|IRQ_EXCLUSIVE,&uart->hook_id_uart) != OK)
			return -1;
	}else if(uart->comSelected==2){
		if(sys_irqsetpolicy(UART_IRQ_COM2,IRQ_REENABLE|IRQ_EXCLUSIVE,&uart->hook_id_uart) != OK)
			return -1;
	}
	return BIT(hook_id_copy);

}

//Unsubscribes uart interrupts
int uart_unsubscribe()
{

	if(sys_irqrmpolicy(&uart->hook_id_uart) != OK) return 1;
	return 0;
}

//Sends a character to the Transmitter Holding Register
int charTx(unsigned char c)
{
	if(setRegisterInformation(c,THR) != OK)
	{
		printf("Error in stringTx()::charTx, sending string");
		return 1;
	}

	return 0;
}

//Transmits a string
int stringTx(unsigned char *s)
{
	while(strlen(s) >0)
	{
		if(pushQueue(uart->transmitQueue,*s) != OK) return 1;
		s++;

	}
	if(pushQueue(uart->transmitQueue,TRAIL_UART_PROTOCOL) != OK) return 1;


	return 0;
}


//prints a string for debug purposes
void printWord(char* word)
{
	printf("\n");
	do
	{
		printf("%c ", *word);
		word++;
	}while(strlen(word) >0 && *word != TRAIL_UART_PROTOCOL);
	printf("\n");
}

void printDecimalWord(char* message)
{
	printf("\nPrint:");
	do
	{
		printf("%d", *message);
		message++;
	}while(strlen(message) >0 && *message != TRAIL_UART_PROTOCOL);

}



//Checks if there is data to be read in the receive FIFO buffer
int dataInFifo()
{
	char c;

	if(getRegisterInformation(&c, LSR) != OK)
	{
		printf("In dataInFifo()::reading information from LSR\n");
		return -1;
	}
	return (c & LSR_DATA_READY_BIT);
}

//Clears the receive FIFO buffer to the receive queue
int clearReceiveFifo()
{
	char c;

	while(dataInFifo())
	{
			//Reading character from receive buffer register
			if(getRegisterInformation(&c, RBR) != OK)
			{
				printf("In clearReceiveFifo()::reading information from RBR\n");
				return 1;
			}
			//Pushing char to receive queue, if there is no error
			if(!discardString )
			{
				if(pushQueue(uart->receiveQueue, c) != OK)
				{
					printf("In clearReceiveFifo()::pushing information to queue");
					return 1;
				}
			}else if(c== TRAIL_UART_PROTOCOL) discardString=false;
	}

	return 0;
}



//Checks if it's possible to process another character
//There are two ways the uart signals the possibility to transfer another character (THR empty)
// 1- Throught an interrupt (Read in IIR)
// 2- Through a flag in the LSR (bit 5)
//This function tests the second way, the other is in the handler
int transmissionPossible()
{
	char info;

	//Reading information from line status register
	if(getRegisterInformation(&info, LSR) != OK)
	{
		printf("In transmissionPossible()::reading information from LSR\n");
		return -1;
	}

	return ((info &LSR_EMPTY_TRANSMITTER_HOLDING_REGISTER_BIT) >>5);
}



//Sends characters in the transmit queue to the transmit FIFO buffer
int processTransmitQueue()
{
	char c;
	while(!isEmptyQueue(uart->transmitQueue))
	{
		if(transmissionPossible())
		{
			c=popQueue(uart->transmitQueue);

			if(charTx(c) !=OK)
			{
				printf("In processTransmitQueue():: transmitting char\n");
				return 1;
			}
		}
	}
	return 0;
}


char* extractString()
{
	char c=0;
	char* string=NULL;
	int i=0;

	while(c!= TRAIL_UART_PROTOCOL && !isEmptyQueue(uart->receiveQueue))
	{
			//Reallocate word to prevent prior function calls garbage
			string= realloc(string, (i+1)* sizeof(*string));
			if (string == NULL)
			{
				printf("Error in extractString():: doing realloc \n");
				return string;
			}

			//Getting next in line chararter
			c=popQueue(uart->receiveQueue);

			//Inserting in the string
			string[i++]=c;

	}

	return string;
}

char* extractEvent()
{
	char c=0;
	char* string=NULL;
	int i=0;

	while(!isEmptyQueue(uart->receiveQueue) && i<(PROTOCOL_MESSAGE_SIZE-1))
	{
			//Reallocate word to prevent prior function calls garbage
			string= realloc(string, (i+1)* sizeof(*string));
			if (string == NULL)
			{
				printf("Error in extractString():: doing realloc \n");
				return string;
			}

			//Getting next in line chararter
			c=popQueue(uart->receiveQueue);

			//Inserting in the string
			string[i++]=c;

	}

	return string;
}


//Recognizes and handles the interrupts related to the Line Status Register interrupt
//Needs to be updated, right now it doesn't do anything
void lsrInterruptHandler()
{
	char interruptOrigin;

	//reads line status register information
	if(getRegisterInformation(&interruptOrigin,LSR) != OK)
	{
		printf("Error in uartErrorReceive():: Reading LSR\n");
		return ;
	}
	//Errors that require retransmission of a character(s)
	//maybe first put characters in a queue and then reorder them to the final queue

	//Parity PE
	//This error is set when the character in the top of the fifo(the next one being read to the queue)
	//Wasn't correctly transfered
	if(((interruptOrigin & LSR_PARITY_ERROR_BIT)>>2))
	{
		discardString=true;
		clearReceiveFifo();
	}

	//As we're using FIFO buffers this can occur in two cases
	//1-Data is overwritten before being read, we lost information
	//2-Fifo is full and character is received to the shift register, the interrupt
	//	will indicate that the character is overwritten but won't try to transfer it to the FIFO
	if(((interruptOrigin & LSR_OVERRUN_ERROR_BIT)>>1))
	{
		discardString=true;
		clearReceiveFifo();
	}

	//we can send information
	//Empty Transmitter Holding Register THRE
	//In the FIFO mode this bit is set when the XMIT FIFO is empty,
	//it is cleared when at least 1 byte is written to the XMIT FIFO.
	//Empty Data Holding Registers TEMT
	//In the FIFO mode this bit is set to one whenever the
	//transmitter FIFO and shift register are both empty.
	if(((interruptOrigin & LSR_EMPTY_TRANSMITTER_HOLDING_REGISTER_BIT)>>1) || ((interruptOrigin & LSR_EMPTY_DATA_HOLDING_REGISTER)>>1))
	{
		//fill transmitter fifo if we're in transmitter mode
		//clear transmitter queue to fifo
		processTransmitQueue();
	}


	//Data ready in the FIFO buffer to be read DR
	if(interruptOrigin & LSR_DATA_READY_BIT)
	{
		//Do nothing this case is covered by the IIR 	Received Data Available Interrupt
	}

}

//Handler of uart related interrupts, includes error checking
//It's intended
void uart_handler()
{
	char information;

	if(getRegisterInformation(&information,IIR) != OK)
	{
		printf("Error in uart_handler()\n");
		return ;
	}

	//Case where no interrupt is pending
	if(information & IIR_NO_INTERRUPT_PENDING_BIT) return ;

	//Filter to get only the interrupt related bits
	char interruptOrigin = (information & IIR_INTERRUPT_BITS) >>1;

	//Debug
	//printf("Interrupt origin: %d", interruptOrigin);

	//Priority (Highest on top)
	//Line Status interrupt (0110)
	//Received Data Available (0100)  && Character Timeout (1100)
	//Transmiter Holding Register (0010)
	//We then have a Modem status interrupt, which we do not use

	switch(interruptOrigin)
	{
	case(1):
		//Transmiter empty
	//	printf("Interrupt:Transmitter Holding Register Empty \n");
		processTransmitQueue();
		break;
	case(2):
		//Received data available
		//The receive Fifo buffer trigger level was reached
	//	printf("Interrupt: Received data available\n");
		clearReceiveFifo();
		//testProtocol();
		receiveCommunication();

		break;
	case (3):
		//Line Status
		//printf("Interrupt: Line status\n");
		lsrInterruptHandler();
		break;
	case(6):
		//Character timeout indication
		//A FIFO timeout interrupt will occur, if the following conditions exist
		//	– at least one character is in the FIFO
		//	– the most recent serial character received was longer than 4 continuous character times ago
		//	– the most recent CPU read of the FIFO was longer than 4 continuous character times ago.
		//Note: Timeout interrupt(Bit 3) also triggers Received data available interrupt (bit2)
		//printf("Interrupt: Character timeout indication\n");
		clearReceiveFifo();
		receiveCommunication();
		//testProtocol();
		//protocolReceiver();
		break;
	default:
		//Modem or other number
		//printf("Interrupt: %x %x Error in uart_handler\n", information, interruptOrigin	);
		break;
	}
}


///////////////////////////////////
////////Protocol functions/////////
///////////////////////////////////


//Sends a event
int sendEvent(char* event)
{

	//Push event+header
	int size= PROTOCOL_MESSAGE_SIZE-1;
	while(size-- >0)
	{
		if(pushQueue(uart->transmitQueue,*event) != OK)
		{
			printf("In sendEvent():: pushing event\n" );
			return 1;
		}
		event++;
	}

	//Push trail to transmission queue
	if(pushQueue(uart->transmitQueue,TRAIL_UART_PROTOCOL) != OK)
	{
		printf("In sendEvent():: pushing trail\n" );
		return 1;
	}

	//Transmits to FIFO
	if(processTransmitQueue()!=OK) return 1;

	return 0;
}


int sendAck()
{
	if(pushQueue(uart->transmitQueue,HEADER_ACK_SYMBOL) != OK)
	{
		printf("In sendAck():: pushing header\n" );
		return 1;
	}


	//Push trail to transmission queue
	if(pushQueue(uart->transmitQueue,TRAIL_UART_PROTOCOL) != OK)
	{
		printf("In sendAck():: pushing trail\n" );
		return 1;
	}

	if(processTransmitQueue()!=OK) return 1;

	return 0;
}

int sendHostMessage()
{
	if(pushQueue(uart->transmitQueue,HEADER_HOST_SYMBOL) != OK)
	{
		printf("In sendEvent():: pushing header\n" );
		return 1;
	}


	//Push trail to transmission queue
	if(pushQueue(uart->transmitQueue,TRAIL_UART_PROTOCOL) != OK)
	{
		printf("In sendEvent():: pushing trail\n" );
		return 1;
	}

	if(processTransmitQueue()!=OK) return 1;

	return 0;
}

int sendDoneMessage()
{
	if(pushQueue(uart->transmitQueue,HEADER_DONE_SYMBOL) != OK)
	{
		printf("In sendDoneEvent():: pushing header\n" );
		return 1;
	}


	//Push trail to transmission queue
	if(pushQueue(uart->transmitQueue,TRAIL_UART_PROTOCOL) != OK)
	{
		printf("In sendDoneEvent():: pushing trail\n" );
		return 1;
	}

	if(processTransmitQueue()!=OK) return 1;

	return 0;
}



int waitForAck()
{
	//Wait for the ack
	while(!dataInFifo()){};

	//Handle the interrupt (transfer message to the queue)
	//Wait for the message to go the queue (maybe no necessary)
	//sizeQueue(uart->receiveQueue) <2)
	while(isEmptyQueue(uart->receiveQueue)){clearReceiveFifo();}

	if(cleanQueue(uart->receiveQueue) != OK) return 1;

	return 0;
}

int receiveMessage()
{
		if(sizeQueue(uart->receiveQueue)< PROTOCOL_MESSAGE_SIZE){clearReceiveFifo(); return 1;}

		//Pops message symbol
		popQueue(uart->receiveQueue);


		switch (firstValueQueue(uart->receiveQueue)) {
			case(POSITION_SYMBOL):
				uart_position_handler();
				break;
			default:
			//	printf("Unrecognized message type \n");
				break;
		}

		return 0;


}


int receiveCommunication()
{
	//printReceiveQueue();
	if(sizeQueue(uart->receiveQueue) <PROTOCOL_ACK_SIZE) {return 1;}
	char c;
	//printf("Extracted:");printWord(message);
	/*if(sizeQueue(uart->receiveQueue) >PROTOCOL_MESSAGE_SIZE*2+1)
	{
		printf("Poped\n" );
		int size=PROTOCOL_MESSAGE_SIZE;
		while(size-->0)popQueue(uart->receiveQueue);

	}*/

	switch(firstValueQueue(uart->receiveQueue))
	{
		case (HEADER_ACK_SYMBOL):
			//printf("ACK\n");
			popQueue(uart->receiveQueue);
			if(firstValueQueue(uart->receiveQueue)== TRAIL_UART_PROTOCOL)popQueue(uart->receiveQueue);

			return 0;
			break;
		case (HEADER_SPACESHIP_POSITION_SYMBOL):
			uart_position_handler();
			return 0;
			break;
		case (HEADER_MESSAGE_SYMBOL):
			//printf("Message\n");
			//printReceiveQueue();
			return(receiveMessage());
		case (HEADER_DONE_SYMBOL):
			getBitSpace()->spaceship2->done=1;
			return 0;
			break;
		default:
			//printf("Size:%d Couldn't recognize type %c %d \n",sizeQueue(uart->receiveQueue),firstValueQueue(uart->receiveQueue),firstValueQueue(uart->receiveQueue) );
			while(!isEmptyQueue(uart->receiveQueue))
			{
				popQueue(uart->receiveQueue);
				if(isEmptyQueue(uart->receiveQueue))break;
				if(firstValueQueue(uart->receiveQueue) ==TRAIL_UART_PROTOCOL ) {printf("1\n" );popQueue(uart->receiveQueue);printf("2\n" );break;}
			}

			return 1;
	}

}



int uart_position_handler()
{
	if(sizeQueue(uart->receiveQueue)< PROTOCOL_MESSAGE_SIZE){clearReceiveFifo(); return 1;}


	//Pops message header
	popQueue(uart->receiveQueue);
	char* message = extractEvent();
	//printDecimalWord(message);
	//printf("Positions x:%d  y:%d\n",(((int)message[1]<<8) &0xFF00) | ((int)message[2] & 0xFF), (((int)message[3]<<8) &0xFF00) | ((int)message[4] & 0xFF));
	getBitSpace()->spaceship2->image->x= (((int)message[0] & PREVENT_NEGATIVE_CONVERSION)<<8) | ((int)message[1] & PREVENT_NEGATIVE_CONVERSION);
	int newY=getBitSpace()->spaceship2->image->y= (((int)message[2] & PREVENT_NEGATIVE_CONVERSION)<<8) | ((int)message[3] & PREVENT_NEGATIVE_CONVERSION);
	if(newY<=getVerRes())getBitSpace()->spaceship2->image->y= newY;
	getBitSpace()->spaceship2->angleIndex=((int)message[4]<<8) | ((int)message[5] & PREVENT_NEGATIVE_CONVERSION);
	if(message[6]) newSpaceShipShoot(getBitSpace()->spaceship2);

	return 0;
}




int protocolReceiver()
{
	while(!receiveCommunication()){}
	//printQueue(uart->receiveQueue);
	return 0;
}
