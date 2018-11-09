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
#include <msp.h>
#include <stdlib.h>

#define NUM_OF_REC_BYTES    10
#define SIZE_ARRAY          25
#define NUMBER_ARRAY        5
#define CALIBRATION_START   0x000200000         // CALIBRATION START

int sysTikToggleSpeed = 15000;
int msDelay           = 25;
int firstRead         = 1;

uint8_t textSize  = 4;

uint16_t bgColor   = ST7735_BLACK;

char timeArr1[SIZE_ARRAY];
char timeArr2[SIZE_ARRAY];
char timeArr3[SIZE_ARRAY];
char timeArr4[SIZE_ARRAY];
char timeArr5[SIZE_ARRAY];

int main(void)
{
    /* Halting the Watchdog */
    MAP_WDT_A_holdTimer();

    COMMONCLOCKS_sysTick_Init();                                // Systick Init
    KEYPAD_port_Init();                                                // Port Init
    RTC_iicInit();                                                  // Init iic to RTC
    COMMONCLOCKS_initClocks();

    ST7735_InitR(INITR_REDTAB);
    ST7735_FillScreen(0);
    ST7735_FillScreen(bgColor);
    //RTC_setTime();                                                  // set time variable for RTC
    ST7735_DrawString(0, 0, "Enter * to view", ST7735_YELLOW);
    ST7735_DrawString(0, 1, "previous times.", ST7735_YELLOW);

    while (1)
    {
        key = KEYPAD_getKey();

        if(key == 10) {
            uint8_t i;                                                                                      // index
            COMMONCLOCKS_sysTick_delay_48MHZ(msDelay);                                                      // Setting MCLK to 48MHz for faster programming
            uint8_t* addr_pointer = CALIBRATION_START+4;                                                             // point to address in flash for saving data

            for(i=0; i<25; i++) {                                                                           // read values in flash before programming
                timeArr1[i] = *addr_pointer++;
            }

            for(i=0; i<25; i++) {                                                                           // read values in flash before programming
                timeArr2[i] = *addr_pointer++;
            }

            for(i=0; i<25; i++) {                                                                           // read values in flash before programming
                timeArr3[i] = *addr_pointer++;
            }

            for(i=0; i<25; i++) {                                                                           // read values in flash before programming
                timeArr4[i] = *addr_pointer++;
            }

            for(i=0; i<25; i++) {                                                                           // read values in flash before programming
                timeArr5[i] = *addr_pointer++;
            }

            addr_pointer = CALIBRATION_START+4;                                                             // point to address in flash for saved data

            KEYPAD_printTime(&firstRead, timeArr1, timeArr2, timeArr3, timeArr4, timeArr5);

            fflush(stdout);
        }

    } // end of main while(1)
}
