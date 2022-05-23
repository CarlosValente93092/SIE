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
#include "../Common/ADC/ADC.h"

#define TRIS_OUT    0       //Define TRIS as output
#define TRIS_IN     1       //Define TRIS as input

#define USE_TIMER_2 0
#define USE_TIMER_3 1

#define arrayLength 200

// Variables needed to make report graphs
volatile int temperatures[arrayLength];
volatile int tempControl[arrayLength];
volatile int setpoint[arrayLength];
volatile int tempIndex;
volatile int arrayCounter;
volatile int arrayActive;
volatile float uk;

// Temperature Variables
volatile float wantedTemperature;
volatile float temperature;
volatile float maxTemperature;
volatile float envTemperature;
volatile float thermocoupleTemperature;

volatile int thermistorVoltage;
volatile int thermocoupleVoltage;

// Variables for display and duty cycle changes
volatile unsigned int showDutyCycle;
volatile unsigned int showEnvTemp;
volatile unsigned int showAllTemp;
volatile unsigned int showThermocoupleTemp;
volatile unsigned int countTemp;

// Variables for UART RX
volatile unsigned int RXFlag;
volatile uint8_t RXChar;

//Timer variables for display of error messages
volatile unsigned int countTimer5;
volatile unsigned int activeTimer5;
volatile unsigned int clearErrorMessage;

//Conolador PI
volatile float integralPartPI; //Integral part of PI
volatile float errorPI;
volatile float dutyCycle;

//Functions names
unsigned int validChar(uint8_t character); //Validates if it is character or not
void initVariables(void); //Initialize variables at startup

void printMenuGeral(void); //Print main menu
void printMenu1(void); //Print menu 1, main project code
void printMenu2(void); //Print menu 2, display environment temperature
void printMenu3(void); //Print menu 3, show thermocouple temperature
void printMenu4(void); //Print menu 4, change duty cycle
void printEnvTemp(void); //Print environmental temperature (for menu 2)
void printAllTemp(void); //print all temperatures (for menu 1)
void printThermocoupleTemp(void); //print thermocouple temperature (read only)
void printDutyCycle(void); //Prints actual duty cycle (menu4)

unsigned int validateExecuteCommand(uint8_t *command, unsigned int sizeOfArray, unsigned int menu); //Validates command
void resetCommand(uint8_t *command, unsigned int sizeOfArray); //resets command

void newEnvTemp(void); //calculate environmental temperature based on a linearization
void newTemp(void); //calculate environmental temperature based on a linearization

void controladorPI(void); //Controlador Proporcional integrador
void putIntArray(void); //Prints all array info, for demonstration only

//Interruptions
//Interrupt for error messages using timer 5

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

//interrupt to have ADC be sampling at constant rate

void __ISR(_TIMER_1_VECTOR, IPL5AUTO) T1Interrupt(void) { //IPL5 is the priority for the interrupt
    startADC();
    if (showDutyCycle) printDutyCycle();
    resetTimer1();
}

//Interrupt for UART receiver

void __ISR(_UART1_VECTOR, IPL3AUTO) UART1ISR(void) {
    if (IFS0bits.U1RXIF) {
        RXFlag = 1;
        RXChar = U1RXREG;
        IFS0bits.U1RXIF = 0; //reset UART1 RX interrupt flag
    }
}

//Interrupt that occurs when ADC finishes converting all 8 samples

void __ISR(_ADC_VECTOR, IPL2AUTO) ADCISR(void) {
    thermocoupleVoltage += getValues2ADCA();
    thermistorVoltage += getValues2ADCB();
    countTemp++;
    int maxCount = 10;
    if (countTemp == maxCount) { //Every 10 samples do the mean, helps in a more controlled measurement
        countTemp = 0;
        thermocoupleVoltage /= maxCount;
        thermistorVoltage /= maxCount;
        newEnvTemp();
        newTemp();

        if (showEnvTemp) printEnvTemp();
        if (showThermocoupleTemp) printThermocoupleTemp();
        if (showAllTemp) {
            printAllTemp();
            controladorPI();
        }
    }

    arrayCounter++;
    if (arrayCounter > 4 * 100 && arrayActive) { //Every 4 seconds we take a sample and put it in the buffers
        temperatures[tempIndex] = temperature;
        tempControl[tempIndex] = (int) uk;
        setpoint[tempIndex] = wantedTemperature;
        tempIndex++;
        arrayCounter = 0;
        if (tempIndex == arrayLength) { //Once it reaches its maximum value, stops adding to buffers
            arrayActive = 0;
            PORTAbits.RA3 = 0; //Visual indication
        }
    }
    resetADC(); //Reset ADC
}

//Main code

int main(void) {
    __XC_UART = 1; //Send input/output/errors to UART 1
    INTCONSET = _INTCON_MVEC_MASK; // Multi vector mode for interrupts

    TRISAbits.TRISA3 = TRIS_OUT; //Set port A3 to output, led4

    if (setupUART(115200, 1, 3) != SETUP_UART_SUCCESS) {
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }

    unsigned int Fout5 = 10; //Hz
    if (setupTimer5(Fout5, TIMER_USE_INTERRUPTS, 7) == TIMER_SETUP_ERROR) {
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }

    unsigned int Fout1 = 100; //Hz, sampling frequency
    if (setupTimer1(Fout1, TIMER_USE_INTERRUPTS, 5) == TIMER_SETUP_ERROR) {
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }

    unsigned int Fout2 = 20000; //Hz, pwm frequency
    if (setupTimer2(Fout2, TIMER_USE_POLLING, 0) == TIMER_SETUP_ERROR) {
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }

    unsigned int PWM_TIMER = USE_TIMER_2;
    ODCDbits.ODCD1 = 1; //Put pin to be open-drain
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
    uint8_t clearLine[] = "\r                                                  \r"; //50 space characters to clear line
    uint8_t command[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Buffer for characters to command
    unsigned int index = 0; //defines the position inside command buffer


    while (1) {
        if (clearErrorMessage) { //Clear error message
            putString(clearLine);
            putString(command);
            clearErrorMessage = 0;
            activeTimer5 = 0;
        }

        if (!RXFlag) continue; //If no key pressed then continue
        RXFlag = 0; //reset flag for UART RX

        switch (RXChar) {
            case 'r': //reset max temperature
                maxTemperature = 0;
                continue;
            case 'p': //prints all array values
                putIntArray();
                continue;
            case 't': //prints both sensors voltages
                putString("\n\r\n\r");
                putVoltage100(thermocoupleVoltage);
                putString("\n\r");
                putVoltage100(thermistorVoltage);
                putString("\n\r\n\r");
                continue;
            case '+': //Increase duty cycle (menu3)
                if (MENU != 4) break;
                dutyCycle += 5;
                if (dutyCycle > 95) dutyCycle = 95;
                changeDutyCycle2(0, dutyCycle);
                continue;
            case '-': //Decrease duty cycle (menu3)
                if (MENU != 4) break;
                dutyCycle -= 5;
                if (dutyCycle < 5) dutyCycle = 5;
                changeDutyCycle2(0, dutyCycle);
                continue;
            case '1': //Choose menu 1 (menu0)
                if (MENU != 0) break;
                printMenu1();
                MENU = 1;
                maxTemperature = 0;
                continue;
            case '2': //Choose menu 2 (menu0)
                if (MENU != 0) break;
                printMenu2();
                MENU = 2;
                showEnvTemp = 1;
                continue;
            case '3': //Choose menu 3 (menu0)
                if (MENU != 0) break;
                printMenu3();
                MENU = 3;
                showThermocoupleTemp = 1;
                continue;
            case '4': //Choose menu 4 (menu0)
                if (MENU != 0) break;
                printMenu4();
                MENU = 4;
                showDutyCycle = 1;
                continue;
            case 13: //ENTER, used when entering command for menu 1
                if (MENU != 1) break;
                if (index == 0) break;
                if (!validateExecuteCommand(command, index, MENU)) {
                    putString(clearLine);
                    putString("Error! Invalid input!");
                    activeTimer5 = 1;
                    continue;
                }
                arrayActive = 1;
                resetCommand(command, index);
                index = 0;
                continue;
            case 27: //ESC, leave current menu
                if (MENU == 0) break;
                resetCommand(command, index);
                index = 0;
                changeDutyCycle2(0, 0);
                MENU = 0;
                initVariables();
                printMenuGeral();
                continue;
            case 127: //BACKSPACE, clear command
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

        if (MENU == 1 && validChar(RXChar)) { //If in menu 1, listen to the input
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
    countTimer5 = 0;
    dutyCycle = 0; //starting with duty cycle at 0% to not start heating resistor
    showDutyCycle = 0;
    showEnvTemp = 0;
    showAllTemp = 0;
    clearErrorMessage = 0;
    activeTimer5 = 0;
    RXFlag = 0;
    showThermocoupleTemp = 0;
    maxTemperature = 0;
    integralPartPI = 0; //Integral part of PI
    errorPI = 0;
    wantedTemperature = 0;
    countTemp = 0;
    arrayCounter = 0;
    tempIndex = 0;
    arrayActive = 0;
    uk = 0;
}

void printMenuGeral(void) {
    putString("\n\r\n\rSelect mode of operation:\n\r");
    putString("1. Set temperature\n\r");
    putString("2. Get environmental temperature\n\r");
    putString("3. Get thermocouple temperature\n\r");
    putString("4. Change duty cycle\n\r");
}

void printMenu1(void) {
    putString("\n\rInsert temperature (40C to 100C)\n\r");
    putString("(Press ESC to go back to main menu)\n\r");
}

void printMenu2(void) {
    putString("\n\rDisplaying environmental temperature\n\r");
    putString("(Press ESC to go back to main menu)\n\r");
}

void printMenu3(void) {
    putString("\n\rPress {+,-} to increase duty cycle:\n\r");
    putString("(Press ESC to go back to main menu)\n\r");
}

void printMenu4(void) {
    putString("\n\rPress {+,-} to increase duty cycle:\n\r");
    putString("(Press ESC to go back to main menu)\n\r");
}

unsigned int validateExecuteCommand(uint8_t *command, unsigned int sizeOfArray, unsigned int menu) {
    if (menu == 1) {
        if (sizeOfArray > 4) return 0;
        if (sizeOfArray == 3) {
            if (command[0] != '1' && command[1] != '0' && command[2] != '0') return 0; //3 digits is only for 100ºC

            wantedTemperature = 100;
            showAllTemp = 1;
            return 1; //All good
        }
        if (sizeOfArray == 2) {
            if (command[0] < '4' || command[0] > '9') return 0; //First digit must be a number between 4 and 9
            if (command[1] < '0' || command[0] > '9') return 0; //Second digit must be a number

            wantedTemperature = (command[0] - 48)*10 + (command[1] - 48);
            showAllTemp = 1;
            return 1; //All good
        }
        if (sizeOfArray == 1) {
            if (command[0] == '0') {
                changeDutyCycle2(USE_TIMER_2, 0);
                wantedTemperature = 0;
                showAllTemp = 1;
                return 1; //All good
            }
        }
        return 0; //Something went wrong
    }
    return 0; //Something went wrong
}

void resetCommand(uint8_t *command, unsigned int sizeOfArray) {
    for (int i = 0; i < sizeOfArray; i++) {
        command[i] = 0;
    }
}

void printDutyCycle(void) {

    uint8_t clearLine[] = "\r                    \r"; //20 space characters to clear line
    putString(clearLine);
    putString("Duty Cycle = ");
    putInt(dutyCycle);
    putString("%");
}

void printEnvTemp(void) {
    putString("\r               \r"); //15 empty spots to clear line
    putString("EnvTemp: ");
    putInt((int) (envTemperature));
    putString("ºC");
}

void printAllTemp(void) {
    putString("\r                                   \r"); //35 space characters to clear line)
    putString("Temp: ");
    putInt((int) (temperature));
    putString("C, Max: ");
    putInt((int) (maxTemperature));
    putString("C, Env: ");
    putInt((int) (envTemperature));
    putString("ºC");
}

void printThermocoupleTemp(void) {
    putString("\r                         \r"); //25 empty spots to clear line
    putString("Thermocouple Temp: ");
    putInt((int) (thermocoupleTemperature));
    putString("ºC");
}

void newEnvTemp() {
    envTemperature = (float) (thermistorVoltage) / 100 * 8.8045 + 10.01;
}

void newTemp() {
    thermocoupleTemperature = (int) ((float) (thermocoupleVoltage) / 100 * 30.359 + 0.6212);
    temperature = thermocoupleTemperature + envTemperature;
    if (temperature > maxTemperature) maxTemperature = temperature;
}

void controladorPI() {
    float Kp = 1;
    float h = 1;
    float Ti = 5;

    //Professor Pedro Fonseca file in sweet.ua.pt website
    float error = wantedTemperature - temperature;
    integralPartPI = integralPartPI / 4 + (Kp * h * error) / Ti;
    uk = (Kp * error) + integralPartPI;
    dutyCycle += uk;

    if (dutyCycle < 5) dutyCycle = 5;
    if (dutyCycle > 95) dutyCycle = 95;
    changeDutyCycle2(0, (int) (dutyCycle));
}

void putIntArray() {
    for (int i = 0; i < arrayLength; i++) {
        putInt(temperatures[i]);
        putchar(' ');
    }

    putString("\n\r\n\r");

    for (int i = 0; i < arrayLength; i++) {
        putInt(tempControl[i]);
        putchar(' ');
    }

    putString("\n\r\n\r");

    for (int i = 0; i < arrayLength; i++) {
        putInt(setpoint[i]);
        putchar(' ');
    }
}