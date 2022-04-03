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

// MOTOR Variables
volatile unsigned int pulseCount;
volatile unsigned int rpm;
volatile unsigned int angularPosition;
volatile unsigned int rotation;

volatile unsigned int wantedRPM;
volatile unsigned int wantedRotation;
volatile unsigned int wantedAngularPosition;

//UART RX
volatile unsigned int RXFlag;
volatile uint8_t RXChar;

//Timer variables
volatile unsigned int countTimer5;
volatile unsigned int activeTimer5;
volatile unsigned int clearErrorMessage;

volatile unsigned int Fout4;
volatile unsigned int displayValues;

//Functions names
unsigned int validChar(uint8_t character);
void printMenuGeral(void);
void printMenu1(void);
void printMenu2(void);
void printMenu3(void);
unsigned int validateExecuteCommand(uint8_t *command, unsigned int sizeOfArray, unsigned int menu);
void resetCommand(uint8_t *command, unsigned int sizeOfArray);
unsigned int raiseToPower(unsigned int baseNumber, unsigned int expoentNumber);
unsigned int getRPM(void);
unsigned int getAngle();
unsigned int getRotation();
void printMenuDados(void);

//Interruptions

void __ISR(_TIMER_5_VECTOR, IPL7AUTO) T5Interrupt(void) { //IPL7 is the priority for the interrupt
    if (activeTimer5) {
        countTimer5++;
        if (countTimer5 == 10) {
            clearErrorMessage = 1;
            countTimer5 = 0;
        }
    }
    resetTimer5();
}

void __ISR(_TIMER_4_VECTOR, IPL5AUTO) T4Interrupt(void) { //IPL5 is the priority for the interrupt
    rpm = getRPM();
    angularPosition = getAngle();
    pulseCount = 0; //Reset pulse count
    if (displayValues) printMenuDados();
    resetTimer4();
}

void __ISR(_EXTERNAL_0_VECTOR, IPL3AUTO) ExtInt0ISR(void) { //IPL3 is the priority for the interrupt
    pulseCount = pulseCount + 4; //4 pulses everytime it detects a rising edge on channel A
    rotation = getRotation(); //get channel B imediately
    IFS0bits.INT0IF = 0; // Reset interrupt flag
}

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
    TRISCbits.TRISC2 = TRIS_IN; //Set port C2 to output, pin 22

    if (setupUART(115200, 1, 3) != SETUP_UART_SUCCESS) {
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }

    unsigned int Fout5 = 10; //Hz
    if (setupTimer5(Fout5, TIMER_USE_INTERRUPTS, 7) == TIMER_SETUP_ERROR) {
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }

    Fout4 = 10; //Hz
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
    if (setupPWM2(PWM_TIMER, 500, 6) == SETUP_PWM_ERROR) { //Pin 5, RD1
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
    uint8_t clearLine[] = "\r                                                  \r"; //50 space characters to clear line
    uint8_t command[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int index = 0;
    displayValues = 0;
    countTimer5 = 0;

    printMenuGeral();
    while (1) {
        if (clearErrorMessage) {
            putString(clearLine);
            putString(command);
            clearErrorMessage = 0;
            activeTimer5 = 0;
        }

        if (!RXFlag) continue;
        RXFlag = 0;
        displayValues = 0;

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
                if (index == 0) break;
                if (!validateExecuteCommand(command, index, MENU)) {
                    putString(clearLine);
                    putString("Error! Invalid input!");
                    activeTimer5 = 1;
                    continue;
                }
                if (!activeTimer5) displayValues = 1;
                resetCommand(command, index);
                index = 0;
                continue;
            case 27: //ESC
                if (MENU == 0) break;
                resetCommand(command, index);
                index = 0;
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

        if (MENU != 0 && validChar(RXChar)) { //If in any menu, listen to the input
            command[index] = RXChar;
            index++;
            putString(clearLine);
            putString(command);
        }
    }


    return (EXIT_SUCCESS);
}

unsigned int validChar(uint8_t character) {
    return (character > 31 && character < 127); //Can print spaces, letters, numbers, ponctuation
}

void printMenuGeral(void) {

    putString("\n\r\n\r\n\rSelect mode of operation:\n\r");
    putString("1. Set motor RPM\n\r");
    putString("2. Set angle for motor to turn\n\r");
    putString("3. Select from bundle pack\n\r");
}

void printMenu1(void) {

    putString("\n\rInsert rpm for motor (10-50) followed by direction (+,-) (e.g. 30+ or 45-)\n\r");
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

unsigned int validateExecuteCommand(uint8_t *command, unsigned int sizeOfArray, unsigned int menu) {
    if (menu == 1) {
        if (sizeOfArray > 3) return 0;
        if (sizeOfArray == 3) {
            if (command[0] == '+' || command[0] == '-' || command[0] == '0' || command[0] > '5')
                return 0; //First digit must be a number between 1 and 5
            if (command[1] == '+' || command[1] == '-' || (command[1] > '1' && command[0] == '5'))
                return 0; //Second digit must be a number but 50 is the limit
            if (command[2] != '+' && command[2] != '-')
                return 0; //Third digit must be a sign

            wantedRPM = 0;
            wantedRotation = 0;
            wantedRPM += (command[0] - 48) * 10 + command[1];
            if (command[2] == '+') wantedRotation = ROTATING_TO_RIGHT;
            if (command[2] == '-') wantedRotation = ROTATING_TO_LEFT;
            return 1; //All good
        }
        if (sizeOfArray == 1)
            if (command[0] == '0') {
                changeDutyCycle2(0, 500); //DutyCycle of 50% to stop motor
                putString("\rStopping Motor!");
                activeTimer5 = 1;
                return 1;
            }
        return 0; //Command was only 1 character long
    }

    if (menu == 2) {
        if (command[0] != '+' && command[0] != '-' && (command[0] < '0' || command[0] > '9')) return 0;

        unsigned int angleStart = 0;
        wantedAngularPosition = 0;
        if (command[0] == '+') {
            angleStart = 1;
            wantedRotation = ROTATING_TO_RIGHT;
        }
        if (command[0] == '-') {
            angleStart = 1;
            wantedRotation = ROTATING_TO_LEFT;
        }
        if (command[0] >= '0' && command[0] <= '9') wantedRotation = ROTATING_TO_RIGHT; //No sign is positive

        for (int i = 0; i < sizeOfArray - angleStart; i++) {
            if (command[i + angleStart] < '0' || command[i + angleStart] > '9') return 0; //Not a number
            wantedAngularPosition += (command[i + angleStart] - 48) * raiseToPower(10, sizeOfArray - i - angleStart - 1);
        }
        return 1; //All good
    }

    if (menu == 3) {
        if (sizeOfArray > 1) return 0;
        if (command[0] == '0') return 0;
        if (command[0] > '3') return 0;

        if (command[0] == '1') {
            putString("\r1. Perform 180 degree turn clockwise");
            activeTimer5 = 1;
        }
        if (command[0] == '2') {
            putString("\r2. Increase from 10 rpm to 50 rpm");
            activeTimer5 = 1;
        }
        if (command[0] == '3') {
            putString("\r3. Decrease from 50 rpm to 10 rpm");
            activeTimer5 = 1;
        }
        return 1; //All good
    }

    return 0; //Something when wrong
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

unsigned int getRPM(void) { // em rpm
    // 180 pulses is 1 rotation therefore, rotation is pulseCount/180
    // This value is for 1/Fout4 seconds so, in 1 minute we have Fout4*60*pulseCount/180
    // A sistematic error in the number of pulses per rotation was found, we fixed it to 1620 = 180*9
    return (Fout4 * 60 * pulseCount) / 1620;
}

unsigned int getAngle() {
    //2 degrees per pulse
    return (angularPosition + (pulseCount * 2) / 9) % 360;
}

unsigned int getRotation() {
    //If 0, counter clockwise
    //if 1, clockwise
    return PORTCbits.RC2;
}

void printMenuDados(void) {
    uint8_t clearLine[] = "\r                                                  \r"; //50 space characters to clear line

    putString(clearLine);
    putString("RPM: ");
    if (rpm < 10) {
        putInt(0);
    }
    putInt(rpm);
    putString(", Angle:");
    if (angularPosition < 100) {
        putInt(0);
    }
    if (angularPosition < 10) {
        putInt(0);
    }
    putInt(angularPosition);
    putString(", Direction: ");
    if (rotation == ROTATING_TO_LEFT) {
        putString("Counter Clockwise");
    } else if (rotation == ROTATING_TO_RIGHT) {
        putString("Clockwise");
    }
}
