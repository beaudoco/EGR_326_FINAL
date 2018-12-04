/*
 * RTC.c
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
#include <msp.h>
#include <stdlib.h>

#define SLAVE_ADDRESS       0b1101000

const eUSCI_I2C_MasterConfig i2cConfigRTC =
{
        EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        3000000,                                // SMCLK = 3MHz
        EUSCI_B_I2C_SET_DATA_RATE_400KBPS,      // Desired I2C Clock of 400khz
        0,                                      // No byte counter threshold
        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
};

uint8_t bcdSecond, bcdMinute, bcdHour, bcdDay, bcdDate, bcdMonth, bcdYear;

uint8_t inline convertToBCD(uint8_t dec) {return (dec%10) | (((dec/10)%10) << 4);}

void RTC_iicInit() {

    // For example, select Port 6 for I2C
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,    // Set Pin 4, 5 to input Primary Module Function,
    GPIO_PIN4 + GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);           // P6.4 is UCB1SDA, P6.5 is UCB1SCL

    // Initializing I2C Master (see description in Driver Lib for
    MAP_I2C_initMaster(EUSCI_B1_BASE, &i2cConfigRTC);               // proper configuration options)
    MAP_I2C_setSlaveAddress(EUSCI_B1_BASE, SLAVE_ADDRESS);          // Specify slave address
    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);      // Set Master in transmit mode
    MAP_I2C_enableModule(EUSCI_B1_BASE);                            // Enable I2C Module to start operations
}

void RTC_setTime() {

    uint8_t tempVar;

    tempVar   = KEYPAD_promptUser("year (0-99)");                  // ask for year
    bcdYear   = convertToBCD(tempVar);                      // assign year
    tempVar   = KEYPAD_promptUser("Month (01-12)");                // ask for month
    bcdMonth  = convertToBCD(tempVar);                      // assign month
    tempVar   = KEYPAD_promptUser("Date (01-31)");                 // ask for date
    bcdDate   = convertToBCD(tempVar);                      // assign date
    tempVar   = KEYPAD_promptUser("Day (1-7)");                    // ask for day
    bcdDay    = convertToBCD(tempVar);                      // assign day
    tempVar   = KEYPAD_promptUser("Hour (1-12 + AM/PM, 00-23)");   // ask for hour
    bcdHour   = convertToBCD(tempVar);                      // assign hour
    tempVar   = KEYPAD_promptUser("Minute (00-59)");               // ask for minute
    bcdMinute = convertToBCD(tempVar);                      // assign minute
    tempVar   = KEYPAD_promptUser("Second (00-59)");               // ask for second
    bcdSecond = convertToBCD(tempVar);                      // set second variable

    RTC_writeFromMaster();                                      // send bcd time/date/etc
}

void RTC_writeFromMaster() {

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

