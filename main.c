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
 * Date            : October 19, 2018
 * Class           : EGR 326 Section 902
 * Instructor      : Dr. Nabeeh Kandalaft
 * Description     : Code to initialize eUSCI interface in IIC mode
 * that will read and write to the RTC over the IIC bus.
 * Credited Author : Professor Krug's Lecture Notes.
 ********************************************************************/

#include "driverlib.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "COMMONCLOCKS.h"
#include "KEYPAD.h"
#include "ST7735.h"
#include "RTC.h"
#include "LCD.h"
#include "LIGHTSENSOR.h"
#include <msp.h>
#include <stdlib.h>

// Statics
static volatile uint16_t curADCResult;
static volatile float normalizedADCRes;

void PWM();
void PWM_Init();

int   flag = 0;
float dc   = 75;

// Timer_A PWM Configuration Parameter
Timer_A_PWMConfig pwmConfig = { TIMER_A_CLOCKSOURCE_SMCLK,
                                TIMER_A_CLOCKSOURCE_DIVIDER_1,
                                6400,
                                TIMER_A_CAPTURECOMPARE_REGISTER_1,
                                TIMER_A_OUTPUTMODE_RESET_SET,
                                3200 };
int main(void)
{
    MAP_WDT_A_holdTimer();              // Halting the Watchdog
    curADCResult = 0;

    COMMONCLOCKS_sysTick_Init();                     // Initializing systick timer
    LIGHTSENSOR_ADC_Init    ();                     // ADC Initialization
    PWM_Init    ();                     // Pulse Width Modulation Initialization

    while (1)
    {
        if(flag == 1)
        {
            COMMONCLOCKS_sysTick_delay_3MHZ(500);         // delay 500 ms
            PWM();                      // Set dc and Trigger PWM
            flag = 0;                   // reset flag.
        }
    }
}

void ADC14_IRQHandler(void)     // ADC Interrupt Handler. This handler is called whenever there is a conversion that is finished for ADC_MEMM0
{
  uint64_t status = MAP_ADC14_getEnabledInterruptStatus();
  flag            = 1;
  MAP_ADC14_clearInterruptFlag(status);
    if (ADC_INT0 & status)
    {
     curADCResult     = MAP_ADC14_getResult(ADC_MEM0);
     normalizedADCRes = (curADCResult * 3.3) / 16384;
     MAP_ADC14_toggleConversionTrigger();
    }
}

void PWM()
{
    dc = (normalizedADCRes/3.3);                        // Equation for get dc
    //    TIMER_A0 -> CCR[0]  = 500 - 1;                         // PWM Period, 500 Hz frequency
    //    TIMER_A0 -> CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;          // Timer_A1 Capture/Compare Control 1 Register, Reset/Set output mode
    //    TIMER_A0 -> CCR[1]  = dc * 100;                        // PWM duty cycle
    pwmConfig.dutyCycle = dc * 6400;                    // changing the duty cycle

    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig); // Generating a PWM
}

void PWM_Init() {
MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION); //Configuring GPIO2.4 as peripheral output for PWM  and P6.7 for button * interrupt
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig); // Configuring Timer_A to have a period of approximately 500ms and an initial duty cycle of 10% of that (3200 ticks)
}
