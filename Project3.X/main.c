/* ************************************************************************** */
/**

  @Authors
    Carlos Valente, NMec 93092
    Gonçalo Urbano, NMec 92949

  @File Name
 main.c

  @Summary
    Main function to execute on Digilent chipKIT MAx32 board to Project 3

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

#define TRIS_OUT    0       //Define TRIS as output
#define TRIS_IN     1       //Define TRIS as input

#define USE_TIMER_2 0
#define USE_TIMER_3 1

#define MAX_COUNT 10

// Variables for UART RX
volatile unsigned int RXFlag;
volatile uint8_t RXChar;

//variables for distance
volatile int voltageA;
volatile int voltageB;
volatile unsigned int showDistance;
volatile float distance;
volatile int outOfBounds;
volatile int dutyCycle;
volatile unsigned int showDutyCycle;

//Variables for counting
volatile int voltageSumA;
volatile int voltageSumB;
volatile int count;

//Functions names
void initVariables(void); //Initialize variables at startup

void printMenuGeral(void); //Print main menu
void printMenu1(void); //Print menu 1, main project code
void printMenu2(void); //Print menu 2, change duty cycle

void printDutyCycle(void);
void getDistance(void);
void printDistance(void);
void changeConverter(void);
void changeLED(void);

//interrupt to have ADC be sampling at constant rate

void __ISR(_TIMER_1_VECTOR, IPL5AUTO) T1Interrupt(void) { //IPL5 is the priority for the interrupt
    if (showDistance) startADC();
    if (showDutyCycle) printDutyCycle();
    resetTimer1();
}

//Interrupt that occurs when ADC finishes converting all 8 samples

void __ISR(_ADC_VECTOR, IPL2AUTO) ADCISR(void) {
    count++;
    voltageSumA += getValues2ADCA();
    voltageSumB += getValues2ADCB();
    if (count == MAX_COUNT) {
        voltageA = voltageSumA / MAX_COUNT;
        voltageB = voltageSumB / MAX_COUNT;
        getDistance();
        printDistance();
        changeConverter();
        changeLED();

        voltageSumA = 0;
        voltageSumB = 0;
        count = 0;
    }
    resetADC(); //Reset ADC
}

//Interrupt for UART receiver

void __ISR(_UART1_VECTOR, IPL3AUTO) UART1ISR(void) {
    if (IFS0bits.U1RXIF) {
        RXFlag = 1;
        RXChar = U1RXREG;
        IFS0bits.U1RXIF = 0; //reset UART1 RX interrupt flag
    }
}

//Main code

int main(void) {
    __XC_UART = 1; //Send input/output/errors to UART 1
    INTCONSET = _INTCON_MVEC_MASK; // Multi vector mode for interrupts
    TRISAbits.TRISA3 = TRIS_OUT; //Set port A3 to output, led4
    TRISGbits.TRISG9 = TRIS_OUT; //Set port G9 to output (pin 53)

    if (setupUART(115200, 1, 3) != SETUP_UART_SUCCESS) {
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }

    unsigned int Fout1 = 100; //Hz, sampling frequency
    if (setupTimer1(Fout1, TIMER_USE_INTERRUPTS, 5) == TIMER_SETUP_ERROR) {
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }

    unsigned int Fout2 = 2000; //Hz, pwm frequency
    if (setupTimer2(Fout2, TIMER_USE_POLLING, 0) == TIMER_SETUP_ERROR) {
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }

    unsigned int PWM_TIMER = USE_TIMER_2;
    //ODCDbits.ODCD1 = 1; //Put pin to be open-drain
    if (setupPWM2(PWM_TIMER, 0, 6) == SETUP_PWM_ERROR) { //Pin 5, RD1
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }

    if (setup2ADC(0, 1) == SETUP_ADC_ERROR) { //Pins A0 e A1
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }

    initVariables(); //initialize variables
    printMenuGeral(); //Print main menu
    __builtin_enable_interrupts(); // Turn on interrupts
    PORTAbits.RA3 = 1; //Every setup succeeded
    unsigned int MENU = 0;

    while (1) {
        if (!RXFlag) continue; //If no key pressed then continue
        RXFlag = 0; //reset flag for UART RX

        switch (RXChar) {
            case 'p':
                putString("\n\r");
                putVoltage100(voltageA);
                putString("\n\r");
                putVoltage100(voltageB);
                putString("\n\r");
                continue;
            case '+': //Increase duty cycle (menu3)
                if (MENU != 2) break;
                dutyCycle += 5;
                if (dutyCycle > 100) dutyCycle = 100;
                changeDutyCycle2(0, dutyCycle);
                continue;
            case '-': //Decrease duty cycle (menu3)
                if (MENU != 2) break;
                dutyCycle -= 5;
                if (dutyCycle < 0) dutyCycle = 0;
                changeDutyCycle2(0, dutyCycle);
                continue;
            case '1': //Choose menu 1 (menu0)
                if (MENU != 0) break;
                printMenu1();
                MENU = 1;
                showDistance = 1;
                continue;
            case '2': //Choose menu 4 (menu0)
                if (MENU != 0) break;
                printMenu2();
                MENU = 2;
                showDutyCycle = 1;
                continue;
            case 27: //ESC, leave current menu
                if (MENU == 0) break;
                changeDutyCycle2(0, 0);
                MENU = 0;
                initVariables();
                printMenuGeral();
                continue;
            default:
                break;
        }
    }
    return (EXIT_SUCCESS);
}

void initVariables(void) {
    dutyCycle = 0; //starting with duty cycle at 0% to not start heating resistor
    showDutyCycle = 0;
    RXFlag = 0;
    distance = 0;
    showDistance = 0;
    count = 0;
    voltageSumA = 0;
    voltageSumB = 0;
    PORTGbits.RG9 = 0;
}

void printMenuGeral(void) {
    putString("\n\r\n\rSelect mode of operation:\n\r");
    putString("1. Distance\n\r");
    putString("2. Change duty cycle\n\r");
}

void printMenu1(void) {
    putString("\n\rDisplaying distances!\n\r");
    putString("(Press ESC to go back to main menu)\n\r");
}

void printMenu2(void) {
    putString("\n\rPress {+,-} to increase duty cycle!\n\r");
    putString("(Press ESC to go back to main menu)\n\r");
}

void printDutyCycle(void) {
    uint8_t clearLine[] = "\r                 \r"; //17 max space characters to clear line
    putString(clearLine);
    putString("Duty Cycle = ");
    putInt(dutyCycle);
    putString("%");
}

void getDistance() {
    if (voltageA == 330 & voltageB == 330) { //Too close to the sensor <100 mm
        outOfBounds = 1;
    } else if (voltageA == 0 & voltageB == 0) { //Too far from the sensor >600 mm
        outOfBounds = 1;
    } else if (voltageA < 330 & voltageB == 330) { //100-250mm interval
        distance = 10 / (0.0175 * voltageA / 100 + 0.0406) + 5;
        outOfBounds = 0;
    } else { //250-600mm interval
        //distance = 10 / (0.0071 * voltageB / 100 + 0.0165219);
        distance = 10 / (0.0071 * voltageB / 100 + 0.0172) - 5;
        outOfBounds = 0;
    }
}

void printDistance(void) {
    uint8_t clearLine[] = "\r             \r"; //16 max space characters to clear line
    putString(clearLine);
    putString("Distance: ");
    if (outOfBounds) {
        putString("E");
    } else {
        putInt(distance);
        putString(" mm");
    }
}

void changeConverter(void) {
    if (outOfBounds) {
        changeDutyCycle2(0, 0); //change duty cycle for the lowest (default)
    } else {
        changeDutyCycle2(0, 2 * distance / 10 - 20);
    }
}

void changeLED(void) {
    if (outOfBounds) {
        PORTGbits.RG9 = 0;
    } else {
        PORTGbits.RG9 = 1;
    }
}