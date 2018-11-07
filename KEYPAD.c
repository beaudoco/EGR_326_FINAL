/*
 * KEYPAD.c
 *
 *  Created on: Nov 3, 2018
 *      Author: Collin Beaudoin
 */
#include <stdio.h>
#include <stdint.h>
#include "COMMONCLOCKS.h"
#include "KEYPAD.h"
#include "msp.h"

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
//
//void KEYPAD_printDay(uint8_t day) {
//    switch(day) {
//        case 1:
//            printf("Sunday ");          // 1 = Sunday
//            break;
//        case 2:
//            printf("Monday ");          // 2 = Monday
//            break;
//        case 3:
//            printf("Tuesday ");         // 3 = Tuesday
//            break;
//        case 4:
//            printf("Wednesday ");       // 4 = Wednesday
//            break;
//        case 5:
//            printf("Thursday ");        // 5 = Thursday
//            break;
//        case 6:
//            printf("Friday ");          // 6 = Friday
//            break;
//        case 7:
//            printf("Saturday ");        // 7 = Saturday
//            break;
//    }
//}
//
//void KEYPAD_printTime(uint8_t optionKey,int firstRead) {
//
//    if(firstRead){
//        readFromSlave();
//        firstRead = 0;
//    }
//    readFromSlave(); // read value from external clock
//
//    uint8_t year, month, date, day, hour, minute, second, tmpInt, tmpFrac;
//
//    year   = convertFromBCD(RTC_registers[6]);      // get year
//    month  = convertFromBCD(RTC_registers[5]);      // get month
//    date   = convertFromBCD(RTC_registers[4]);      // get date
//    day    = convertFromBCD(RTC_registers[3]);      // get day
//    hour   = convertFromBCD(RTC_registers[2]);      // get hour
//    minute = convertFromBCD(RTC_registers[1]);      // get minute
//    second = convertFromBCD(RTC_registers[0]);      // get second
//    tmpInt  =  RTC_registers[17];                   // get int of tempature
//    tmpFrac = (RTC_registers[18] >> 6) * 25;        // get decimal of tempature
//
//    char iHourtoC[2];
//    char iMinuteC[2];
//    char iSecondC[2];
//    char iMonthC[2];
//    char iDateC[2];
//    char iYearC[4];
//
//    sprintf(iHourtoC,"%d",hour);
//    sprintf(iMinuteC,"%d",minute);
//    sprintf(iSecondC,"%d",second);
//    sprintf(iMonthC,"%d",month);
//    sprintf(iDateC,"%d",date);
//    sprintf(iYearC,"%d",year);
//
//    int count = 0;
//
//    int j = 0;
//    for(j = 0; j<sizeof(iHourtoC) / sizeof(uint8_t); j++){
//        testArray[j] = iHourtoC[j];
//        count ++;
//    }
//
//    memcpy (testArray + count, ":", 1);
//    count ++;
//    memcpy (testArray + count, iMinuteC , sizeof(iMinuteC));
//    count += (sizeof(iMinuteC) / sizeof(uint8_t));
//    memcpy (testArray + count, ":", 1);
//    count ++;
//    memcpy (testArray + count, iSecondC , sizeof(iSecondC));
//    count += sizeof(iSecondC) / sizeof(uint8_t);
//    memcpy (testArray + count, " ", 1);
//    count ++;
//    memcpy (testArray + count, iMonthC , sizeof(iMonthC));
//    count += (sizeof(iMonthC) / sizeof(uint8_t));
//    memcpy (testArray + count, "/", 1);
//    count ++;
//    memcpy (testArray + count, iDateC , sizeof(iDateC));
//    count += (sizeof(iDateC) / sizeof(uint8_t));
//    memcpy (testArray + count, "/", 1);
//    count ++;
//    memcpy (testArray + count, iYearC , sizeof(iYearC));
//    count += (sizeof(iYearC) / sizeof(uint8_t));
//    memcpy (testArray + count, "-", 1);
//    count ++;
//
//    int i = 0;
//    for(i = 0; i<25; i++){
//        timeArr5[i] = timeArr4[i];
//        timeArr4[i] = timeArr3[i];
//        timeArr3[i] = timeArr2[i];
//        timeArr2[i] = timeArr1[i];
//        timeArr1[i] = testArray[i];
//    }
//
//    MAP_FlashCtl_unprotectSector(FLASH_INFO_MEMORY_SPACE_BANK0,FLASH_SECTOR0);                      // Unprotecting Info Bank 0, Sector 0
//
//    while(!MAP_FlashCtl_eraseSector(CALIBRATION_START));                                            // Erase the flash sector starting CALIBRATION_START.
//                                                                                                    // Program the flash with the new data.
//    while (!MAP_FlashCtl_programMemory(timeArr1,(void*) CALIBRATION_START+4, 25 )); // leave first 4 bytes unprogrammed
//
//    while (!MAP_FlashCtl_programMemory(timeArr2,(void*) CALIBRATION_START+4+25, 25 ));
//
//    while (!MAP_FlashCtl_programMemory(timeArr3,(void*) CALIBRATION_START+4+50, 25 ));
//
//    while (!MAP_FlashCtl_programMemory(timeArr4,(void*) CALIBRATION_START+4+75, 25 ));
//
//    while (!MAP_FlashCtl_programMemory(timeArr5,(void*) CALIBRATION_START+4+100, 25 ));
//
//    MAP_FlashCtl_protectSector(FLASH_INFO_MEMORY_SPACE_BANK0,FLASH_SECTOR0);                        // Setting the sector back to protected
//
//    switch(optionKey)
//    {
//        // Todays Date
//        case 1:
//            printf("Todays Date: ");
//            printDay(day);
//            printf("%d/%d/20%d\n",month,date,year);
//            break;
//        case 2:
//            printf("Time: %d:%d:%d\n",hour,minute,second);
//            break;
//        case 3:
//            printf("Degree C : %d.%d\n", tmpInt, tmpFrac);
//            break;
//        default:
//            break;
//    }
//}
