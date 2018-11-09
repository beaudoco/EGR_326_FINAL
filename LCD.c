/*
 * LCD.c
 *
 *  Created on: Nov 8, 2018
 *      Author: Collin Beaudoin
 */
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
#include <msp.h>
#include <stdlib.h>

void LCD_controlLCD(int distance, int *count, uint16_t textColor) {
    char dist[13];
    sprintf(dist, "Distance : %d", distance);
    if (distance <= 9 && distance >= 0) {
        if(distance <= 3 ) {
            if(*count<20)
                *count++;
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
            *count = 0;
        }
    }
}
