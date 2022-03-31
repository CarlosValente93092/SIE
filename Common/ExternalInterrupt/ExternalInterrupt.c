/* ************************************************************************** */
/**

  @Authors
    Carlos Valente, NMec 93092
    Gonçalo Urbano, NMec 92949

  @File Name
    ExternalInterrupt.c

  @Summary
    Set of functions to use External Interrupts in PIC32MX795F512L
    Ext0 uses pin 3, RD0
    Ext1 uses pin 2, RE8
    Ext2 uses pin 7, RE9
    Ext3 uses pin 21, RA14
    Ext4 uses pin 20, RA15

 */

/* ************************************************************************** */

#include <xc.h>
#include "ExternalInterrupt.h"

int setuptExt0(unsigned int Edge, unsigned int interruptPriority) {
    if (Edge > 1) return SETUP_EXTERNAL_ERROR;
    if (interruptPriority > 7) return SETUP_EXTERNAL_ERROR;

    TRISDbits.TRISD0 = 1; // Set pin 3 as input
    INTCONbits.INT0EP = Edge; // Generate interrupts on rising edge or falling edge
    IFS0bits.INT0IF = 0; // Reset interrupt flag
    IPC0bits.INT0IP = interruptPriority; // Set interrupt priority
    IEC0bits.INT0IE = 1; // Enable Int0 interrupts

    return SETUP_EXTERNAL_SUCCESS;
}

int setuptExt1(unsigned int Edge, unsigned int interruptPriority) {
    if (Edge > 1) return SETUP_EXTERNAL_ERROR;
    if (interruptPriority > 7) return SETUP_EXTERNAL_ERROR;

    TRISEbits.TRISE8 = 1; // Set pin 2 as input
    INTCONbits.INT1EP = Edge; // Generate interrupts on rising edge or falling edge
    IFS0bits.INT1IF = 0; // Reset interrupt flag
    IPC1bits.INT1IP = interruptPriority; // Set interrupt priority
    IEC0bits.INT1IE = 1; // Enable Int1 interrupts

    return SETUP_EXTERNAL_SUCCESS;
}

int setuptExt2(unsigned int Edge, unsigned int interruptPriority) {
    if (Edge > 1) return SETUP_EXTERNAL_ERROR;
    if (interruptPriority > 7) return SETUP_EXTERNAL_ERROR;

    TRISEbits.TRISE9 = 1; // Set pin 7 as input
    INTCONbits.INT2EP = Edge; // Generate interrupts on rising edge or falling edge
    IFS0bits.INT2IF = 0; // Reset interrupt flag
    IPC2bits.INT2IP = interruptPriority; // Set interrupt priority
    IEC0bits.INT2IE = 1; // Enable Int2 interrupts

    return SETUP_EXTERNAL_SUCCESS;
}

int setuptExt3(unsigned int Edge, unsigned int interruptPriority) {
    if (Edge > 1) return SETUP_EXTERNAL_ERROR;
    if (interruptPriority > 7) return SETUP_EXTERNAL_ERROR;

    TRISAbits.TRISA14 = 1; // Set pin 21 as input
    INTCONbits.INT3EP = Edge; // Generate interrupts on rising edge or falling edge
    IFS0bits.INT3IF = 0; // Reset interrupt flag
    IPC3bits.INT3IP = interruptPriority; // Set interrupt priority
    IEC0bits.INT3IE = 1; // Enable Int3 interrupts

    return SETUP_EXTERNAL_SUCCESS;
}

int setuptExt4(unsigned int Edge, unsigned int interruptPriority) {
    if (Edge > 1) return SETUP_EXTERNAL_ERROR;
    if (interruptPriority > 7) return SETUP_EXTERNAL_ERROR;

    TRISAbits.TRISA15 = 1; // Set pin 20 as input
    INTCONbits.INT4EP = Edge; // Generate interrupts on rising edge or falling edge
    IFS0bits.INT4IF = 0; // Reset interrupt flag
    IPC4bits.INT4IP = interruptPriority; // Set interrupt priority
    IEC0bits.INT4IE = 1; // Enable Int4 interrupts

    return SETUP_EXTERNAL_SUCCESS;
}

void resetExt0() {
    IFS0bits.INT0IF = 0; // Reset interrupt flag
}

void resetExt1() {
    IFS0bits.INT1IF = 0; // Reset interrupt flag
}

void resetExt2() {
    IFS0bits.INT2IF = 0; // Reset interrupt flag
}

void resetExt3() {
    IFS0bits.INT3IF = 0; // Reset interrupt flag
}

void resetExt4() {
    IFS0bits.INT4IF = 0; // Reset interrupt flag
}

