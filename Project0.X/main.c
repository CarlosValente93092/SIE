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

#include "../Common/Config/config_bits.h"
#include "../Common/Timers/Timers.h"
#include "../Common/UART/UART.h"
#include "../Common/ADC/ADC.h"
#include "../Common/PWM/PWM.h"

#define TRIS_OUT    0       //Define TRIS as output
#define TRIS_IN     1       //Define TRIS as input

int main(void) {


    TRISAbits.TRISA3 = TRIS_OUT; //Set port A3 to output (LED4)
    PORTAbits.RA3 = 0;

    int Fout = 100; //Hz
    if (setupTimer4(Fout, TIMER_USE_POLLING) == TIMER_SETUP_ERROR) {
        PORTAbits.RA3 = 0;
        putString("TIMER ERROR\n");
        return -1; //Error occurred
    }

    if (setupUART(115200, 0) != SETUP_UART_SUCCESS) {
        PORTAbits.RA3 = 0;
        putString("UART ERROR\n");
        return -1; //Error occurred
    }

    if (setupADC(2, 1, 0) != SETUP_ADC_SUCCESS) {
        PORTAbits.RA3 = 0;
        putString("ADC ERROR\n");
        return -1; //Error occurred
    }

    int Ton = 1; // seconds
    int count = 0;
    uint64_t rawValue;
    while (1) {
        waitTimer4();
        resetTimer4();
        count = count + 1;
        if (count > Ton * Fout) {
            count = 0;
            PORTAbits.RA3 = !PORTAbits.RA3;

            startADC();
            waitADC();
            int voltage = getValuesADC(&rawValue);
            resetADC();

            putString("Voltage: ");
            putInt(voltage);
            putString(" ,Mean: ");
            putInt(rawValue);
            putString("\n\r");
        }

    }

    return (EXIT_SUCCESS);
}