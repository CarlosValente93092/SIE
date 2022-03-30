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
#include <math.h>

#include "../Common/Config/config_bits.h"
#include "../Common/Timers/Timers.h"
#include "../Common/UART/UART.h"
#include "../Common/ADC/ADC.h"
#include "../Common/PWM/PWM.h"

#define TRIS_OUT    0       //Define TRIS as output
#define TRIS_IN     1       //Define TRIS as input

#define USE_TIMER_2 0
#define USE_TIMER_3 1

//unsigned int pow(unsigned int, unsigned int);

int main(void) {


    TRISAbits.TRISA3 = TRIS_OUT; //Set port A3 to output (LED4)
    PORTAbits.RA3 = 0;

    int Fout4 = 100; //Hz
    if (setupTimer4(Fout4, TIMER_USE_POLLING) != TIMER_SETUP_SUCCESS) {
        PORTAbits.RA3 = 0;
        putString("TIMER 4 ERROR\n\r");
        return -1; //Error occurred
    }

    //Use baudrate 115200 and no interrupts
    if (setupUART(115200, 0) != SETUP_UART_SUCCESS) {
        PORTAbits.RA3 = 0;
        putString("UART ERROR\n\r");
        return -1; //Error occurred
    }

    //Use port A2, with 1 sample per conversion and with no interrupts
    if (setupADC(2, 1, 0) != SETUP_ADC_SUCCESS) {
        PORTAbits.RA3 = 0;
        putString("ADC ERROR\n\r");
        return -1; //Error occurred
    }

    unsigned int Fout2 = 10; //Hz
    if (setupTimer2(Fout2, TIMER_USE_POLLING) != PWM_SETUP_SUCCESS) {
        PORTAbits.RA3 = 0;
        putString("TIMER 2 ERROR\n\r");
        return -1; //Error occurred
    }
    //Use timer 2, initial duty cycle of 0 and mode 7
    unsigned int PWM_TIMER = USE_TIMER_2;
    if (setupPWM4(PWM_TIMER, 1000, 7) != PWM_SETUP_SUCCESS) {
        PORTAbits.RA3 = 0;
        putString("PWM ERROR\n\r");
        return -1; //Error occurred
    }

    //    int Ton = 1; // seconds
    //    int count = 0;
    //    uint64_t rawValue;
    PORTAbits.RA3 = 1;
    char dutyCycleArray[] = {0, 0, 0, 0};
    unsigned int index = 0;
    unsigned count = 0;
    putString("Insert Duty Cycle: ");
    while (1) {
        waitTimer4();

        uint8_t character;
        waitForChar();
        getChar(&character);
        putChar(character);

        if (character == '\r') {
            putChar('\n');
            if (index > 3) {
                putString("Invalid Duty Cycle\n\r");
                index = 0;
                continue;
            }
            unsigned int dutyCycle;
            for (int i = 0; i < index; i++) {
                dutyCycle += (dutyCycleArray[i] - 48) * pow(10, index - i - 1);
            }
            changeDutyCycle4(PWM_TIMER, dutyCycle);
            index = 0;
            putString("Insert Duty Cycle: ");
        }

        if (index < 4 && character >= '0' && character <= '9') {
            dutyCycleArray[index] = character;
        }
        index++;
        resetTimer4();




        //        waitTimer4();
        //        resetTimer4();
        //        count = count + 1;
        //        if (count > Ton * Fout4) {
        //            count = 0;
        //            PORTAbits.RA3 = !PORTAbits.RA3;
        //
        //            startADC();
        //            waitADC();
        //            int voltage = getValuesADC(&rawValue);
        //            resetADC();
        //
        //            putString("Voltage: ");
        //            putInt(voltage);
        //            putString(" ,Mean: ");
        //            putInt(rawValue);
        //            putString("\n\r");
        //        }

    }

    return (EXIT_SUCCESS);
}

////unsigned int pow(unsigned int baseNumber, unsigned int powerNumber) {
//    unsigned int value = 1;
//    for (int i = 0; i < powerNumber; i++) {
//        value *= baseNumber;
//    }
//    return value;
//}
