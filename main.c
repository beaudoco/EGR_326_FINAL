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
#include <msp.h>
#include <stdlib.h>

#define SLAVE_ADDRESS       0b1101000
#define NUM_OF_REC_BYTES    10
#define SIZE_ARRAY          25
#define NUMBER_ARRAY        5
#define CALIBRATION_START   0x000200000         // CALIBRATION START

const eUSCI_I2C_MasterConfig i2cConfig =
{
        EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        3000000,                                // SMCLK = 3MHz
        EUSCI_B_I2C_SET_DATA_RATE_400KBPS,      // Desired I2C Clock of 400khz
        0,                                      // No byte counter threshold
        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
};

int sysTikToggleSpeed = 15000;
int msDelay           = 25;
int firstRead         = 1;

uint8_t row, col, value, key;
uint8_t bcdSecond, bcdMinute, bcdHour, bcdDay, bcdDate, bcdMonth, bcdYear;
uint8_t textSize  = 4;

uint8_t inline convertToBCD(uint8_t dec) {return (dec%10) | (((dec/10)%10) << 4);}
uint8_t inline convertFromBCD(uint8_t bcd) {return (bcd & 0x0F) + (((bcd & 0xF0)>>4) * 10);}

uint16_t textColor = ST7735_GREEN;
uint16_t bgColor   = ST7735_BLACK;
uint16_t promptUser(char* str);

char timeArr1[25];
char timeArr2[25];
char timeArr3[25];
char timeArr4[25];
char timeArr5[25];
char RTC_registers[20];

void iicInit(void);
void readFromSlave(void);
void writeFromMaster(void);
//void port_Init(void);
void setTime(void);
void printTime(void);
void initClocks(void);

//int getKey(void);

int main(void)
{
    /* Halting the Watchdog */
    MAP_WDT_A_holdTimer();

    COMMONCLOCKS_sysTick_Init();                                // Systick Init
    KEYPAD_port_Init();                                                // Port Init
    iicInit();                                                  // Init iic to RTC
    initClocks();

    uint8_t read_back_data[SIZE_ARRAY];                         // array to hold values read back from flash
    uint8_t* addr_pointer;                                      // pointer to address in flash for reading back values

    ST7735_InitR(INITR_REDTAB);
    ST7735_FillScreen(0);
    ST7735_FillScreen(bgColor);
    //setTime();                                                  // set time variable for RTC
    ST7735_DrawString(0, 0, "Enter * to view", ST7735_YELLOW);
    ST7735_DrawString(0, 1, "previous times.", ST7735_YELLOW);

    while (1)
    {
        key = KEYPAD_getKey();

        if(key == 10) {
            uint8_t i;                                                                                      // index
            COMMONCLOCKS_sysTick_delay_48MHZ(msDelay);                                                      // Setting MCLK to 48MHz for faster programming
            addr_pointer = CALIBRATION_START+4;                                                             // point to address in flash for saving data

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

            printTime();

            fflush(stdout);
        }

    } // end of main while(1)
}

void setTime() {

    uint8_t tempVar;

    tempVar   = promptUser("year (0-99)");                  // ask for year
    bcdYear   = convertToBCD(tempVar);                      // assign year
    tempVar   = promptUser("Month (01-12)");                // ask for month
    bcdMonth  = convertToBCD(tempVar);                      // assign month
    tempVar   = promptUser("Date (01-31)");                 // ask for date
    bcdDate   = convertToBCD(tempVar);                      // assign date
    tempVar   = promptUser("Day (1-7)");                    // ask for day
    bcdDay    = convertToBCD(tempVar);                      // assign day
    tempVar   = promptUser("Hour (1-12 + AM/PM, 00-23)");   // ask for hour
    bcdHour   = convertToBCD(tempVar);                      // assign hour
    tempVar   = promptUser("Minute (00-59)");               // ask for minute
    bcdMinute = convertToBCD(tempVar);                      // assign minute
    tempVar   = promptUser("Second (00-59)");               // ask for second
    bcdSecond = convertToBCD(tempVar);                      // set second variable

    writeFromMaster();                                      // send bcd time/date/etc
}

uint16_t promptUser(char* str)
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

void printTime() {

    if(firstRead){
        readFromSlave();
        firstRead = 0;
    }
    readFromSlave(); // read value from external clock

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

    while(timeArr4[i] != '\0'){
        timeArr5[i] = timeArr4[i];
        i++;
    }
    i = 0;

    while(timeArr3[i] != '\0'){
        timeArr4[i] = timeArr3[i];
        i++;
    }
    i = 0;

    while(timeArr2[i] != '\0'){
        timeArr3[i] = timeArr2[i];
        i++;
    }
    i = 0;

    while(timeArr1[i] != '\0'){
        timeArr2[i] = timeArr1[i];
        i++;
    }

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

void iicInit() {

    // For example, select Port 6 for I2C
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,    // Set Pin 4, 5 to input Primary Module Function,
    GPIO_PIN4 + GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);           // P6.4 is UCB1SDA, P6.5 is UCB1SCL

    // Initializing I2C Master (see description in Driver Lib for
    MAP_I2C_initMaster(EUSCI_B1_BASE, &i2cConfig);                  // proper configuration options)
    MAP_I2C_setSlaveAddress(EUSCI_B1_BASE, SLAVE_ADDRESS);          // Specify slave address
    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);      // Set Master in transmit mode
    MAP_I2C_enableModule(EUSCI_B1_BASE);                            // Enable I2C Module to start operations
}

void writeFromMaster() {

    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);      // Set the master in transmit mode
    while(MAP_I2C_isBusBusy(EUSCI_B1_BASE));                        // Wait until bus is no longer busy, Master is ready to write
    MAP_I2C_masterSendMultiByteStart(EUSCI_B1_BASE, 0);             // Start multi-byte transmission from MSP432 to RTC
    MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, bcdSecond);      // Write to seconds register
    MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, bcdMinute);      // Write to minutes register
    MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, bcdHour);        // Write to hours register
    MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, bcdDay);         // Write to days register
    MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, bcdDate);        // Write to date register
    MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, bcdMonth);       // Write to months register
    MAP_I2C_masterSendMultiByteFinish(EUSCI_B1_BASE, bcdYear);      // Write to year register and send stop bit
}

void readFromSlave() {

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

//int getKey()
//{
//    for(col = 0; col < 3; col++)
//    {
//        P4DIR &= ~(BIT0 | BIT1 | BIT2);             // Disable all columns
//        P4DIR |=  BIT(col);                         // col# is enabled
//        P4OUT &= ~BIT(col);                         // Drive col# low
//        COMMONCLOCKS_sysTick_delay_48MHZ(10);                          // delay 20 ms.
//        row = (P4IN & 0b1111000);                   // Read the rows
//
//        /** Wait for button to be released */
//        while(!(P4IN & BIT3) | !(P4IN & BIT4) | !(P4IN & BIT5) | !(P4IN &BIT6));
//
//        P4OUT |= BIT(col);                          // Drive col# high
//
//        if(row != 0b1111000)                        // If a row is low a key has been pressed for the selected col
//            break;
//     }
//
//    P4OUT |= BIT0 | BIT1 | BIT2;                    // Drive columns high before disabling them
//    P4DIR &= ~(BIT0 | BIT1 | BIT2);                 // Disable All Columns
//
//    /** Simple Algebra to return the correct key press */
//    if(col == 3)
//        return 0;                                   // No key is pressed
//    if(row == 0b0111000)                            // Row 0
//        value = col + 1;
//    if(row == 0b1011000)                            // Row 1
//        value = 3 + col + 1;
//    if(row == 0b1101000)                            // Row 2
//        value = 6 + col + 1;
//    if(row == 0b1110000)                            // Row 3
//        value = 9 + col + 1;
//    return value;
//}

//void port_Init()
//{
//    P4SEL0 = 0x00;                                  // Port 4 set for GPIO
//    P4SEL1 = 0x00;
//    P4DIR  = 0X00;                                  // All bits in port 4 are setup as inputs
//    P4REN |= 0b1111000;                             // Enable pull resistor on bits 3-6
//    P4OUT |= 0b1111000;                             // Bits 3-6 are pull-up
//}

/*************************************************
 * Initializes MCLK to run from external crystal.
 * Code taken from lecture notes
 ************************************************/
void initClocks()
{
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ, GPIO_PIN3 | GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION); // Configuring pins for XTL usage
    MAP_CS_setExternalClockSourceFrequency(32000,48000000);                                                             // Setting external clock frequency
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);                                                                            // Starting HFXT
    MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
    MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);
    MAP_CS_startHFXT(false);
    MAP_CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);                                             // Initializing MCLK to HFXT (48MHz)
    MAP_CS_initClockSignal(CS_SMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_16);                                            // Setting SMCLK to 12MHz (HFXT/4)
}
