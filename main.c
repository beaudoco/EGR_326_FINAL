/*
 * -------------------------------------------
 *    MSP432 DriverLib - v3_21_00_05 
 * -------------------------------------------
 *
 * --COPYRIGHT--,BSD,BSD
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/*********************************************************************
 * Name            : Collin Beaudoin & Larson
 * Date            : October 26, 2018
 * Class           : EGR 326 Section 902
 * Instructor      : Dr. Nabeeh Kandalaft
 * Description     : This Program uses an HC-SR04 Proximity sensor
 * to detect distance and display it to an LCD. If that distance is
 * within 3 inches the distance is dispalyed in red instead of green
 * Credited Author : Professor Krug's Lecture Notes.
 ********************************************************************/
#include "driverlib.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "COMMONCLOCKS.h"
#include "ST7735.h"
#include <msp.h>
#include <stdlib.h>

const Timer_A_CaptureModeConfig captureModeConfig =
{
     TIMER_A_CAPTURECOMPARE_REGISTER_1,             // CC Register 1
     TIMER_A_CAPTUREMODE_RISING_AND_FALLING_EDGE,   // Rising Edge and Falling
     TIMER_A_CAPTURE_INPUTSELECT_CCIxA,             // CCIxA Input Select
     TIMER_A_CAPTURE_SYNCHRONOUS,                   // Synchronized Capture
     TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE,       // Enable Interrupt
     TIMER_A_OUTPUTMODE_OUTBITVALUE                 // Output bit value
};

const Timer_A_ContinuousModeConfig continuousModeConfig =
{
     TIMER_A_CLOCKSOURCE_SMCLK,         // SMCLK Clock Source
     TIMER_A_CLOCKSOURCE_DIVIDER_1,     // SMCLK/1 = 3MHz
     TIMER_A_TAIE_INTERRUPT_DISABLE,    // Disable Timer ISR
     TIMER_A_SKIP_CLEAR                 // SKIP CLEAR COUNTER
};

int delay    = 150;
int distance = 0;
int meas1    = 0;
int meas2    = 0;
int count    = 0;

uint16_t textColor = ST7735_GREEN;
uint16_t bgColor   = ST7735_BLACK;

void controlLCD(int distance);

int main(void)
{
    /* Halting the Watchdog */
    MAP_WDT_A_holdTimer();

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN1);

    COMMONCLOCKS_sysTick_Init();                                        // Systick Init

    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ, GPIO_PIN3 | GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION); // Configuring pins for XTL usage
    MAP_CS_setExternalClockSourceFrequency(32000,48000000);                                                             // Setting external clock frequency
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);                                                                            // Starting HFXT
    MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
    MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);
    MAP_CS_startHFXT(false);
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_24);                    // 24000000 Hz
    CS_initClockSignal(CS_MCLK,  CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1); // 24000000 Hz
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_8); //  3000000 Hz

    ST7735_InitR(INITR_REDTAB);
    ST7735_FillScreen(0);
    ST7735_FillScreen(bgColor);

    /* Configuring P2.4 as peripheral input for capture */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN5);

    COMMONCLOCKS_timerAInit();

    /* Starting the Timer32 */
    Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
    Timer32_disableInterrupt(TIMER32_0_BASE);
    Timer32_setCount(TIMER32_0_BASE, 1);
    Timer32_startTimer(TIMER32_0_BASE, true);

    /* Starting the Timer_A0 in continuous mode */
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_CONTINUOUS_MODE);

    while (1)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN5);
        Timer32_setCount(TIMER32_0_BASE, 24 * 10);
        while (Timer32_getValue(TIMER32_0_BASE) > 0);       // Wait 10us
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN5);

        distance = (((meas2-meas1) *340)/60000)*.393701;
        controlLED(distance);
        COMMONCLOCKS_sysTick_delay_3MHZ(delay);
       // printf("%d\n",distance);

    } // end of main while(1)
}



void controlLCD(int distance) {
    char dist[13];
    sprintf(dist, "Distance : %d", distance);
    if (distance <= 9 && distance >= 0) {
        if(distance <= 3 ) {
            if(count<20)
                count++;
            else {
                MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN1);
                textColor = ST7735_RED;
                ST7735_DrawString(2, 8,dist, textColor);
            }
        }
        else {
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN1);
            textColor = ST7735_GREEN;
            ST7735_DrawString(2, 8,dist, textColor);
            count = 0;
        }
    }
}

void TA0_N_IRQHandler(void)
{
    int rising = 0;
    if (TIMER_A0->CCTL[1] & BIT0)                                                                   // Timer A0.1 was the cause. This is setup as a capture
        Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);     // clear timer_A interrupt flag
    if(P2IN&BIT4)
        rising=1;
    else                                                                                            // check for rising or falling edge on input
        rising = 0;
    if (rising) {
        meas1 = Timer_A_getCaptureCompareCount(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);   // read timer_A value
    } else {
        meas2 = Timer_A_getCaptureCompareCount(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
    }
}
