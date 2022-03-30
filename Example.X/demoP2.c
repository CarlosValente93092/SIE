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

#include "../Common/Config/config_bits.h"
#include "../Common/Timers/Timers.h"
#include "../Common/UART/UART.h"
#include "../Common/PWM/PWM.h"
#include "../Common/ADC/ADC.h"

#define TRIS_OUT    0       //Define TRIS as output
#define TRIS_IN     1       //Define TRIS as input
#define OFF    0       //Set PORT/LAT to OFF
#define ON     1       //Set PORT/LAT to ON

int main(void) {
    TRISAbits.TRISA3 = TRIS_OUT; //Set port A3 to output (LED4)
    TRISGbits.TRISG9 = TRIS_OUT; //Set port G9 to output (pin 53)

    TRISEbits.TRISE0 = TRIS_OUT; //Set port A3 to output (LED4)
    TRISEbits.TRISE4 = TRIS_OUT; //Set port G9 to output (pin 53)
    TRISEbits.TRISE5 = TRIS_OUT; //Set port G9 to output (pin 53)

    TRISAbits.TRISA4 = TRIS_OUT; //Set port G9 to output (pin 53)

    PORTEbits.RE0 = 0;
    PORTEbits.RE4 = 0;
    PORTEbits.RE5 = 0;

    unsigned int Fout = 2000; //Hz
    if (setupTimer3(Fout, TIMER_USE_POLLING) == TIMER_SETUP_ERROR) {
        PORTAbits.RA3 = OFF;
        return -1; //Error occurred
    }
    if (setupPWM5(1, 50, 6) == PWM_SETUP_ERROR) {
        PORTAbits.RA3 = OFF;
        return -1; //Error occurred
    }

    if (setupUART(115200, 0) != SETUP_UART_SUCCESS) {
        PORTAbits.RA3 = 1;
        return -1; //Error occurred
    }

    if (setupADC(0, 16, 0) != SETUP_ADC_SUCCESS) {
        PORTAbits.RA3 = 1;
        return -1; //Error occurred
    }

    char message[] = "\r\nTest program.\r\n";
    char c;
    unsigned int halfPeriod = 1; // seconds
    unsigned int count = 0;

    putString(message);
    putString("Commands:\r\n");
    putString("'+': turn led on\r\n");
    putString("'-': turn led off\r\n");
    putString("'x': toggle led state\r\n\r\n");
    while (1) {
        waitTimer3();
        count += 1;
        if (count >= halfPeriod * Fout) {
            count = 0;
            PORTGbits.RG9 = !PORTGbits.RG9; //Toggle port A3
        }
        resetTimer3();

        unsigned int valueADC;
        if (IFS1bits.AD1IF) {
            valueADC = getValuesADC();
            putInt(valueADC);
            IFS1bits.AD1IF = 0;
        }

        //waitForChar();
        if (getChar(&c) != SETUP_UART_SUCCESS)
            continue;

        switch (c) {

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
            case 's':
                changeDutyCycle5(1, 0);
                break;
            case 'j':
                changeDutyCycle5(1, 250);
                break;
            case 'k':
                changeDutyCycle5(1, 500);
                break;
            case 'l':
                changeDutyCycle5(1, 750);
                break;
            case 'e':
                PORTAbits.RA4 = !PORTAbits.RA4;
                break;
            default:
                break;
        }


    }

    return (EXIT_SUCCESS);
}