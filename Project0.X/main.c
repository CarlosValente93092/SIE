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
#include <sys/attribs.h>

#include "../Common/Config/config_bits.h"
#include "../Common/Timers/Timers.h"
#include "../Common/UART/UART.h"
#include "../Common/ADC/ADC.h"
#include "../Common/PWM/PWM.h"

#define TRIS_OUT    0       //Define TRIS as output
#define TRIS_IN     1       //Define TRIS as input

#define USE_TIMER_2 0
#define USE_TIMER_3 1

//Filter
volatile unsigned int previousVoltage;

//UART RX
volatile unsigned int RXFlag;
volatile uint8_t RXChar;

void __ISR(_UART1_VECTOR, IPL3AUTO) UART1ISR(void) {
    if (IFS0bits.U1RXIF) {
        RXFlag = 1;
        RXChar = U1RXREG;
        IFS0bits.U1RXIF = 0; //reset UART1 RX interrupt flag
    }
}

unsigned int C(unsigned int);

int main(void) {
    __XC_UART = 1; //Send input/output/errors to UART 1
    INTCONSET = _INTCON_MVEC_MASK; // Multi vector mode for interrupts

    TRISAbits.TRISA3 = TRIS_OUT; //Set port A3 to output (LED4)

    unsigned int Fout5 = 100; //Hz
    if (setupTimer5(Fout5, TIMER_USE_POLLING, 0) != TIMER_SETUP_SUCCESS) {
        PORTAbits.RA3 = 0;
        putString("TIMER 5 ERROR\n\r");
        return -1; //Error occurred
    }

    unsigned int Fout2 = 2000; //Hz
    if (setupTimer2(Fout2, TIMER_USE_POLLING, 0) != TIMER_SETUP_SUCCESS) {
        PORTAbits.RA3 = 0;
        putString("TIMER 2 ERROR\n\r");
        return -1; //Error occurred
    }

    //Use baudrate 115200 and no interrupts
    if (setupUART(115200, 1, 3) != SETUP_UART_SUCCESS) {
        PORTAbits.RA3 = 0;
        putString("UART ERROR\n\r");
        return -1; //Error occurred
    }

    //Use port A2, with 1 sample per conversion and with no interrupts
    if (setupADC(2, 16, 0) != SETUP_ADC_SUCCESS) {
        PORTAbits.RA3 = 0;
        putString("ADC ERROR\n\r");
        return -1; //Error occurred
    }

    //Use timer 2, initial duty cycle of 0 and mode 7, pin 5
    unsigned int PWM_TIMER = USE_TIMER_2;
    if (setupPWM2(PWM_TIMER, 500, 6) != SETUP_PWM_SUCCESS) {
        PORTAbits.RA3 = 0;
        putString("PWM ERROR\n\r");
        return -1; //Error occurred
    }

    __builtin_enable_interrupts(); // Turn on interrupts


    unsigned int Ton = 1; // seconds
    unsigned int Fdisplay = 60; //Hz
    unsigned count = 0;
    unsigned int voltage = 0;
    uint8_t clearLine[] = "\r                    \r"; //20 space characters to clear line

    putString("\n\r\n\rStarting Project 0\n\r");
    putString("1. Slow counting!\n\r");
    putString("2. Normal counting!\n\r");
    putString("3. Change PWM!\n\r");

    unsigned int slowRate = 1;
    previousVoltage = 0;
    unsigned int MENU = 0;
    unsigned int dutyCycle = 0;
    unsigned int maxVoltage = 0;
    unsigned int minVoltage = 33;

    while (1) {

        if (MENU == 1) { // Slow Rate
            waitTimer5();
            count = count + 1;
            resetTimer5();

            if (count > Ton * Fout5) {
                count = 0;
                PORTAbits.RA3 = !PORTAbits.RA3;

                startADC();
                waitADC();
                voltage = getValuesADC();
                resetADC();

                putString(clearLine);
                putString("Voltage: ");
                putVoltage(voltage);
                putString(" Volts");

                if (voltage > maxVoltage) maxVoltage = voltage;
                else if (voltage < minVoltage) minVoltage = voltage;
                unsigned int meanVoltage = (maxVoltage + minVoltage) / 2;

                changeDutyCycle2(USE_TIMER_2, C(meanVoltage));
            }
        }

        if (MENU == 2) {
            waitTimer5();
            resetTimer5();
            PORTAbits.RA3 = !PORTAbits.RA3;

            startADC();
            waitADC();
            voltage = getValuesADC();
            resetADC();

            putString(clearLine);
            putString("Voltage: ");
            putVoltage(voltage);
            putString(" Volts");

            if (voltage > maxVoltage) maxVoltage = voltage;
            else if (voltage < minVoltage) minVoltage = voltage;
            unsigned int meanVoltage = (maxVoltage + minVoltage) / 2;

            //            changeDutyCycle2(USE_TIMER_2, C(meanVoltage));
            changeDutyCycle2(USE_TIMER_2, C(FIR(voltage)));
        }

        if (!RXFlag) continue;
        RXFlag = 0;

        switch (RXChar) {
            case '+':
                if (MENU != 3) break;
                dutyCycle += 50;
                if (dutyCycle > 1000) dutyCycle = 1000;
                changeDutyCycle2(0, dutyCycle);
                continue;
            case '-':
                if (MENU != 3) break;
                if (dutyCycle - 50 < 0) dutyCycle = 0;
                else dutyCycle -= 50;
                changeDutyCycle2(0, dutyCycle);
                continue;
            case '1':
                if (MENU != 0) break;
                MENU = 1;
                continue;
            case '2':
                if (MENU != 0) break;
                MENU = 2;
                continue;
            case '3':
                if (MENU != 0) break;
                MENU = 3;
                dutyCycle = 0;
                putString("PWM");
                continue;
            case 27: //ESC
                if (MENU == 0) break;
                changeDutyCycle2(0, 0);
                MENU = 0;
                maxVoltage = 0;
                minVoltage = 33;
                putString(clearLine);
                continue;
            default:
                break;
        }
    }

    return (EXIT_SUCCESS);
}

unsigned int C(unsigned int voltage) {
    previousVoltage = voltage;
    unsigned int FIR = (voltage + previousVoltage) / 2;
    return (voltage * 1000) / 33;
}