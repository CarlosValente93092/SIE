/* ************************************************************************** */
/**

  @Authors
    Carlos Valente, NMec 93092
    Gonçalo Urbano, NMec 92949

  @File Name
    ExternalInterrupt.h

  @Summary
    Set of functions to use External Interrupts in PIC32MX795F512L
    Ext0 uses pin 3, RD0
    Ext1 uses pin 2, RE8
    Ext2 uses pin 7, RE9
    Ext3 uses pin 21, RA14
    Ext4 uses pin 20, RA15

 */

/* ************************************************************************** */

#ifndef _EXTERNAL_INTERRUPTS_PIC32MX_H
#define _EXTERNAL_INTERRUPTS_PIC32MX_H

#define EXTERNAL_FALLING_EDGE 0
#define EXTERNAL_RISING_EDGE 1

#define SETUP_EXTERNAL_SUCCESS 0
#define SETUP_EXTERNAL_ERROR -1

int setuptExt0(unsigned int Edge, unsigned int interruptPriority);
int setuptExt1(unsigned int Edge, unsigned int interruptPriority);
int setuptExt2(unsigned int Edge, unsigned int interruptPriority);
int setuptExt3(unsigned int Edge, unsigned int interruptPriority);
int setuptExt4(unsigned int Edge, unsigned int interruptPriority);

void resetExt0();
void resetExt1();
void resetExt2();
void resetExt3();
void resetExt4();

#endif
