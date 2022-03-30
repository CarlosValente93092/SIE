/* ************************************************************************** */
/**

  @Authors
    Carlos Valente, NMec 93092
    Gonçalo Urbano, NMec 92949

  @File Name
    PWM.c

  @Summary
    Set of functions to use PWM in PIC32MX795F512L
    Resolution is given as log2(Fprescaler / Fout)
    PWM with a resolution of 500 steps (0.2 between each value)
    Pins:
        PW1, OC1, pin 3,  RE8
        PW2, OC2, pin 5,  RD1
        PW3, OC3, pin 6,  RD2
        PW4, OC4, pin 9,  RD3
        PW5, OC5, pin 10, RD4
    chooseTimer:
        0 for Timer2
        1 for Timer3
    mode:
        111 = PWM mode on OCx; Fault pin enabled
        110 = PWM mode on OCx; Fault pin disabled
        101 = Initialize OCx pin low; generate continuous output pulses on OCx pin
        100 = Initialize OCx pin low; generate single output pulse on OCx pin
        011 = Compare event toggles OCx pin
        010 = Initialize OCx pin high; compare event forces OCx pin low
        001 = Initialize OCx pin low; compare event forces OCx pin high
        000 = Output compare peripheral is disabled but continues to draw current

 */

/* ************************************************************************** */

#include <xc.h>
#include "PWM.h"

int getOCXRS(unsigned int chooseTimer, unsigned int dutyCycle) {
    if (chooseTimer) {

        return (PR3 * dutyCycle) / PWM_MAX_VALUE; //Timer 3
    }

    return (PR2 * dutyCycle) / PWM_MAX_VALUE; //Timer 2
}

int setupPWM1(unsigned int chooseTimer, unsigned int dutyCycle, unsigned int mode) {
    if (chooseTimer > 1) return PWM_SETUP_ERROR;
    if (dutyCycle > PWM_MAX_VALUE) return PWM_SETUP_ERROR;
    if (mode > 7) return PWM_SETUP_ERROR;

    OC1CONbits.OCM = mode; // PWM mode on OCx; fault pin disabled
    OC1CONbits.OCTSEL = chooseTimer; // Use timer T2 (0) or T3 (1) as the time base for PWM generation
    OC1RS = getOCXRS(chooseTimer, dutyCycle);
    OC1CONbits.ON = 1; // Enable OC1 module

    return PWM_SETUP_SUCCESS;
}

int setupPWM2(unsigned int chooseTimer, unsigned int dutyCycle, unsigned int mode) {
    if (chooseTimer > 2) return PWM_SETUP_ERROR;
    if (dutyCycle > PWM_MAX_VALUE) return PWM_SETUP_ERROR;
    if (mode > 7) return PWM_SETUP_ERROR;

    OC2CONbits.OCM = mode; // PWM mode on OCx; fault pin disabled
    OC2CONbits.OCTSEL = chooseTimer; // Use timer T2 (0) or T3 (1) as the time base for PWM generation
    OC2RS = getOCXRS(chooseTimer, dutyCycle);
    OC2CONbits.ON = 1; // Enable OC1 module

    return PWM_SETUP_SUCCESS;
}

int setupPWM3(unsigned int chooseTimer, unsigned int dutyCycle, unsigned int mode) {
    if (chooseTimer > 2) return PWM_SETUP_ERROR;
    if (dutyCycle > PWM_MAX_VALUE) return PWM_SETUP_ERROR;
    if (mode > 7) return PWM_SETUP_ERROR;

    OC3CONbits.OCM = mode; // PWM mode on OCx; fault pin disabled
    OC3CONbits.OCTSEL = chooseTimer; // Use timer T2 (0) or T3 (1) as the time base for PWM generation
    OC3RS = getOCXRS(chooseTimer, dutyCycle);
    OC3CONbits.ON = 1; // Enable OC1 module

    return PWM_SETUP_SUCCESS;
}

int setupPWM4(unsigned int chooseTimer, unsigned int dutyCycle, unsigned int mode) {
    if (chooseTimer > 2) return PWM_SETUP_ERROR;
    if (dutyCycle > PWM_MAX_VALUE) return PWM_SETUP_ERROR;
    if (mode > 7) return PWM_SETUP_ERROR;

    OC4CONbits.OCM = mode; // PWM mode on OCx; fault pin disabled
    OC4CONbits.OCTSEL = chooseTimer; // Use timer T2 (0) or T3 (1) as the time base for PWM generation
    OC4RS = getOCXRS(chooseTimer, dutyCycle);
    OC4CONbits.ON = 1; // Enable OC1 module

    return PWM_SETUP_SUCCESS;
}

int setupPWM5(unsigned int chooseTimer, unsigned int dutyCycle, unsigned int mode) {
    if (chooseTimer > 2) return PWM_SETUP_ERROR;
    if (dutyCycle > PWM_MAX_VALUE) return PWM_SETUP_ERROR;
    if (mode > 7) return PWM_SETUP_ERROR;

    OC5CONbits.OCM = mode; // PWM mode on OCx; fault pin disabled
    OC5CONbits.OCTSEL = chooseTimer; // Use timer T2 (0) or T3 (1) as the time base for PWM generation
    OC5RS = getOCXRS(chooseTimer, dutyCycle);
    OC5CONbits.ON = 1; // Enable OC1 module

    return PWM_SETUP_SUCCESS;
}

void changeDutyCycle1(unsigned int chooseTimer, unsigned int dutyCycle) {
    OC1RS = getOCXRS(chooseTimer, dutyCycle);
}

void changeDutyCycle2(unsigned int chooseTimer, unsigned int dutyCycle) {
    OC2RS = getOCXRS(chooseTimer, dutyCycle);
}

void changeDutyCycle3(unsigned int chooseTimer, unsigned int dutyCycle) {
    OC3RS = getOCXRS(chooseTimer, dutyCycle);
}

void changeDutyCycle4(unsigned int chooseTimer, unsigned int dutyCycle) {
    OC4RS = getOCXRS(chooseTimer, dutyCycle);
}

void changeDutyCycle5(unsigned int chooseTimer, unsigned int dutyCycle) {
    OC5RS = getOCXRS(chooseTimer, dutyCycle);
}