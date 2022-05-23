/* ************************************************************************** */
/**

  @Authors
    Carlos Valente, NMec 93092
    Gonçalo Urbano, NMec 92949

  @File Name
    ADC.h

  @Summary
    Set of functions to use PWM in PIC32MX795F512L
    analogPort:
        Pins RB0 to RB15
    numberSamples:
        Number of samples per conversion done
    useInterrupts:
        Flag for the case of using interrupts

 */

/* ************************************************************************** */

#ifndef _ADC_PIC32_H    /* Guard against multiple inclusion */
#define _ADC_PIC32_H

#define SETUP_ADC_SUCCESS 0
#define SETUP_ADC_ERROR -1

/********************************************************************
* Function: 	configureAnalog()
* Precondition:
* Input: 		Analog port to be used
* Returns:      
* Side Effects:	Sets respective pin to be an input analog pin
* Overview:     Defines the correspondent pin to be input analog pin
*
* Note:		 	
*
********************************************************************/
void configureAnalog(unsigned int analogPort);

/********************************************************************
* Function: 	setupADC()
* Precondition:
* Input: 		Analog port to be used, number of samples per conversion and 
                interrupt flag if needed
* Returns:      ADC_SETUP_SUCCESS if Ok.
*               ADC_SETUP_ERROR error in case of failure.
* Side Effects:	Sets respective port to PWM 
* Overview:     Select the OCxCON register to use and with the given Timer, duty 
                cycle and mode, generate a PWM wave in the respective port.
*
* Note:		 	
*
********************************************************************/
int setupADC(unsigned int analogPort, unsigned int numberSamples, unsigned int useInterrupts);

/********************************************************************
* Function: 	getValuesADC()
* Precondition: Needs to be called after interrupted has generated
* Input: 		
* Returns:      Mean of all samples taken from conversion done
* Side Effects:	
* Overview:     Loops the ADC buffer registers summing every value obtained and
                returning the mean of those values.
*
* Note:		 	
*
********************************************************************/
int getValuesADC();

void startADC(void);
void waitADC(void);
void resetADC(void);

int setup2ADC(unsigned int, unsigned int);
int getValues2ADCA(void);
int getValues2ADCB(void);

#endif 

