/* ************************************************************************** */
/**

  @Authors
    Carlos Valente, NMec 93092
    Gonçalo Urbano, NMec 92949

  @File Name
    PWM.h

  @Summary
    Set of functions to use PWM in PIC32MX795F512L
    Resolution is given as log2(Fprescaler / Fout)
    Pins:
        PW1, OC1, pin 3,  RE8
        PW2, OC2, pin 5,  RD1
        PW3, OC3, pin 5,  RD2
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

#ifndef _PWM_PIC32_H 
#define _PWM_PIC32_H

#define SETUP_PWM_SUCCESS 0
#define SETUP_PWM_ERROR -1

#define PWM_MAX_VALUE 1000

/********************************************************************
* Function: 	getOCXRS()
* Precondition:
* Input: 		Get constant for duty cycle.
* Returns:      Constant for OCXRS register.
* Side Effects:	
* Overview:     Depending on what timer we are using, it will get the according 
                value for OCXRS register.
*
* Note:		 	
*
********************************************************************/
int getOCXRS(unsigned int chooseTimer, unsigned int dutyCycle);

/********************************************************************
* Function: 	setPWMX()
* Precondition:
* Input: 		Timer to use (T2 or T3), dutycycle pretended, mode for output port
* Returns:      PWM_SETUP_SUCCESS if Ok.
*               PWM_SETUP_ERROR error in case of failure.
* Side Effects:	Sets respective port to PWM 
* Overview:     Select the OCxCON register to use and with the given Timer, duty 
                cycle and mode, generate a PWM wave in the respective port.
*
* Note:		 	
*
********************************************************************/
int setupPWM1(unsigned int chooseTimer, unsigned int dutyCycle, unsigned int mode);
int setupPWM2(unsigned int chooseTimer, unsigned int dutyCycle, unsigned int mode);
int setupPWM3(unsigned int chooseTimer, unsigned int dutyCycle, unsigned int mode);
int setupPWM4(unsigned int chooseTimer, unsigned int dutyCycle, unsigned int mode);
int setupPWM5(unsigned int chooseTimer, unsigned int dutyCycle, unsigned int mode);

void changeDutyCycle1(unsigned int chooseTimer, unsigned int dutyCycle);
void changeDutyCycle2(unsigned int chooseTimer, unsigned int dutyCycle);
void changeDutyCycle3(unsigned int chooseTimer, unsigned int dutyCycle);
void changeDutyCycle4(unsigned int chooseTimer, unsigned int dutyCycle);
void changeDutyCycle5(unsigned int chooseTimer, unsigned int dutyCycle);


#endif