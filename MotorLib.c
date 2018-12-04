/*********************************************************************
 * Name            : Collin Larson & Beaudoin
 * Date            : November 24th, 2018
 * Class           : EGR 326 Section 902
 * Instructor      : Dr. Nabeeh Kandalaft
 * Description     : Library Two control motors
 *
 *
 * Control :
 * | In1 | In2 | PWM | OUT1 | OUT2 | MODE |
 * |  H  |  H  | H/L |   L  |   L  |  SB  |
 * |  L  |  H  |  H  |   L  |   H  |  CCW |
 * |  L  |  H  |  L  |   L  |   L  |  SB  |
 * |  H  |  L  |  H  |   H  |   L  |  CW  |
 * |  H  |  L  |  L  |   L  |   L  |  SB  |
 * |  L  |  L  |  H  |  OFF |  OFF | STOP |
 * Don't forget STBY must be high for the motors to drive
 *
 * Abbreviations:
 * STBY = Stand By | CW = Clock Wise | CCW = Counter Clock Wise
 * SB = Short Brake
 *
 * LayOut:
 * MFL = PWM->2.5 In1->8.5 In2->9.0
 * MBL = PWM->2.5 In1->8.6 In2->8.7
 * MFR = PWM->2.6 In1->10.0 In2->10.2
 * MBR = PWM->2.6 In1->10.1 In2->10.3
 *
 * Modes:
 * Forward   = DC 100 Both Ways              | CW
 * Left      = DC 100 MFL MBL, DC 50 MFR MBR | CW
 * Right     = DC 100 MFR MBR, DC 50 MFL MBL | CW
 * Backwards = DC 100 Both Ways              | CCW
 ********************************************************************/

#include "driverlib.h"
#include <msp.h>
#include "MotorLib.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

int dcLeft  = 100;
int dcRight = 100;

void MOTORLIB_portInit(void) {

    // Configure GPIO
    P2->DIR  |= BIT4;
    P2->SEL0 |= BIT4;
    P2->SEL1 %= ~(BIT4);

    // Configure GPIO
    P2->DIR  |= BIT5;
    P2->SEL0 |= BIT5;
    P2->SEL1 %= ~(BIT5);

    // Configure GPIO
    P2->DIR  |= BIT6;
    P2->SEL0 |= BIT6;
    P2->SEL1 %= ~(BIT6);

    // Configure GPIO
    P2->DIR  |= BIT7;
    P2->SEL0 |= BIT7;
    P2->SEL1 %= ~(BIT7);

    /* MFL */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN5);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P9, GPIO_PIN0);
    /* MFR */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P10, GPIO_PIN0);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P10, GPIO_PIN2);
    /* MBL */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN6);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN7);
    /* MBR */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P10, GPIO_PIN1);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P10, GPIO_PIN3);
}

void MOTORLIB_timerInit(void) {
    TIMER_A0 -> CTL = TIMER_A_CTL_SSEL__SMCLK | // Timer_A control Register clock source select, Use SMCLK
    TIMER_A_CTL_MC__UP |                        // Timer_A control Register mode control (MC) up mode counts from 0 - TA1CCRX value
    TIMER_A_CTL_CLR;                            // Timer_A control Register clear
}

void MOTORLIB_moveForward(void) {
    MOTORLIB_setTimerALeftDC(dcLeft);
    MOTORLIB_setTimerARightDC(dcRight);

    /* MFL */
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN5);                 // High
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P9, GPIO_PIN0);                  // Low
    /* MFR */
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P10, GPIO_PIN0);                // High
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P10, GPIO_PIN2);                 // Low
    /* MBL */
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN6);                 // High
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN7);                  // Low
    /* MBR */
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P10, GPIO_PIN1);                // High
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P10, GPIO_PIN3);                 // Low

}

void MOTORLIB_moveBackward(void) {
    MOTORLIB_setTimerALeftDC(dcLeft);
    MOTORLIB_setTimerARightDC(dcRight);

    /* MFL */
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN5);                  // High
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P9, GPIO_PIN0);                 // Low
    /* MFR */
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P10, GPIO_PIN0);                 // High
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P10, GPIO_PIN2);                // Low
    /* MBL */
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN6);                  // High
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN7);                 // Low
    /* MBR */
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P10, GPIO_PIN1);                 // High
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P10, GPIO_PIN3);                // Low

}

void MOTORLIB_moveLeft(void) {
    MOTORLIB_setTimerALeftDC(dcLeft/2);
    MOTORLIB_setTimerARightDC(dcRight);
}

void MOTORLIB_moveRight(void) {
    MOTORLIB_setTimerALeftDC(dcLeft);
    MOTORLIB_setTimerARightDC(dcRight/2);
}

void MOTORLIB_setTimerALeftDC(int dc) {
    TIMER_A0->CCR[0]  = 30000; // PWM Period
    TIMER_A0->CCTL[2] = TIMER_A_CCTLN_OUTMOD_7; //CCR1 reset/set
    TIMER_A0->CCR[2]  = dc*3000;// CCR1 PWM Duty Cycle
    TIMER_A0->CTL     = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR; // Clear TAR
}

void MOTORLIB_setTimerARightDC(int dc) {
    TIMER_A0->CCR[0]  = 30000; // PWM Period
    TIMER_A0->CCTL[3] = TIMER_A_CCTLN_OUTMOD_7; //CCR1 reset/set
    TIMER_A0->CCR[3]  = dc*3000;// CCR1 PWM Duty Cycle
    TIMER_A0->CTL     = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR; // Clear TAR
}

void MOTORLIB_setLeftDC(int dc) {
    dcLeft = dc;
}

void MOTORLIB_setRightDC(int dc) {
    dcRight = dc;
}
