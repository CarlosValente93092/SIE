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
#include <sys/attribs.h>

#include "../Common/Config/config_bits.h"
#include "../Common/Timers/Timers.h"
#include "../Common/UART/UART.h"
#include "../Common/PWM/PWM.h"
#include "../Common/ExternalInterrupt/ExternalInterrupt.h"

#define TRIS_OUT    0       //Define TRIS as output
#define TRIS_IN     1       //Define TRIS as input

#define USE_TIMER_2 0
#define USE_TIMER_3 1

#define ROTATING_TO_LEFT 0
#define ROTATING_TO_RIGHT 1

volatile unsigned int pulseCount;
volatile unsigned int rpm;
volatile unsigned int angularPosition;
volatile unsigned int rotation;

volatile unsigned int RXFlag;
volatile uint8_t RXChar;

void printMenuGeral(void);
void printMenu1(void);
void printMenu2(void);
void printMenu3(void);
unsigned int validateCommand(uint8_t *command, unsigned int sizeOfArray, unsigned int menu);
void executeCommand(uint8_t *command, unsigned int sizeOfArray, unsigned int menu);
void resetCommand(uint8_t *command, unsigned int sizeOfArray);
unsigned int raiseToPower(unsigned int baseNumber, unsigned int expoentNumber);

void __ISR(_TIMER_4_VECTOR, IPL5AUTO) T3Interrupt(void) { //IPL5 is the priority for the interrupt
    //    rpm = function();
    //    angularPosition = function();
    //    rotation = function();
    pulseCount = 0; //Reset pulse count
    resetTimer4();
}

void __ISR(_EXTERNAL_0_VECTOR, IPL3AUTO) ExtInt0ISR(void) { //IPL3 is the priority for the interrupt
    pulseCount++;
    IFS0bits.INT0IF = 0; // Reset interrupt flag
}

void __ISR(_UART1_VECTOR, IPL3AUTO) UART1ISR(void) {
    if (IFS0bits.U1RXIF) {
        RXFlag = 1;
        RXChar = U1RXREG;
        IFS0bits.U1RXIF = 0; //reset UART1 RX interrupt flag
    }
}

int main(void) {
    __XC_UART = 1; //Send input/output/errors to UART 1
    INTCONSET = _INTCON_MVEC_MASK; // Multi vector mode for interrupts

    TRISAbits.TRISA3 = TRIS_OUT; //Set port A3 to output, led4

    if (setupUART(115200, 1, 3) != SETUP_UART_SUCCESS) {
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }

    int Fout4 = 100; //Hz
    if (setupTimer4(Fout4, TIMER_USE_INTERRUPTS, 5) == TIMER_SETUP_ERROR) {
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }

    int Fout2 = 20000; //Hz
    if (setupTimer2(Fout2, TIMER_USE_POLLING, 0) == TIMER_SETUP_ERROR) {
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }

    unsigned int PWM_TIMER = USE_TIMER_2;
    if (setupPWM2(PWM_TIMER, 0, 7) == SETUP_PWM_ERROR) { //Pin 5, RD1
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }

    unsigned int InterruptPriorityEXT0 = 3;
    if (setuptExt0(EXTERNAL_RISING_EDGE, InterruptPriorityEXT0) != SETUP_EXTERNAL_SUCCESS) { //Pin 3, RD0
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }


    __builtin_enable_interrupts(); // Turn on interrupts
    PORTAbits.RA3 = 1; //Every setup succeeded

    unsigned int MENU = 0;
    uint8_t clearLine[] = "\r                              \r"; //30 space characters to clear line
    uint8_t command[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int index = 0;

    printMenuGeral();
    while (1) {
        if (!RXFlag) continue;
        RXFlag = 0;

        switch (RXChar) {
            case '1':
                if (MENU != 0) break;
                printMenu1();
                MENU = 1;
                continue;

            case '2':
                if (MENU != 0) break;
                printMenu2();
                MENU = 2;
                continue;
            case '3':
                if (MENU != 0) break;
                printMenu3();
                MENU = 3;
                continue;
            case 13: //ENTER
                if (MENU == 0) break;
                __builtin_disable_interrupts();
                if (!validateCommand(command, index, MENU)) {
                    putString(clearLine);
                    putString("Error! Invalid input!");
                    continue;
                }
                executeCommand(command, index, MENU);
                __builtin_enable_interrupts();
                continue;
            case 27: //ESC
                if (MENU == 0) break;
                resetCommand(command, index);
                printMenuGeral();
                MENU = 0;
                continue;
            case 127: //BACKSPACE
                if (MENU == 0) break;
                if (index == 0) break;
                index--;
                command[index] = 0;
                putString(clearLine);
                putString(command);
                continue;
            default:
                break;
        }

        if (MENU != 0) { //If in any menu, listen to the input
            command[index] = RXChar;
            index++;
            putString(clearLine);
            putString(command);
        }
    }

    return (EXIT_SUCCESS);
}

void printMenuGeral(void) {

    putString("\n\rSelect mode of operation:\n\r");
    putString("1. Set motor RPM\n\r");
    putString("2. Set angle for motor to turn\n\r");
    putString("3. Select from bundle pack\n\r");
}

void printMenu1(void) {

    putString("\n\rInsert rpm for motor followed by direction {+,-} (e.g. 30+ or 45-)\n\r");
    putString("(Press ESC to go back to main menu)\n\r");
}

void printMenu2(void) {

    putString("\n\rInsert angle to rotate the motor (e.g. 180 or -360)\n\r");
    putString("(Press ESC to go back to main menu)\n\r");
}

void printMenu3(void) {
    putString("\n\rWelcome to bundle selector! Choose:\n\r");
    putString("1. Perform 180 degree turn clockwise\n\r");
    putString("2. Increase from 10 rpm to 50 rpm\n\r");
    putString("3. Decrease from 50 rpm to 10 rpm\n\r");
    putString("(Press ESC to go back to main menu)\n\r");
}

unsigned int validateCommand(uint8_t *command, unsigned int sizeOfArray, unsigned int menu) {
    for (int i = 0; i < sizeOfArray; i++) {
        if ((command[i] >= '0' && command[i] <= '9') || command[i] == '+' || command[i] == '-')
            continue;
        return 0; //The condition above was not verified, error!
    }
    return 1;
}

void executeCommand(uint8_t *command, unsigned int sizeOfArray, unsigned int menu) {
    uint8_t h = 0;
    unsigned int x = 0;
    unsigned int y = 0;
    unsigned int z = 0;

    unsigned int wantedRPM = 0;
    switch (menu) {
        case 1:
            if (command[sizeOfArray - 1] == '+') rotation = ROTATING_TO_RIGHT;
            if (command[sizeOfArray - 1] == '-') rotation = ROTATING_TO_LEFT;
            for (int i = 0; i < sizeOfArray - 1; i++) {
                wantedRPM += (command[i] - 48) * raiseToPower(10, sizeOfArray - 2 - i);
            }
            putInt(wantedRPM);
            break;
        case 2:
            break;
        case 3:
            break;
    }
}

void resetCommand(uint8_t *command, unsigned int sizeOfArray) {
    for (int i = 0; i < sizeOfArray; i++) {
        command[i] = 0;
    }
}

unsigned int raiseToPower(unsigned int baseNumber, unsigned int expoentNumber) {
    unsigned int totalValue = 1;
    for (int i = 0; i < expoentNumber; i++) {
        totalValue *= baseNumber;
    }
    return totalValue;
}
