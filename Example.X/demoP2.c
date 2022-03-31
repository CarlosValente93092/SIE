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

void __ISR(_TIMER_3_VECTOR, IPL5AUTO) T3Interrupt(void) { //IPL5 is the priority for the interrupt
    count++;
    resetTimer3();
}

void __ISR(_EXTERNAL_0_VECTOR, IPL3AUTO) ExtInt0ISR(void) { //IPL3 is the priority for the interrupt
    int0_int_flag = 1;
    countX++;
    IFS0bits.INT0IF = 0; // Reset interrupt flag
}

int main(void) {
    __XC_UART = 1; //Send input/output/errors to UART 1
    INTCONSET = _INTCON_MVEC_MASK; // Multi vector mode for interrupts


    TRISAbits.TRISA3 = TRIS_OUT; //Set port A3 to output (LED4)
    TRISGbits.TRISG9 = TRIS_OUT; //Set port G9 to output (pin 53)

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

    if (setupUART(115200, 0) != SETUP_UART_SUCCESS) {
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

    char c;
    unsigned int halfPeriod = 1; // seconds

    putString("\r\nTest program.\r\n");
    putString("Commands:\r\n");
    putString("'+': turn led on\r\n");
    putString("'-': turn led off\r\n");
    putString("'x': toggle led state\r\n\r\n");
    while (1) {
        if (count >= halfPeriod * Fout) {
            count = 0;
            PORTGbits.RG9 = !PORTGbits.RG9; //Toggle port A3
        }

        if (int0_int_flag) {
            int0_int_flag = 0;
            putString("Hello darling ");
            putInt(countX);
            putString("\n\r");
        }

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
                changeDutyCycle5(PWMTIMER, 0);
                break;
            case 'j':
                changeDutyCycle5(PWMTIMER, 250);
                break;
            case 'k':
                changeDutyCycle5(PWMTIMER, 500);
                break;
            case 'l':
                changeDutyCycle5(PWMTIMER, 750);
                break;
            default:
                break;
        }


    }

    return (EXIT_SUCCESS);
}