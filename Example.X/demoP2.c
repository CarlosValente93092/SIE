/* ************************************************************************** */
/**

  @Authors
    Carlos Valente, NMec 93092
    Gonçalo Urbano, NMec 92949

  @File Name
    Timers.h

  @Summary
    Main function to execute on Digilent chipKIT MAx32 board

 */
/* ************************************************************************** */

#include <xc.h>
#include <stdio.h>
#include <sys/attribs.h>

#include "../Common/Config/config_bits.h"
#include "../Common/Timers/Timers.h"
#include "../Common/UART/UART.h"
#include "../Common/PWM/PWM.h"
#include "../Common/ADC/ADC.h"
#include "../Common/ExternalInterrupt/ExternalInterrupt.h"

#define TRIS_OUT    0       //Define TRIS as output
#define TRIS_IN     1       //Define TRIS as input

volatile unsigned int count;
volatile unsigned int countX;
volatile unsigned int int0_int_flag;
volatile unsigned int RXFlag;
volatile uint8_t character;

void __ISR(_TIMER_3_VECTOR, IPL5AUTO) T3Interrupt(void) { //IPL5 is the priority for the interrupt
    count++;
    resetTimer3();
}

void __ISR(_EXTERNAL_0_VECTOR, IPL3AUTO) ExtInt0ISR(void) { //IPL3 is the priority for the interrupt
    int0_int_flag = 1;
    countX++;
    IFS0bits.INT0IF = 0; // Reset interrupt flag
}

void __ISR(_UART1_VECTOR, IPL3AUTO) UART1ISR(void) {
    //    if (IFS0bits.U1TXIF) {
    //        putString("Transmission\n\r");
    //        IFS0bits.U1TXIF = 0; // Reset UART1 TX interrupt flag
    //    }

    if (IFS0bits.U1RXIF) {
        RXFlag = 1;
        character = U1RXREG;
        IFS0bits.U1RXIF = 0; //reset UART1 RX interrupt flag
    }
}

int main(void) {
    __XC_UART = 1; //Send input/output/errors to UART 1
    INTCONSET = _INTCON_MVEC_MASK; // Multi vector mode for interrupts


    TRISAbits.TRISA3 = TRIS_OUT; //Set port A3 to output (LED4)
    TRISGbits.TRISG9 = TRIS_OUT; //Set port G9 to output (pin 53)
    TRISGbits.TRISG8 = TRIS_OUT; //Set port G8 to output (pin 51)

    unsigned int Fout = 2000; //Hz
    unsigned int InterruptPriorityTimer3 = 5;
    if (setupTimer3(Fout, TIMER_USE_INTERRUPTS, InterruptPriorityTimer3) == TIMER_SETUP_ERROR) {
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }

    unsigned int PWMTIMER = 1;
    if (setupPWM5(PWMTIMER, 50, 6) == SETUP_PWM_ERROR) {
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }

    if (setupUART(115200, 1, 3) != SETUP_UART_SUCCESS) {
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }

    if (setupADC(0, 16, 0) != SETUP_ADC_SUCCESS) {
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }

    unsigned int InterruptPriorityEXT0 = 3;
    if (setuptExt0(EXTERNAL_RISING_EDGE, InterruptPriorityEXT0) != SETUP_EXTERNAL_SUCCESS) {
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }


    __builtin_enable_interrupts(); // Turn on interrupts
    PORTAbits.RA3 = 1; //Every setup succeeded

    count = 0; //Set to 0 global count variable
    countX = 0;
    int0_int_flag = 0;

    unsigned int halfPeriod = 1; // seconds
    uint8_t clearLine[] = "                              "; //30 space characters to clear line

    putString("\r\nTest program.\r\n");
    putString("Commands:\r\n");
    putString("'+': turn led on\r\n");
    putString("'-': turn led off\r\n");
    putString("'x': toggle led state\r\n\r\n");
    while (1) {
        if (count >= halfPeriod * Fout) {
            count = 0;
            PORTGbits.RG9 = !PORTGbits.RG9; //Toggle port G9
            PORTGbits.RG8 = !PORTGbits.RG8; //Toggle port G8
        }

        if (int0_int_flag) {
            int0_int_flag = 0;
            putString(clearLine);
            putString("\rHello ");
            putInt(countX);
        }

        unsigned int valueADC;
        if (IFS1bits.AD1IF) {
            valueADC = getValuesADC();
            putInt(valueADC);
            IFS1bits.AD1IF = 0;
        }

        if (!RXFlag) continue;

        RXFlag = 0;
        putChar(character);
        switch (character) {

            case '+':
                /* Turn led on */
                PORTAbits.RA3 = 1;
                break;

            case '-':
                /* Turn led off */
                PORTAbits.RA3 = 0;
                break;
            case 'x':
                /* Toggle led state */
                PORTAINV = 0x08;
                break;
            case 'g':
                changeDutyCycle5(PWMTIMER, 0);
                break;
            case 'h':
                changeDutyCycle5(PWMTIMER, 250);
                break;
            case 'j':
                changeDutyCycle5(PWMTIMER, 500);
                break;
            case 'k':
                changeDutyCycle5(PWMTIMER, 750);
                break;
            case 'l':
                changeDutyCycle5(PWMTIMER, 1000);
                break;
            default:
                break;
        }


    }

    return (EXIT_SUCCESS);
}