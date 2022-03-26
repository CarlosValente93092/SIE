/* ************************************************************************** */
/**

  @Authors
    Carlos Valente, NMec 93092
    Gonçalo Urbano, NMec 92949

  @File Name
    Timers.c

  @Summary
    Set of functions to initialise and control timers in PIC32MX795F512L using
 Digilent chipKIT MAx32 board

 */
/* ************************************************************************** */

#include <xc.h>
#include <stdint.h>
#include "Timers.h"

int getCloserPrescaler(uint64_t frequencyRatio, unsigned int prescalersList[], unsigned int numberOfPrescalers) {
    if (frequencyRatio >= 256) return TIMER_SETUP_ERROR;
    for (int i = 0; i < numberOfPrescalers; i++) {
        int distance = prescalersList[i] - frequencyRatio;
        if (distance < 0) continue;
        return i;
    }
}

int setupTimer1(unsigned int Fout, unsigned int useInterrupts) {
    unsigned int numberOfPrescalers = 4;
    unsigned int prescalersAvailable[] = {1, 8, 64, 256};

    uint64_t frequencyDivider = PBCLOCK / (INT16_MAX * Fout); //Get frequency divider constant
    int TCKPS = getCloserPrescaler(frequencyDivider, prescalersAvailable, numberOfPrescalers);
    if (TCKPS == TIMER_SETUP_ERROR) return TIMER_SETUP_ERROR; //Ocurred error
    unsigned int Fprescaler = PBCLOCK / prescalersAvailable[TCKPS];

    T1CONbits.TCKPS = TCKPS;
    PR1 = Fprescaler / Fout - 1; //Get value for PR1
    TMR1 = 0; // Reset timer T1 count register

    if (useInterrupts) {
        IPC1bits.T1IP = 2; // Interrupt priority (must be in range [1..6])
        IEC0bits.T1IE = 1; // Enable timer T1 interrupts
    }
    T1CONbits.TON = 1; // Enable timer T1

    return TIMER_SETUP_SUCCESS;
}

int setupTimer2(unsigned int Fout, unsigned int useInterrupts) {
    unsigned int numberOfPrescalers = 8;
    unsigned int prescalersAvailable[] = {1, 2, 4, 8, 16, 32, 64, 256};

    uint64_t frequencyDivider = PBCLOCK / (INT16_MAX * Fout); //Get frequency divider constant
    int TCKPS = getCloserPrescaler(frequencyDivider, prescalersAvailable, numberOfPrescalers);
    if (TCKPS == TIMER_SETUP_ERROR) return TIMER_SETUP_ERROR; //Ocurred error
    unsigned int Fprescaler = PBCLOCK / prescalersAvailable[TCKPS];

    T2CONbits.TCKPS = TCKPS;
    PR2 = Fprescaler / Fout - 1; //Get value for PR2
    TMR2 = 0; // Reset timer T2 count register

    if (useInterrupts) {
        IPC2bits.T2IP = 2; // Interrupt priority (must be in range [1..6])
        IEC0bits.T2IE = 1; // Enable timer T2 interrupts
    }
    T2CONbits.TON = 1; // Enable timer T2

    return TIMER_SETUP_SUCCESS;
}

int setupTimer3(unsigned int Fout, unsigned int useInterrupts) {
    unsigned int numberOfPrescalers = 8;
    unsigned int prescalersAvailable[] = {1, 2, 4, 8, 16, 32, 64, 256};

    uint64_t frequencyDivider = PBCLOCK / (INT16_MAX * Fout); //Get frequency divider constant
    int TCKPS = getCloserPrescaler(frequencyDivider, prescalersAvailable, numberOfPrescalers);
    if (TCKPS == TIMER_SETUP_ERROR) return TIMER_SETUP_ERROR; //Ocurred error
    unsigned int Fprescaler = PBCLOCK / prescalersAvailable[TCKPS];

    T3CONbits.TCKPS = TCKPS;
    PR3 = Fprescaler / Fout - 1; //Get value for PR3
    TMR3 = 0; // Reset timer T3 count register

    if (useInterrupts) {
        IPC3bits.T3IP = 2; // Interrupt priority (must be in range [1..6])
        IEC0bits.T3IE = 1; // Enable timer T3 interrupts
    }
    T3CONbits.TON = 1; // Enable timer T3


    return TIMER_SETUP_SUCCESS;
}

int setupTimer4(unsigned int Fout, unsigned int useInterrupts) {
    unsigned int numberOfPrescalers = 8;
    unsigned int prescalersAvailable[] = {1, 2, 4, 8, 16, 32, 64, 256};

    uint64_t frequencyDivider = PBCLOCK / (INT16_MAX * Fout); //Get frequency divider constant
    int TCKPS = getCloserPrescaler(frequencyDivider, prescalersAvailable, numberOfPrescalers);
    if (TCKPS == TIMER_SETUP_ERROR) return TIMER_SETUP_ERROR; //Ocurred error
    unsigned int Fprescaler = PBCLOCK / prescalersAvailable[TCKPS];

    T4CONbits.TCKPS = TCKPS;
    PR4 = Fprescaler / Fout - 1; //Get value for PR4
    TMR4 = 0; // Reset timer T4 count register

    if (useInterrupts) {
        IPC4bits.T4IP = 2; // Interrupt priority (must be in range [1..6])
        IEC0bits.T4IE = 1; // Enable timer T4 interrupts
    }
    T4CONbits.TON = 1; // Enable timer T4

    return TIMER_SETUP_SUCCESS;
}

int setupTimer5(unsigned int Fout, unsigned int useInterrupts) {
    unsigned int numberOfPrescalers = 8;
    unsigned int prescalersAvailable[] = {1, 2, 4, 8, 16, 32, 64, 256};

    uint64_t frequencyDivider = PBCLOCK / (INT16_MAX * Fout); //Get frequency divider constant
    int TCKPS = getCloserPrescaler(frequencyDivider, prescalersAvailable, numberOfPrescalers);
    if (TCKPS == TIMER_SETUP_ERROR) return TIMER_SETUP_ERROR; //Ocurred error
    unsigned int Fprescaler = PBCLOCK / prescalersAvailable[TCKPS];

    T5CONbits.TCKPS = TCKPS;
    PR5 = Fprescaler / Fout - 1; //Get value for PR5
    TMR5 = 0; // Reset timer T5 count register

    if (useInterrupts) {
        IPC5bits.T5IP = 2; // Interrupt priority (must be in range [1..6])
        IEC0bits.T5IE = 1; // Enable timer T5 interrupts
    }
    T5CONbits.TON = 1; // Enable timer T5

    return TIMER_SETUP_SUCCESS;
}

void waitTimer1() {
    while (IFS0bits.T1IF == 0);
}

void waitTimer2() {
    while (IFS0bits.T2IF == 0);
}

void waitTimer3() {
    while (IFS0bits.T3IF == 0);
}

void waitTimer4() {
    while (IFS0bits.T4IF == 0);
}

void waitTimer5() {
    while (IFS0bits.T5IF == 0);
}

void resetTimer1() {
    IFS0bits.T1IF = 0;
}

void resetTimer2() {
    IFS0bits.T2IF = 0;
}

void resetTimer3() {
    IFS0bits.T3IF = 0;
}

void resetTimer4() {
    IFS0bits.T4IF = 0;
}

void resetTimer5() {
    IFS0bits.T5IF = 0;
}