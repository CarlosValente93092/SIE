/* ************************************************************************** */
/**

  @Authors
    Carlos Valente, NMec 93092
    Gonçalo Urbano, NMec 92949

  @File Name
    Timers.h

  @Summary
    Main function to execute on Digilent chipKIT MAx32 board

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
#include "../Common/ExternalInterrupt/ExternalInterrupt.h"

#define TRIS_OUT    0       //Define TRIS as output
#define TRIS_IN     1       //Define TRIS as input

volatile unsigned int RXFlag;
volatile uint8_t character;

void __ISR(_UART1_VECTOR, IPL3AUTO) UART1ISR(void) {
    if (IFS0bits.U1RXIF) {
        RXFlag = 1;
        character = U1RXREG;
        IFS0bits.U1RXIF = 0; //reset UART1 RX interrupt flag
    }
}

void __ISR(_ADC_VECTOR, IPL2AUTO) ADCISR(void) {
    //    putVoltage(getValuesADC()); //Return ADC values
    //    putString("\n\r");
    //    resetADC(); //Reset ADC

    int thermistorVoltage = getValues2ADCA();
    int thermocoupleVoltage = getValues2ADCB();
    putVoltage(thermistorVoltage);
    putString("\n\r");
    putVoltage(thermocoupleVoltage);
    putString("\n\r\n\r");
    resetADC(); //Reset ADC
}

int main(void) {
    __XC_UART = 1; //Send input/output/errors to UART 1
    INTCONSET = _INTCON_MVEC_MASK; // Multi vector mode for interrupts


    TRISAbits.TRISA3 = TRIS_OUT; //Set port A3 to output (LED4)
    TRISGbits.TRISG9 = TRIS_OUT; //Set port G9 to output (pin 53)
    TRISGbits.TRISG8 = TRIS_OUT; //Set port G8 to output (pin 51)

    if (setupUART(115200, 1, 3) != SETUP_UART_SUCCESS) {
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }

    //    if (setupADC(0, 16, 1) == SETUP_ADC_ERROR) { //Pins A0 e A1
    //        PORTAbits.RA3 = 0;
    //        return -1; //Error occurred
    //    }

    if (setup2ADC(0, 1) == SETUP_ADC_ERROR) { //Pins A0 e A1
        PORTAbits.RA3 = 0;
        return -1; //Error occurred
    }


    __builtin_enable_interrupts(); // Turn on interrupts
    PORTAbits.RA3 = 1; //Every setup succeeded

    uint8_t clearLine[] = "                              "; //30 space characters to clear line

    putString("\r\nTest program.\r\n");
    putString("Commands:\r\n");
    putString("'+': turn led on\r\n");
    putString("'-': turn led off\r\n");
    putString("'x': toggle led state\r\n\r\n");
    while (1) {
        if (!RXFlag) continue;
        RXFlag = 0;
        switch (character) {

            case '+':
                /* Turn led on */
                PORTAbits.RA3 = 1;
                break;

            case '-':
                /* Turn led off */
                PORTAbits.RA3 = 0;
                break;
            case 'x':
                /* Toggle led state */
                PORTAINV = 0x08;
                break;
            case 't':
                startADC(); //Start Sampling
                //                waitADC(); //Wait for ADC to end
                //                putVoltage(getValuesADC()); //Return ADC values
                //                putString("\n\r");
                //                resetADC(); //Reset ADC
                break;
            default:
                break;
        }
    }

    return (EXIT_SUCCESS);
}