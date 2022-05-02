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

#define ROTATING_TO_LEFT 1
#define ROTATING_TO_RIGHT 0

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

//Controller variables
volatile int dutyCycle;
volatile int integralPartPI; //Integral part of PI
volatile int errorPI;
volatile int controlSignalPI;
volatile unsigned int startingRPM;
volatile unsigned int revolutions;

volatile unsigned int showDutyCycle;

//Functions names
unsigned int validChar(uint8_t character);
void initVariables(void);

void printMenuGeral(void);
void printMenu1(void);
void printMenu2(void);
void printMenu3(void);
void printMenu4(void);
void printMenuDados(void);
void printDutyCycle(void);

unsigned int validateExecuteCommand(uint8_t *command, unsigned int sizeOfArray, unsigned int menu);
void resetCommand(uint8_t *command, unsigned int sizeOfArray);
unsigned int raiseToPower(unsigned int baseNumber, unsigned int expoentNumber);

unsigned int getRPM(void);
unsigned int getAngle();
unsigned int getRotation();

void controladorPI(unsigned int option);
void controladorOnOff(void);
void controladorAngularPosition(void);

void stopAtFixedAngle(void);
void stopMotor(void);



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
    if (showDutyCycle) printDutyCycle();
    if (wantedAngularPosition != 0) controladorAngularPosition();
    //    if (wantedRPM != 0) controladorOnOff();
    if (wantedRPM != 0) controladorPI(2);
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
    initVariables();

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
                showDutyCycle = 1;
                continue;
            case '4':
                if (MENU != 0) break;
                printMenu4();
                MENU = 4;
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
                showDutyCycle = 0;
                wantedAngularPosition = 0;
                wantedRPM = 0;
                wantedRotation = 0;
                dutyCycle = 500;
                changeDutyCycle2(0, dutyCycle);
                printMenuGeral();
                angularPosition = 0;
                integralPartPI = 0;
                errorPI = 0;
                controlSignalPI = 0;
                MENU = 0;
                revolutions = 0;
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

void initVariables(void) {
    showDutyCycle = 0;
    displayValues = 0;
    countTimer5 = 0;
    errorPI = 0;
    controlSignalPI = 0;
    integralPartPI = 0;
    dutyCycle = 500;
    angularPosition = 0;
    startingRPM = 0;
    revolutions = 0;
}

void printMenuGeral(void) {

    putString("\n\r\n\r\n\rSelect mode of operation:\n\r");
    putString("1. Set motor RPM\n\r");
    putString("2. Set angle for motor to turn\n\r");
    putString("3. Control speed and direction of motor\n\r");
    putString("4. Select duty cycle\n\r");
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
    putString("\n\rPress {+,-} to increase speed to {right,left}:\n\r");
    putString("(Press ESC to go back to main menu)\n\r");
}

void printMenu4(void) {
    putString("\n\rInsert Duty Cycle (0-100)%\n\r");
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
            wantedRPM += (command[0] - 48) * 10 + (command[1] - 48);
            if (command[2] == '+') wantedRotation = ROTATING_TO_RIGHT;
            if (command[2] == '-') wantedRotation = ROTATING_TO_LEFT;


            startingRPM = 1;
            return 1; //All good
        }
        if (sizeOfArray == 1) {
            if (command[0] == '0') {
                stopMotor();
                activeTimer5 = 1;
                return 1;
            }

        }
        return 0; //Command was only 1 character long
    }

    if (menu == 2) {
        if (command[0] != '+' && command[0] != '-' && (command[0] < '0' || command[0] > '9')) return 0;

        unsigned int angleStart = 0;
        wantedAngularPosition = 0;
        revolutions = 0;
        angularPosition = 0;
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

    if (menu == 4) {
        dutyCycle = 0;
        if (sizeOfArray > 3) return 0;
        for (int i = 0; i < sizeOfArray; i++) {
            if (command[i] < '0' || command[i] > '9') return 0; //Not a number
            dutyCycle += (command[i] - 48) * raiseToPower(10, sizeOfArray - i - 1);
        }
        dutyCycle *= 10;
        changeDutyCycle2(0, dutyCycle);
        return 1; //All good
    }

    return 0; //Something when wrong
}

void stopMotor(void) {
    wantedRPM = 0;
    changeDutyCycle2(0, 500); //DutyCycle of 50% to stop motor
    putString("\rStopping Motor!");
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
    if ((angularPosition + (pulseCount * 2) / 9) >= 360)
        revolutions++;
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
    if (rpm < 10) putInt(0);

    putInt(rpm);
    putString(", Angle:");
    if (angularPosition < 100) putInt(0);

    if (angularPosition < 10) putInt(0);

    putInt(angularPosition);
    putString(", Direction: ");
    if (rotation == ROTATING_TO_LEFT) {
        putString("Counter Clockwise");
    } else if (rotation == ROTATING_TO_RIGHT) {
        putString("Clockwise");
    }
}

void controladorPI(unsigned int option) {
    unsigned int Kp = 5;
    unsigned int h = 1;
    unsigned int Ti = 5;

    if (wantedRotation != rotation && startingRPM) {
        if (dutyCycle > 490 && dutyCycle < 510) {
            startingRPM = 0;
        } else {
            if (500 - dutyCycle < 0) dutyCycle -= 10;
            else dutyCycle += 10;
            changeDutyCycle2(0, dutyCycle);
        }
    } else {
        int dutyCycleMotorDirection = 1;
        if (wantedRotation == ROTATING_TO_LEFT) dutyCycleMotorDirection = -dutyCycleMotorDirection;
        if (option == 1) {
            //Professor Pedro Fonseca file in sweet.ua.pt website
            int error = wantedRPM - rpm;
            //        integralPartPI = integralPartPI + (Kp * h * error) / Ti;
            integralPartPI = integralPartPI + (Kp * h * error) / Ti;
            int uk = (Kp * error) + integralPartPI;
            dutyCycle += (uk * dutyCycleMotorDirection);

            if (dutyCycle < 0) dutyCycle = 0;
            if (dutyCycle > 1000) dutyCycle = 1000;
            changeDutyCycle2(0, dutyCycle);
        }

        if (option == 2) {
            //PI controller given by SSD notes
            int s0 = Kp + (Kp * h) / Ti; // s0(1) = 6
            int s1 = -Kp; // s1(1) = -5
            int prevError = errorPI;
            errorPI = wantedRPM - rpm;
            controlSignalPI = controlSignalPI + s0 * errorPI + s1*prevError;

            dutyCycle += controlSignalPI*dutyCycleMotorDirection;
            if (dutyCycle < 0) dutyCycle = 0;
            if (dutyCycle > 1000) dutyCycle = 1000;
            changeDutyCycle2(0, dutyCycle);
        }
    }
}

void controladorOnOff(void) {
    // wantedRPM, rpm, rotation, dutyCycle
    int U = 0;
    int error = wantedRPM - rpm;
    int dutyCycleMotorDirection = 10; //Increase dutyCycle when rotating right
    if (wantedRotation == ROTATING_TO_LEFT) dutyCycleMotorDirection = -dutyCycleMotorDirection; //Decrease dutyCycle when rotating left

    if (error == 0)
        U = 0;
    else if (error > 0)
        U = dutyCycleMotorDirection;
    else
        U = -dutyCycleMotorDirection;

    dutyCycle = dutyCycle + U;
    changeDutyCycle2(0, dutyCycle);
}

void printDutyCycle(void) {
    uint8_t clearLine[] = "\r                    \r"; //50 space characters to clear line

    putString(clearLine);
    putString("Duty Cycle = ");
    putInt(dutyCycle / 10);
    putString("%");
}

void controladorAngularPosition() {
    int error = wantedAngularPosition - (angularPosition + 360 * revolutions);
    if (error < 20)
        changeDutyCycle2(0, 500);
    else {
        int rotate = 1; //Increase dutyCycle when rotating right
        if (wantedRotation == ROTATING_TO_LEFT) rotate = -1; //Decrease dutyCycle when rotating left
        changeDutyCycle2(0, 500 + 200 * rotate);
    }
}
