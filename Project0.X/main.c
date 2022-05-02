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

//Variables for output/input control for TRIS register
#define TRIS_OUT    0       //Define TRIS as output
#define TRIS_IN     1       //Define TRIS as input

//Variables for selection of timers for PWM control
#define USE_TIMER_2 0
#define USE_TIMER_3 1

//Filter Variable
volatile unsigned int previousVoltage;

//UART RX Variables
volatile unsigned int RXFlag;
volatile uint8_t RXChar;

//Interruption function for UART RX for menus
void __ISR(_UART1_VECTOR, IPL3AUTO) UART1ISR(void) {
    if (IFS0bits.U1RXIF) {
        RXFlag = 1;
        RXChar = U1RXREG;
        IFS0bits.U1RXIF = 0; //reset UART1 RX interrupt flag
    }
}

//Function that implements filter from input to output and convert corresponding voltage to duty cycle values
unsigned int C(unsigned int);

int main(void) {
    __XC_UART = 1; //Send input/output/errors to UART 1
    INTCONSET = _INTCON_MVEC_MASK; // Multi vector mode for interrupts

    TRISAbits.TRISA3 = TRIS_OUT; //Set port A3 to output (LED4)

    // Timer for sampling
    unsigned int Fout5 = 100; //Hz
    if (setupTimer5(Fout5, TIMER_USE_POLLING, 0) != TIMER_SETUP_SUCCESS) {
        PORTAbits.RA3 = 0;
        putString("TIMER 5 ERROR\n\r");
        return -1; //Error occurred
    }

    //Timer for PWM
    unsigned int Fout2 = 2000; //Hz
    if (setupTimer2(Fout2, TIMER_USE_POLLING, 0) != TIMER_SETUP_SUCCESS) {
        PORTAbits.RA3 = 0;
        putString("TIMER 2 ERROR\n\r");
        return -1; //Error occurred
    }

    //Use baudrate 115200 and interrupts withy priority 3
    if (setupUART(115200, 1, 3) != SETUP_UART_SUCCESS) {
        PORTAbits.RA3 = 0;
        putString("UART ERROR\n\r");
        return -1; //Error occurred
    }

    //Use port A2, with 16 sample per conversion and with no interrupts
    if (setupADC(2, 16, 0) != SETUP_ADC_SUCCESS) {
        PORTAbits.RA3 = 0;
        putString("ADC ERROR\n\r");
        return -1; //Error occurred
    }

    //Use timer 2, initial duty cycle of 0 and mode 7, accessable on pin 5
    unsigned int PWM_TIMER = USE_TIMER_2;
    if (setupPWM2(PWM_TIMER, 500, 6) != SETUP_PWM_SUCCESS) {
        PORTAbits.RA3 = 0;
        putString("PWM ERROR\n\r");
        return -1; //Error occurred
    }

    __builtin_enable_interrupts(); // Turn on interrupts


    unsigned int Ton = 1; // Time on for slow rate in seconds
    unsigned int Fdisplay = 60; // Frequency to display information on terminal in Hz
    unsigned count = 0; //Count variable for slow rate
    unsigned int voltage = 0; // Voltage read by ADC
    uint8_t clearLine[] = "\r                    \r"; //20 space characters to clear line (only as aesthetic)

    putString("\n\r\n\rStarting Project 0\n\r");
    putString("1. Slow counting!\n\r");
    putString("2. Normal counting!\n\r");
    putString("3. Change PWM!\n\r");

    previousVoltage = 0; //Initial atribution of value for previous voltage
    unsigned int MENU = 0;  //Holds which menu we are on
    unsigned int dutyCycle = 0; //Duty cycle for PWM
    unsigned int maxVoltage = 0; //Max value of voltage detected
    unsigned int minVoltage = 33; //Min value of voltage detected

    while (1) {

        if (MENU == 1) { // Slow Rate Sampling
            waitTimer5(); //Wait for timer to finish
            count = count + 1; //Can change count to reduce time between samples
            resetTimer5(); //reset timer

            if (count > Ton * Fout5) {
                count = 0; //reset count
                PORTAbits.RA3 = !PORTAbits.RA3; //Invert LED

                startADC(); //Start Sampling
                waitADC(); //Wait for ADC to end
                voltage = getValuesADC(); //Return ADC values
                resetADC(); //Reset ADC

                //Print voltage information
                putString(clearLine);
                putString("Voltage: ");
                putVoltage(voltage);
                putString(" Volts");

                //Update max or min voltages accordingly
                if (voltage > maxVoltage) maxVoltage = voltage;
                else if (voltage < minVoltage) minVoltage = voltage;
                unsigned int meanVoltage = (maxVoltage + minVoltage) / 2; //Mean voltage
                //Change duty cycle to the mean voltage
                changeDutyCycle2(USE_TIMER_2, C(meanVoltage)); //(e.g. 0-3.3 gives 1.65 mean meaning 50%)
            }
        }

        if (MENU == 2) { //Normal Rate Sampling
            waitTimer5(); //wait for timer to finish
            resetTimer5(); //Reset timer

            startADC(); //start sampling
            waitADC(); //wait for ADC to finish
            voltage = getValuesADC(); //Get values
            resetADC(); //Reset ADC

            //Print voltage information
            putString(clearLine);
            putString("Voltage: ");
            putVoltage(voltage);
            putString(" Volts");

            //Update max or min voltages accordingly
            if (voltage > maxVoltage) maxVoltage = voltage;
            else if (voltage < minVoltage) minVoltage = voltage;
            unsigned int meanVoltage = (maxVoltage + minVoltage) / 2; //Mean voltage
            //Change duty cycle to the current voltage after passing a LP filter (FIR)
            changeDutyCycle2(USE_TIMER_2, C(voltage);
        }

        // Skip if it didn't receive a character
        if (!RXFlag) continue;
        RXFlag = 0; //Reset flag

        switch (RXChar) {
            case '+': //Increment duty cycle
                if (MENU != 3) break;
                dutyCycle += 50; //Increments of 5%
                if (dutyCycle > 1000) dutyCycle = 1000; //Limit of 100% for duty cycle
                changeDutyCycle2(0, dutyCycle); //change duty cycle
                continue;
            case '-':  //Decrement duty cycle
                if (MENU != 3) break;
                if (dutyCycle - 50 < 0) dutyCycle = 0; //limit of 0% for duty cycle
                else dutyCycle -= 50; //Decrement of 5%
                changeDutyCycle2(0, dutyCycle); //change duty cycle
                continue;
            case '1': //Slow sampling rate menu
                if (MENU != 0) break;
                MENU = 1; //Select menu 1
                continue;
            case '2': //normal sampling rate menu
                if (MENU != 0) break;
                MENU = 2; //Select menu 2
                continue;
            case '3': //manual increment/decrement of duty cycle
                if (MENU != 0) break;
                MENU = 3; //Select menu 3
                dutyCycle = 0; //Change duty cycle to be at 0%
                putString("PWM"); //Posicional string to indicate we are on menu 3
                continue;
            case 27: //ESC, return to previous menu (if possible)
                if (MENU == 0) break;
                changeDutyCycle2(0, 0); //Change duty cycle to be at 0%
                MENU = 0; //back to main menu
                maxVoltage = 0; //set max voltage to minimum
                minVoltage = 33;  //set min voltage to maximum
                putString(clearLine); //clear line of terminal
                continue;
            default: //anything else
                break;
        }
    }
    return (EXIT_SUCCESS);
}

unsigned int C(unsigned int voltage) {
    unsigned int FIR = (voltage + previousVoltage) / 2; //Simple low pass FIR filter (attenuates rapid transitions)
    previousVoltage = voltage; //save current voltage for next calculation
    return (voltage * 1000) / 33; //Associate 3.3V to 100% for duty cycle
}
