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
volatile unsigned int RXFlag; //flag to signal main code
volatile uint8_t RXChar; //Character detected

//variables for distance
volatile int voltageA; //holds the value for voltage of 10-25cm distances
volatile int voltageB; //holds the value for voltage of 25-60cm distances
volatile unsigned int showDistance; //Flag start converting
volatile float distance; //holds the distance calculated by the linearization
volatile int outOfBounds; //flag to signal object is out of bounds (out of distance)
volatile int dutyCycle; //holds the value for duty cycle for PWM
volatile unsigned int showDutyCycle; //flag to signal prints of the duty cycle

//Variables for counting (for values to not change abruptly)
volatile int voltageSumA; //holds many voltages from 10-25 cm
volatile int voltageSumB; //holds many voltages from 25-60 cm
volatile int count; //Counts the iterations in which is in

//Functions names
void initVariables(void); //Initialize variables at startup

void printMenuGeral(void); //Print main menu
void printMenu1(void); //Print menu 1, main project code
void printMenu2(void); //Print menu 2, change duty cycle

void printDutyCycle(void); //prints duty cycle
void getDistance(void); //calculates the distance using linearization of the voltages
void printDistance(void); //prints the distance calculated
void changeConverter(void); //Changes the duty cycle for the PWM 
void changeLED(void); //Changes states os LED to ON if object is in range

//timer interrupt to have ADC be sampling at constant rate

void __ISR(_TIMER_1_VECTOR, IPL5AUTO) T1Interrupt(void) { //IPL5 is the priority for the interrupt
    if (showDistance) startADC(); //if menu 1 is chosen, then start sampling
    if (showDutyCycle) printDutyCycle(); //if menu 2 is chosen, then print the duty cycle
    resetTimer1(); //Reset timer
}

//Interrupt that occurs when ADC finishes converting all 8 samples

void __ISR(_ADC_VECTOR, IPL2AUTO) ADCISR(void) {
    count++; //increase count 
    voltageSumA += getValues2ADCA(); //get values from the 10-25 cm ADC
    voltageSumB += getValues2ADCB(); //get values from the 25-60 cm ADC
    if (count == MAX_COUNT) { //When it reaches 10 iterations
        voltageA = voltageSumA / MAX_COUNT; //Get the average voltage
        voltageB = voltageSumB / MAX_COUNT; //Get the average voltage
        getDistance(); //Calculates distance
        printDistance(); //prints distance to user terminal
        changeConverter(); //Changes duty cycle for pwm
        changeLED(); //Change LED state

        voltageSumA = 0; //reset counting variables
        voltageSumB = 0; //reset counting variables
        count = 0; //reset counting variables
    }
    resetADC(); //Reset ADC
}

//Interrupt for UART receiver

void __ISR(_UART1_VECTOR, IPL3AUTO) UART1ISR(void) {
    if (IFS0bits.U1RXIF) {
        RXFlag = 1; //indicates a character as been typed
        RXChar = U1RXREG; //Collects the character from uart
        IFS0bits.U1RXIF = 0; //reset UART1 RX interrupt flag
    }
}

//Main code

int main(void) {
    __XC_UART = 1; //Send input/output/errors to UART 1
    INTCONSET = _INTCON_MVEC_MASK; // Multi vector mode for interrupts
    TRISAbits.TRISA3 = TRIS_OUT; //Set port A3 to output, led4
    TRISGbits.TRISG9 = TRIS_OUT; //Set port G9 to output (pin 53)

    if (setupUART(115200, 1, 3) != SETUP_UART_SUCCESS) { //setup uart with baudrate of 115200, with interrupts and priority of 3
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }

    unsigned int Fout1 = 100; //Hz, sampling frequency
    if (setupTimer1(Fout1, TIMER_USE_INTERRUPTS, 5) == TIMER_SETUP_ERROR) { //setup timer 1 as main timer for system sampling
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }

    unsigned int Fout2 = 2000; //Hz, pwm frequency
    if (setupTimer2(Fout2, TIMER_USE_POLLING, 0) == TIMER_SETUP_ERROR) { //setup timer 2 for pwm usage
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }

    unsigned int PWM_TIMER = USE_TIMER_2; //Uses timer 2 for PWM
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
            case 'p': //If 'p' is typed, then prints to user terminal the voltages read in that instance (for debug purposes)
                putString("\n\r");
                putVoltage100(voltageA); //print voltage for 10-25 cm
                putString("\n\r");
                putVoltage100(voltageB); //print voltage for 25-60 cm
                putString("\n\r");
                continue;
            case '+': //Increase duty cycle (menu3)
                if (MENU != 2) break;
                dutyCycle += 5; //increment duty cycle
                if (dutyCycle > 100) dutyCycle = 100; //upper limit for duty cycle is 100
                changeDutyCycle2(0, dutyCycle); //change duty cycle
                continue;
            case '-': //Decrease duty cycle (menu3)
                if (MENU != 2) break;
                dutyCycle -= 5; //decrement duty cycle
                if (dutyCycle < 0) dutyCycle = 0; //bottom limit for duty cycle is 0
                changeDutyCycle2(0, dutyCycle); //change duty cycle
                continue;
            case '1': //Choose menu 1 (menu0)
                if (MENU != 0) break;
                printMenu1(); //print menu 1
                MENU = 1; //Select Menu 1
                showDistance = 1; //Start converting
                continue;
            case '2': //Choose menu 4 (menu0)
                if (MENU != 0) break;
                printMenu2(); //print menu 2
                MENU = 2; //Select Menu 2
                showDutyCycle = 1; //start showing duty cycle
                continue;
            case 27: //ESC, leave current menu
                if (MENU == 0) break;
                changeDutyCycle2(0, 0); //resets PWM's duty cycle
                MENU = 0; //Select Menu 0
                initVariables(); //Resets all variables to default
                printMenuGeral(); //prints initial menu
                continue;
            default:
                break;
        }
    }
    return (EXIT_SUCCESS);
}

void initVariables(void) {
    dutyCycle = 0; //starting with duty cycle at 0% to not start heating resistor
    showDutyCycle = 0; //do not show duty cycle
    RXFlag = 0; //no character detected
    distance = 0; //distance read is 0
    showDistance = 0; //stop converting
    count = 0; //count reset
    voltageSumA = 0; //Average reset
    voltageSumB = 0; //Average reset
    PORTGbits.RG9 = 0; //LED is turn off
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
        distance = 10 / (0.0175 * voltageA / 100 + 0.0406) + 5; //small offset to adjust to real distance read
        outOfBounds = 0;
    } else { //250-600mm interval
        //distance = 10 / (0.0071 * voltageB / 100 + 0.0165219);
        distance = 10 / (0.0071 * voltageB / 100 + 0.0172) - 5; //small offset to adjust to real distance read
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
