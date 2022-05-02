/* ************************************************************************** */
/**

  @Authors
    Carlos Valente, NMec 93092
    Gonçalo Urbano, NMec 92949

  @File Name
    UART.h

  @Summary
    Set of functions to initialise and control UART in PIC32MX795F512L using
 Digilent chipKIT MAx32 board 
    In Linux terminal, use screen /dev/ttyUSB0 <baudrate>
 

 */
/* ************************************************************************** */

#ifndef __UART_PIC32_H__
#define __UART_PIC32_H__

#define SETUP_UART_SUCCESS 0
#define SETUP_UART_ERROR -1
#define PBCLOCK 40000000L


/********************************************************************
 * Function:    setupUART()
 * Precondition:
 * Input: 		Baudrate for UART connection and interruptFlag in case needed
 * Returns:     SETUP_UART_SUCCESS if Ok.
                SETUP_UART_ERROR in case error occurred
 * Side Effects:Sets respective pin to be an input analog pin
 * Overview:    Defines the correspondent pin to be input analog pin
 *
 * Note:
 *
 ********************************************************************/
int setupUART(unsigned int baudrate, unsigned int useInterrupts, unsigned int interruptPriority);

/********************************************************************
 * Function: 	getChar()
 * Precondition:
 * Input:
 * Returns:     Character detected by UART RX
 * Side Effects:
 * Overview:    Returns character read from the keyboard using the Serial
 *              terminal created between PC and PIC32 Board
 *
 * Note:
 *
 ********************************************************************/
int getChar(uint8_t* charRX);

/********************************************************************
 * Function: 	putChar()
 * Precondition:
 * Input: 		Character to be sent to serial terminal
 * Returns:
 * Side Effects:Stops running while TX buffer is being emptied
 * Overview:    Uses UART TX to send a character
 *
 * Note:
 *
 ********************************************************************/
void putChar(uint8_t charTX);

/********************************************************************
 * Function: 	putString()
 * Precondition:
 * Input: 		String to be sent to serial terminal
 * Returns:
 * Side Effects:Stops running while TX buffer is being emptied
 * Overview:    Uses UART TX to send multiple characters
 *
 * Note:
 *
 ********************************************************************/
void putString(uint8_t *string);

/********************************************************************
 * Function: 	waitForChar()
 * Precondition:
 * Input: 		
 * Returns:
 * Side Effects:Stops execution of program while waiting for character
 * Overview:    Waits until RX interruption flag turn 1
 * Note:
 *
 ********************************************************************/
void waitForChar(void);

/********************************************************************
 * Function: 	putInt()
 * Precondition:
 * Input: 		INteger to be sent to serial terminal
 * Returns:
 * Side Effects:Stops running while TX buffer is being emptied
 * Overview:    Uses UART TX to send multiple characters
 * Note:
 *
 ********************************************************************/
void putInt(int);

void putVoltage(unsigned int);

#endif
