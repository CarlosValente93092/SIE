/* ************************************************************************** */
/**

  @Authors
    Carlos Valente, NMec 93092
    Gonçalo Urbano, NMec 92949

  @File Name
 main.c

  @Summary
    Main function to execute on Digilent chipKIT MAx32 board to Project 0

 */
/* ************************************************************************** */

#include <xc.h>
#include <stdio.h>

#include "../Common/Config/config_bits.h"
#include "../Common/Timers/Timers.h"
#include "../Common/UART/UART.h"
#include "../Common/ADC/ADC.h"
#include "../Common/PWM/PWM.h"

#define TRIS_OUT    0       //Define TRIS as output
#define TRIS_IN     1       //Define TRIS as input

#define USE_TIMER_2 0
#define USE_TIMER_3 1

void printMenu(void);

int main(void) {

    TRISAbits.TRISA3 = TRIS_OUT; //Set port A3 to output, led4
    TRISAbits.TRISA4 = TRIS_OUT; //Set port A4 to output, pin 72

    TRISDbits.TRISD1 = TRIS_OUT; //Set port D1 to output, pin 5
    TRISDbits.TRISD2 = TRIS_OUT; //Set port D2 to output, pin 6

    TRISDbits.TRISD7 = TRIS_IN; //Set port D7 to input, pin 77
    TRISDbits.TRISD9 = TRIS_IN; //Set port D9 to input, pin 74


    if (setupUART(115200, 0) != SETUP_UART_SUCCESS) {
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }

    int Fout = 20000; //Hz
    if (setupTimer2(Fout, TIMER_USE_POLLING) == TIMER_SETUP_ERROR) {
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }

    unsigned int PWM_TIMER = USE_TIMER_2;
    if (setupPWM2(PWM_TIMER, 0, 7) == PWM_SETUP_ERROR) {
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }
    if (setupPWM3(PWM_TIMER, 0, 7) == PWM_SETUP_ERROR) {
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }


    PORTAbits.RA3 = 1;
    PORTAbits.RA4 = 0;
    uint8_t c;
    printMenu();
    while (1) {
        if (getChar(&c) != SETUP_UART_SUCCESS)
            continue;

        switch (c) {
            case 'm':
                changeDutyCycle2(PWM_TIMER, 500);
                changeDutyCycle3(PWM_TIMER, 0);
                break;
            case 'n':
                changeDutyCycle2(PWM_TIMER, 0);
                changeDutyCycle3(PWM_TIMER, 500);
                break;
            case 's':
                changeDutyCycle2(PWM_TIMER, 0);
                changeDutyCycle3(PWM_TIMER, 0);
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

void printMenu(void) {
    putString("Commands:\r\n");
    putString("'+': turn led on\r\n");
    putString("'-': turn led off\r\n");
    putString("'x': toggle led state\r\n\r\n");
}