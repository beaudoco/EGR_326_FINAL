/*
 * KEYPAD.c
 *
 *  Created on: Nov 3, 2018
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
#include <msp.h>
#include <stdlib.h>

#define SIZE_ARRAY          25
#define CALIBRATION_START   0x000200000         // CALIBRATION START

char RTC_registers[20];

uint8_t inline convertFromBCD(uint8_t bcd) {return (bcd & 0x0F) + (((bcd & 0xF0)>>4) * 10);}

void KEYPAD_port_Init()
{
    P4SEL0 = 0x00;                                  // Port 4 set for GPIO
    P4SEL1 = 0x00;
    P4DIR  = 0X00;                                  // All bits in port 4 are setup as inputs
    P4REN |= 0b1111000;                             // Enable pull resistor on bits 3-6
    P4OUT |= 0b1111000;                             // Bits 3-6 are pull-up
}

int KEYPAD_getKey()
{
    uint8_t row, col, value;

    for(col = 0; col < 3; col++)
    {
        P4DIR &= ~(BIT0 | BIT1 | BIT2);             // Disable all columns
        P4DIR |=  BIT(col);                         // col# is enabled
        P4OUT &= ~BIT(col);                         // Drive col# low
        COMMONCLOCKS_sysTick_delay_3MHZ(10);                          // delay 20 ms.
        row = (P4IN & 0b1111000);                   // Read the rows

        /** Wait for button to be released */
        while(!(P4IN & BIT3) | !(P4IN & BIT4) | !(P4IN & BIT5) | !(P4IN &BIT6));

        P4OUT |= BIT(col);                          // Drive col# high

        if(row != 0b1111000)                        // If a row is low a key has been pressed for the selected col
            break;
     }

    P4OUT |= BIT0 | BIT1 | BIT2;                    // Drive columns high before disabling them
    P4DIR &= ~(BIT0 | BIT1 | BIT2);                 // Disable All Columns

    /** Simple Algebra to return the correct key press */
    if(col == 3)
        return 0;                                   // No key is pressed
    if(row == 0b0111000)                            // Row 0
        value = col + 1;
    if(row == 0b1011000)                            // Row 1
        value = 3 + col + 1;
    if(row == 0b1101000)                            // Row 2
        value = 6 + col + 1;
    if(row == 0b1110000)                            // Row 3
        value = 9 + col + 1;
    return value;
}

uint16_t KEYPAD_promptUser(char* str)
{
    uint16_t tempVar;

    printf("Please enter the %s then press the # key.\n", str);
    fflush(stdout);

    int key    = KEYPAD_getKey();
    int number = 0;
    do
    {
        if(key != 0 && key != 10)
        {
            key    = (key==11) ? 0 : key;
            number = number*10 + key;
            printf("%d", key);
            fflush(stdout);
        }

        key = KEYPAD_getKey();
    }while(key != 12);

    printf("\n");

    tempVar = number;

    return tempVar;
}

void KEYPAD_printDay(uint8_t day) {
    switch(day) {
        case 1:
            printf("Sunday ");          // 1 = Sunday
            break;
        case 2:
            printf("Monday ");          // 2 = Monday
            break;
        case 3:
            printf("Tuesday ");         // 3 = Tuesday
            break;
        case 4:
            printf("Wednesday ");       // 4 = Wednesday
            break;
        case 5:
            printf("Thursday ");        // 5 = Thursday
            break;
        case 6:
            printf("Friday ");          // 6 = Friday
            break;
        case 7:
            printf("Saturday ");        // 7 = Saturday
            break;
    }
}

void *KEYPAD_printTime(int *firstRead, char timeArr1[], char timeArr2[], char timeArr3[], char timeArr4[], char timeArr5[], uint16_t textColor) {

    if(*firstRead){
        KEYPAD_readFromSlave();
        *firstRead = 0;
    }
    KEYPAD_readFromSlave(); // read value from external clock

    uint8_t year, month, date, day, hour, minute, second, tmpInt, tmpFrac;

    year    = convertFromBCD(RTC_registers[6]);      // get year
    month   = convertFromBCD(RTC_registers[5]);      // get month
    date    = convertFromBCD(RTC_registers[4]);      // get date
    day     = convertFromBCD(RTC_registers[3]);      // get day
    hour    = convertFromBCD(RTC_registers[2]);      // get hour
    minute  = convertFromBCD(RTC_registers[1]);      // get minute
    second  = convertFromBCD(RTC_registers[0]);      // get second
    tmpInt  =  RTC_registers[17];                    // get int of tempature
    tmpFrac = (RTC_registers[18] >> 6) * 25;         // get decimal of tempature

    char iHourtoC[2];
    char iMinuteC[2];
    char iSecondC[2];
    char iMonthC[2];
    char iDateC[2];
    char iYearC[4];

    sprintf(iHourtoC,"%d",hour);
    sprintf(iMinuteC,"%d",minute);
    sprintf(iSecondC,"%d",second);
    sprintf(iMonthC,"%d",month);
    sprintf(iDateC,"%d",date);
    sprintf(iYearC,"%d",year);

    int count = 0;
    char testArray[SIZE_ARRAY]; // array to hold data

    int j = 0;
    for(j = 0; j<sizeof(iHourtoC) / sizeof(uint8_t); j++){
        testArray[j] = iHourtoC[j];
        count ++;
    }

    memcpy (testArray + count, ":", 1);
    count ++;
    memcpy (testArray + count, iMinuteC , sizeof(iMinuteC));
    count += (sizeof(iMinuteC) / sizeof(uint8_t));
    memcpy (testArray + count, ":", 1);
    count ++;
    memcpy (testArray + count, iSecondC , sizeof(iSecondC));
    count += sizeof(iSecondC) / sizeof(uint8_t);
    memcpy (testArray + count, " ", 1);
    count ++;
    memcpy (testArray + count, iMonthC , sizeof(iMonthC));
    count += (sizeof(iMonthC) / sizeof(uint8_t));
    memcpy (testArray + count, "/", 1);
    count ++;
    memcpy (testArray + count, iDateC , sizeof(iDateC));
    count += (sizeof(iDateC) / sizeof(uint8_t));
    memcpy (testArray + count, "/", 1);
    count ++;
    memcpy (testArray + count, iYearC , sizeof(iYearC));
    count += (sizeof(iYearC) / sizeof(uint8_t));

    int i = 0;
    int r = 0;

    memset(timeArr5,' ',SIZE_ARRAY);

    while(timeArr4[i] != '\0'){
        timeArr5[i] = timeArr4[i];
        i++;
    }
    i = 0;

    memset(timeArr4,' ',SIZE_ARRAY);

    while(timeArr3[i] != '\0'){
        timeArr4[i] = timeArr3[i];
        i++;
    }
    i = 0;

    memset(timeArr3,' ',SIZE_ARRAY);

    while(timeArr2[i] != '\0'){
        timeArr3[i] = timeArr2[i];
        i++;
    }
    i = 0;

    memset(timeArr2,' ',SIZE_ARRAY);

    while(timeArr1[i] != '\0'){
        timeArr2[i] = timeArr1[i];
        i++;
    }

    memset(timeArr1,' ',SIZE_ARRAY);

    for(i = 0; i<(sizeof(testArray) / sizeof(char)); i++) {

        if(testArray[i] != '\0') {
            timeArr1[r] = testArray[i];
            r++;
        }
    }

    MAP_FlashCtl_unprotectSector(FLASH_INFO_MEMORY_SPACE_BANK0,FLASH_SECTOR0);                      // Unprotecting Info Bank 0, Sector 0

    while(!MAP_FlashCtl_eraseSector(CALIBRATION_START));                                            // Erase the flash sector starting CALIBRATION_START.
                                                                                                    // Program the flash with the new data.
    while (!MAP_FlashCtl_programMemory(timeArr1,(void*) CALIBRATION_START+4, 25 ));                 // leave first 4 bytes unprogrammed

    while (!MAP_FlashCtl_programMemory(timeArr2,(void*) CALIBRATION_START+4+25, 25 ));

    while (!MAP_FlashCtl_programMemory(timeArr3,(void*) CALIBRATION_START+4+50, 25 ));

    while (!MAP_FlashCtl_programMemory(timeArr4,(void*) CALIBRATION_START+4+75, 25 ));

    while (!MAP_FlashCtl_programMemory(timeArr5,(void*) CALIBRATION_START+4+100, 25 ));

    MAP_FlashCtl_protectSector(FLASH_INFO_MEMORY_SPACE_BANK0,FLASH_SECTOR0);                        // Setting the sector back to protected

    ST7735_DrawString(2, 6, timeArr1, textColor);
    ST7735_DrawString(2, 7, timeArr2, textColor);
    ST7735_DrawString(2, 8, timeArr3, textColor);
    ST7735_DrawString(2, 9, timeArr4, textColor);
    ST7735_DrawString(2, 10, timeArr5, textColor);
}

void KEYPAD_readFromSlave() {

    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);      // Set Master in transmit mode
    while (MAP_I2C_isBusBusy(EUSCI_B1_BASE));                       // Wait for bus release, ready to write
    MAP_I2C_masterSendSingleByte(EUSCI_B1_BASE,0);                  // set pointer to beginning of RTC registers
    while (MAP_I2C_isBusBusy(EUSCI_B1_BASE));                       // Wait for bus release
    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_RECEIVE_MODE);       // Set Master in receive mode
    while (MAP_I2C_isBusBusy(EUSCI_B1_BASE));                       // Wait for bus release, ready to receive

    int i;  // read from RTC registers (pointer auto increments after each read)

    for(i = 0; i < 19; i++) {
        RTC_registers[i]=MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
    }

    while(MAP_I2C_isBusBusy(EUSCI_B1_BASE));    //Wait

}

