/* ************************************************************************** */
/**

  @Authors
    Carlos Valente, NMec 93092
    Gonçalo Urbano, NMec 92949

  @File Name
    UART.c

  @Summary
    Set of functions to initialise and control UART in PIC32MX795F512L using
 Digilent chipKIT MAx32 board
    In Linux terminal, use screen /dev/ttyUSB0 <baudrate>


 */
/* ************************************************************************** */


#include <xc.h>
#include "UART.h"

#define SIZE_NUMBER_DECIMAL 10

int setupUART(unsigned int baudrate, unsigned int useInterrupts) {
    if ((baudrate > 115200) || (baudrate < 600)) return SETUP_UART_ERROR;
    U1BRG = ((PBCLOCK + 8 * baudrate) / (16 * baudrate)) - 1;

    U1MODEbits.BRGH = 0; //High Baud Rate Enable disabled
    U1MODEbits.PDSEL = 0; //8 bit data with no parity
    U1MODEbits.STSEL = 0; //1 stop bits

    U1STAbits.UTXEN = 1; //Enable transmitter module
    U1STAbits.URXEN = 1; //Enable receiver module

    if (useInterrupts) {
        U1STAbits.UTXSEL = 0; //Property for transmission interrupt
        U1STAbits.URXISEL = 0; //Property for recession interrupt

        IEC0bits.U1TXIE = 0; //Enable transmission interrupts
        IEC0bits.U1RXIE = 1; //Enable recession interrupts
        IPC6bits.U1IP = 2; //Define interrupt priority
    }
    IFS0bits.U1RXIF = 0; //Clear recession interrupt flag
    U1MODEbits.ON = 1; //Enable UART 1

    return SETUP_UART_SUCCESS;
}

int getChar(uint8_t* charRX) {
    if (U1STAbits.OERR || U1STAbits.FERR || U1STAbits.PERR) {
        U1STAbits.OERR = 0; // clear OERR
    }

    if (U1STAbits.URXDA) {
        *charRX = U1ARXREG; // get data from UART RX FIFO
        return SETUP_UART_SUCCESS;
    }

    return SETUP_UART_ERROR;
}

void putChar(uint8_t charTX) {
    while (U1STAbits.UTXBF); // wait for TX buffer to be empty
    U1ATXREG = charTX;
}

void putString(uint8_t *string) {
    while (*string != '\0') {
        putChar(*string);
        string += 1;
    }
}

void waitForChar(void) {
    while (!IFS0bits.U1RXIF);
    IFS0bits.U1RXIF = 0;
}

void putInt(int number) {
    if (number > INT32_MAX) putString("Number to print bigger than INT32_MAX\n");
    if (number == 0) putChar('0');

    int digitsBuffer[SIZE_NUMBER_DECIMAL]; // INT32_MAX = 4294967295 (10 digits))
    char charBuffer[SIZE_NUMBER_DECIMAL + 1]; // Max of 10 digits plus '\0'

    for (int i = 0; i < SIZE_NUMBER_DECIMAL; i++) {
        digitsBuffer[i] = number % 10;
        number /= 10;
    }

    unsigned int firstDigitFlag = 0; //Set flag to false
    unsigned int charBufferIndex = 0;
    for (int i = 0; i < SIZE_NUMBER_DECIMAL; i++) {
        unsigned int digitIndex = SIZE_NUMBER_DECIMAL - i - 1;
        if (!firstDigitFlag && !digitsBuffer[digitIndex]) continue;
        firstDigitFlag = 1; //set flag to true
        charBuffer[charBufferIndex] = digitsBuffer[digitIndex] + 48;
        charBufferIndex++;
    }
    charBuffer[charBufferIndex] = 0;
    putString(charBuffer);
}



///* PRIVATE - Needed for printf() function from stdio library */
//void _mon_putc(char c) {
//    putChar((uint8_t) c);
//}
