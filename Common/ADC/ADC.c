/* ************************************************************************** */
/**

  @Authors
    Carlos Valente, NMec 93092
    Gonçalo Urbano, NMec 92949

  @File Name
    ADC.c

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

#include <xc.h>
#include "ADC.h"

void configureAnalog(unsigned int analogPort) {
    switch (analogPort) {
        case 0:
        {
            TRISBbits.TRISB0 = 1; // RB0 digital output disconnected
            AD1PCFGbits.PCFG0 = 0; // RB0 configured as analog input (AN0)
            break;
        }
        case 1:
        {
            TRISBbits.TRISB1 = 1; // RB1 digital output disconnected
            AD1PCFGbits.PCFG1 = 0; // RB1 configured as analog input (AN1)
            break;
        }
        case 2:
        {
            TRISBbits.TRISB2 = 1; // RB2 digital output disconnected
            AD1PCFGbits.PCFG2 = 0; // RB2 configured as analog input (AN2)
            break;
        }
        case 3:
        {
            TRISBbits.TRISB3 = 1; // RB3 digital output disconnected
            AD1PCFGbits.PCFG3 = 0; // RB3 configured as analog input (AN3)
            break;
        }
        case 4:
        {
            TRISBbits.TRISB4 = 1; // RB4 digital output disconnected
            AD1PCFGbits.PCFG4 = 0; // RB4 configured as analog input (AN4)
            break;
        }
        case 5:
        {
            TRISBbits.TRISB5 = 1; // RB5 digital output disconnected
            AD1PCFGbits.PCFG5 = 0; // RB5 configured as analog input (AN5)
            break;
        }
        case 6:
        {
            TRISBbits.TRISB6 = 1; // RB6 digital output disconnected
            AD1PCFGbits.PCFG6 = 0; // RB6 configured as analog input (AN6)
            break;
        }
        case 7:
        {
            TRISBbits.TRISB7 = 1; // RB7 digital output disconnected
            AD1PCFGbits.PCFG7 = 0; // RB7 configured as analog input (AN7)
            break;
        }
        case 8:
        {
            TRISBbits.TRISB8 = 1; // RB8 digital output disconnected
            AD1PCFGbits.PCFG8 = 0; // RB8 configured as analog input (AN8)
            break;
        }
        case 9:
        {
            TRISBbits.TRISB9 = 1; // RB9 digital output disconnected
            AD1PCFGbits.PCFG9 = 0; // RB9 configured as analog input (AN9)
            break;
        }
        case 10:
        {
            TRISBbits.TRISB10 = 1; // RB10 digital output disconnected
            AD1PCFGbits.PCFG10 = 0; // RB10 configured as analog input (AN10)
            break;
        }
        case 11:
        {
            TRISBbits.TRISB11 = 1; // RB11 digital output disconnected
            AD1PCFGbits.PCFG11 = 0; // RB11 configured as analog input (AN11)
            break;
        }
        case 12:
        {
            TRISBbits.TRISB12 = 1; // RB12 digital output disconnected
            AD1PCFGbits.PCFG12 = 0; // RB12 configured as analog input (AN12)
            break;
        }
        case 13:
        {
            TRISBbits.TRISB13 = 1; // RB13 digital output disconnected
            AD1PCFGbits.PCFG13 = 0; // RB13 configured as analog input (AN13)
            break;
        }
        case 14:
        {
            TRISBbits.TRISB14 = 1; // RB14 digital output disconnected
            AD1PCFGbits.PCFG14 = 0; // RB14 configured as analog input (AN14)
            break;
        }
        case 15:
        {
            TRISBbits.TRISB15 = 1; // RB15 digital output disconnected
            AD1PCFGbits.PCFG15 = 0; // RB15 configured as analog input (AN15)
            break;
        }
    }
}

int setupADC(unsigned int analogPort, unsigned int numberSamples, unsigned int useInterrupts) {
    if (analogPort > 15) return SETUP_ADC_ERROR;
    if (numberSamples > 16) return SETUP_ADC_ERROR;

    configureAnalog(analogPort);
    // Conversion trigger selection bits: in this mode an internal counter ends sampling and starts conversion
    AD1CON1bits.SSRC = 7;
    // Stop conversions when the 1st A/D converter interrupt is generated. At the same time, hardware clears the ASAM bit
    AD1CON1bits.CLRASAM = 1;
    AD1CON3bits.SAMC = 16; // Sample time is 16 TAD (TAD = 100 ns)
    AD1CON2bits.SMPI = numberSamples - 1; //Interrupt is generated after an amount samples
    AD1CHSbits.CH0SA = analogPort; // Input analog channel (0 to 15)


    if (useInterrupts) {
        IPC6bits.AD1IP = 2; // configure priority of A/D interrupts
        IEC1bits.AD1IE = 1; // enable A/D interrupts
    }
    IFS1bits.AD1IF = 0;
    AD1CON1bits.ON = 1; // Enable A/D converter

    return SETUP_ADC_SUCCESS;
}

int getValuesADC(uint64_t *rawValue) {
    int sum = 0;
    for (int *p = (int *) (&ADC1BUF0); p <= (int *) (&ADC1BUFF); p += 4) {
        sum += *p;
    }
    uint64_t mean = sum / (AD1CON2bits.SMPI + 1);
    *rawValue = mean;
    return (mean * 33 + 511) / 1023;
}

void startADC(void) {
    AD1CON1bits.ASAM = 1; // Start conversion
}

void waitADC(void) {
    while (IFS1bits.AD1IF == 0);
}

void resetADC(void) {
    IFS1bits.AD1IF = 0;
}