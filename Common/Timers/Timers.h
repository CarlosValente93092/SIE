/* ************************************************************************** */
/**

  @Authors
    Carlos Valente, NMec 93092
    Gonçalo Urbano, NMec 92949

  @File Name
    Timers.h

  @Summary
    Set of functions to initialise and control timers in PIC32MX795F512L using
 Digilent chipKIT MAx32 board 
 

 */
/* ************************************************************************** */

#ifndef _TIMERS_PIC32MX_H
#define _TIMERS_PIC32MX_H

// Define PIC32 clock speeds
#define SYSCLK 80000000L // System clock frequency, in Hz
#define PBCLOCK 40000000L // Peripheral Bus Clock frequency, in Hz

#define TIMER_SETUP_SUCCESS 0
#define TIMER_SETUP_ERROR -1

#define TIMER_USE_POLLING 0 //Configure to use polling
#define TIMER_USE_INTERRUPTS 1 //Configure to use interrupts

/********************************************************************
* Function: 	getCloserPrescaler()
* Precondition:
* Input: 		Frequency Ratio, List of possible prescaler constant 
 *              and size of prescaler list.
* Returns:      Prescaler if OK.
*               TIMER_SETUP_ERROR error in case of failure.
* Side Effects:	
* Overview:     Returns closest and next prescaler from the list of prescalers.
*
* Note:		 	frequencyRatio need to be uint64_t because of division
*
********************************************************************/
int getCloserPrescaler(uint64_t frequencyRatio, unsigned int prescalersList[], unsigned int numberOfPrescalers);

/********************************************************************
* Function: 	setupTimerX()
* Precondition:
* Input: 		Pretended output frequency.
* Returns:      TIMER_SETUP_SUCCESS if Ok.
*               TIMER_SETUP_ERROR error in case of failure.
* Side Effects:	Initializes registers related to timer X.
* Overview:     Initializes timer X.
*
* Note:		 	TODO: Add interrupt bool in case it is needed
*
********************************************************************/
int setupTimer1(unsigned int Fout, unsigned int useInterrupts);
int setupTimer2(unsigned int Fout, unsigned int useInterrupts);
int setupTimer3(unsigned int Fout, unsigned int useInterrupts);
int setupTimer4(unsigned int Fout, unsigned int useInterrupts);
int setupTimer5(unsigned int Fout, unsigned int useInterrupts);

/********************************************************************
* Function: 	waitTimerX()
* Precondition:
* Input: 		None
* Returns:      None
* Side Effects:	Stops executing of program
* Overview:     Waits for interrupt flag of timer X
*
* Note:		 	
*
********************************************************************/
void waitTimer1();
void waitTimer2();
void waitTimer3();
void waitTimer4();
void waitTimer5();

/********************************************************************
* Function: 	resetTimerX()
* Precondition: 
* Input: 		None
* Returns:      None
* Side Effects:	
* Overview:     Resets respective timer's interrupt flag
*
* Note:		 	
*
********************************************************************/
void resetTimer1();
void resetTimer2();
void resetTimer3();
void resetTimer4();
void resetTimer5();

#endif
