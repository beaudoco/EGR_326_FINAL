/*********************************************************************
 * Name            : Collin Beaudoin & Larson
 * Date            : October 19, 2018
 * Class           : EGR 326 Section 902
 * Instructor      : Dr. Nabeeh Kandalaft
 * Description     : Code to initialize eUSCI interface in IIC mode
 * that will read and write to the RTC over the IIC bus.
 * Credited Author : Professor Krug's Lecture Notes.
 ********************************************************************/
//#include "driverlib.h"
//#include <stdint.h>
//#include <stdbool.h>
//#include <stdio.h>
//#include <inttypes.h>
//#include <string.h>
//#include "COMMONCLOCKS.h"
//#include "KEYPAD.h"
//#include "ST7735.h"
//#include "RTC.h"
//#include <msp.h>
//#include <stdlib.h>
//
//void ADC_Init();
//void PWM();
//void PWM_Init();
//
//#define NUM_OF_REC_BYTES    10
//#define SIZE_ARRAY          25
//#define NUMBER_ARRAY        5
//#define CALIBRATION_START   0x000200000         // CALIBRATION START
//
//int msDelay           = 25;
//int firstRead         = 1;
//int key = 0;
//int   flag = 0;
//float dc   = 75;
//
//uint8_t textSize  = 4;
//
//uint16_t bgColor   = ST7735_BLACK;
//
//char timeArr1[SIZE_ARRAY];
//char timeArr2[SIZE_ARRAY];
//char timeArr3[SIZE_ARRAY];
//char timeArr4[SIZE_ARRAY];
//char timeArr5[SIZE_ARRAY];
//
//// Statics
//static volatile uint16_t curADCResult;
//static volatile float normalizedADCRes;
//
//// Timer_A PWM Configuration Parameter
//Timer_A_PWMConfig pwmConfig = { TIMER_A_CLOCKSOURCE_SMCLK,
//                                TIMER_A_CLOCKSOURCE_DIVIDER_1,
//                                6400,
//                                TIMER_A_CAPTURECOMPARE_REGISTER_1,
//                                TIMER_A_OUTPUTMODE_RESET_SET,
//                                3200 };
//
//int main(void)
//{
//    /* Halting the Watchdog */
//    MAP_WDT_A_holdTimer();
//
//    COMMONCLOCKS_sysTick_Init();                                // Systick Init
//    KEYPAD_port_Init();                                                // Port Init
//    RTC_iicInit();                                                  // Init iic to RTC
//    COMMONCLOCKS_initClocks();
//
//    curADCResult = 0;
//
//    ADC_Init    ();                     // ADC Initialization
//    PWM_Init    ();                     // Pulse Width Modulation Initialization
//
//    ST7735_InitR(INITR_REDTAB);
//    ST7735_FillScreen(0);
//    ST7735_FillScreen(bgColor);
//    //RTC_setTime();                                                  // set time variable for RTC
//    ST7735_DrawString(0, 0, "Enter * to view", ST7735_YELLOW);
//    ST7735_DrawString(0, 1, "previous times.", ST7735_YELLOW);
//
//    while (1)
//    {
//        key = KEYPAD_getKey();
//
//        if(key == 10) {
//            uint8_t i;                                                                                      // index
//            COMMONCLOCKS_sysTick_delay_48MHZ(msDelay);                                                      // Setting MCLK to 48MHz for faster programming
//            uint8_t* addr_pointer = CALIBRATION_START+4;                                                             // point to address in flash for saving data
//
//            for(i=0; i<25; i++) {                                                                           // read values in flash before programming
//                timeArr1[i] = *addr_pointer++;
//            }
//
//            for(i=0; i<25; i++) {                                                                           // read values in flash before programming
//                timeArr2[i] = *addr_pointer++;
//            }
//
//            for(i=0; i<25; i++) {                                                                           // read values in flash before programming
//                timeArr3[i] = *addr_pointer++;
//            }
//
//            for(i=0; i<25; i++) {                                                                           // read values in flash before programming
//                timeArr4[i] = *addr_pointer++;
//            }
//
//            for(i=0; i<25; i++) {                                                                           // read values in flash before programming
//                timeArr5[i] = *addr_pointer++;
//            }
//
//            addr_pointer = CALIBRATION_START+4;                                                             // point to address in flash for saved data
//
//            KEYPAD_printTime(&firstRead, timeArr1, timeArr2, timeArr3, timeArr4, timeArr5);
//
//            fflush(stdout);
//        }
//
//
//        if(flag == 1)
//        {
//            COMMONCLOCKS_sysTick_delay_3MHZ(500);         // delay 500 ms
//            PWM();                      // Set dc and Trigger PWM
//            flag = 0;                   // reset flag.
//        }
//
//    } // end of main while(1)
//}
//
//void ADC_Init(void) {
//
//    // Setting Flash wait state
//    MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
//    MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);
//
//    // Setting DCO to 48MHz
//    MAP_PCM_setPowerState(PCM_AM_LDO_VCORE1);
//    MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
//
//    // Enabling the FPU for floating point operation
//    MAP_FPU_enableModule();
//    MAP_FPU_enableLazyStacking();
//
//    // Initializing ADC (MCLK/1/4)
//    MAP_ADC14_enableModule();
//    MAP_ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_4, 0);
//
//    // Configuring GPIOs (5.5 A0)
//    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN5, GPIO_TERTIARY_MODULE_FUNCTION);
//
//    // Configuring ADC Memory
//    MAP_ADC14_configureSingleSampleMode(ADC_MEM0, true);
//    MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A0, false);
//
//    // Configuring Sample Timer
//    MAP_ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);
//
//    // Enabling/Toggling Conversion
//    MAP_ADC14_enableConversion();
//    MAP_ADC14_toggleConversionTrigger();
//
//    // Enabling interrupts
//    MAP_ADC14_enableInterrupt(ADC_INT0);
//    MAP_Interrupt_enableInterrupt(INT_ADC14);
//    MAP_Interrupt_enableMaster();
//}
//
//void PWM()
//{
//    dc = (normalizedADCRes/3.3);                        // Equation for get dc
//    //    TIMER_A0 -> CCR[0]  = 500 - 1;                         // PWM Period, 500 Hz frequency
//    //    TIMER_A0 -> CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;          // Timer_A1 Capture/Compare Control 1 Register, Reset/Set output mode
//    //    TIMER_A0 -> CCR[1]  = dc * 100;                        // PWM duty cycle
//    pwmConfig.dutyCycle = dc * 6400;                    // changing the duty cycle
//
//    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig); // Generating a PWM
//}
//
//void PWM_Init() {
//MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION); //Configuring GPIO2.4 as peripheral output for PWM  and P6.7 for button * interrupt
//    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig); // Configuring Timer_A to have a period of approximately 500ms and an initial duty cycle of 10% of that (3200 ticks)
//}
//
//void ADC14_IRQHandler(void)     // ADC Interrupt Handler. This handler is called whenever there is a conversion that is finished for ADC_MEMM0
//{
//  uint64_t status = MAP_ADC14_getEnabledInterruptStatus();
//  flag            = 1;
//  MAP_ADC14_clearInterruptFlag(status);
//    if (ADC_INT0 & status)
//    {
//     curADCResult     = MAP_ADC14_getResult(ADC_MEM0);
//     normalizedADCRes = (curADCResult * 3.3) / 16384;
//     MAP_ADC14_toggleConversionTrigger();
//    }
//}

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
#include "MotorLib.h"
#include "MSPIO.h"
#include "UART_Driver.h"
#include "KEYPAD.h"

#define BUFFER_SIZE    128
#define SLAVE_ADDRESS  0x48

// Statics
static volatile uint16_t curADCResult;
static volatile float normalizedADCRes;

/*Data Buffer*/
char Buffer[BUFFER_SIZE];

int dcM  = 75;
int key = 0;

uint16_t textColor = ST7735_GREEN;
uint16_t bgColor   = ST7735_BLACK;

uint16_t cusGrey;
uint16_t cusBlue;
uint16_t cusGreen;
uint16_t cusRed;

char TXData[10] = "RGBBGRBGGR";

void port_Init(void);
void initClocks(void);
void displayMenu(void);
void sysTick_Init();
void sysTick_delay(uint16_t delay);
void ADC_Init();
void PWM();
void PWM_Init();
void Backlight(void);

int   flag = 0;
float dc   = 75;

// Timer_A PWM Configuration Parameter
Timer_A_PWMConfig pwmConfig = { TIMER_A_CLOCKSOURCE_SMCLK,
                                TIMER_A_CLOCKSOURCE_DIVIDER_1,
                                6400,
                                TIMER_A_CAPTURECOMPARE_REGISTER_1,
                                TIMER_A_OUTPUTMODE_RESET_SET,
                                3200 };

const eUSCI_I2C_MasterConfig i2cConfig =
{
 EUSCI_B_I2C_CLOCKSOURCE_SMCLK,     // SMCLK Clock Source
 3000000,                           // SMCLK = 3MHz
 EUSCI_B_I2C_SET_DATA_RATE_100KBPS, // Desired I2C Clock of 400khz
 0,                                 // No byte counter threshold
 EUSCI_B_I2C_NO_AUTO_STOP           // No Autostop
};

/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 9600 baud rate. These
 * values were calculated using the online calculator that TI provides
 * at:
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
eUSCI_UART_Config UART2Config =
{
     EUSCI_A_UART_CLOCKSOURCE_SMCLK,
     19,
     8,
     0,
     EUSCI_A_UART_NO_PARITY,
     EUSCI_A_UART_LSB_FIRST,
     EUSCI_A_UART_ONE_STOP_BIT,
     EUSCI_A_UART_MODE,
     EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
};


int main(void)
{
    MAP_WDT_A_holdTimer();                      // Stop Watch dog
    curADCResult = 0;
    initClocks();                               // MSP432 running at 24 MHz
    UART_Init(EUSCI_A2_BASE, UART2Config);      // Initialize Hardware required for the HC-05
    port_Init();                                // Initializing ports for
    COMMONCLOCKS_sysTick_Init();              // Initializing systick timer
    ADC_Init    ();                             // ADC Initialization
    PWM_Init    ();                             // Pulse Width Modulation Initialization
    ST7735_InitR(INITR_REDTAB);                 // Initializing lcd

    MOTORLIB_portInit();                        // Initializing Motor ports
    MOTORLIB_timerInit();                       // Initializing Timer A0 ports
    MOTORLIB_setLeftDC(dcM);                    // Setting DC for Timer A) port
    MOTORLIB_setRightDC(dcM);                               // Setting DC for Timer A) port

    bgColor  = ST7735_Color565(37, 40, 48);                 // #252830
    cusGrey  = ST7735_Color565(207, 210, 218);              // #cfd2da
    cusBlue  = ST7735_Color565(28, 168, 221);               // #1ca8dd
    cusGreen = ST7735_Color565(27, 201, 142);               // #1bc98e
    cusRed   = ST7735_Color565(230, 71, 89);                // #E64759

    void KEYPAD_port_Init();                                // initializing keypad ports

    /*Get data from HC-05*/
    MSPrintf(EUSCI_A2_BASE, "Please Start Car...\r\n");
    MSPgets(EUSCI_A2_BASE, Buffer, BUFFER_SIZE);            // Will hang here until it receives any input.
    memset(Buffer, 0, sizeof(Buffer));                      // Clearing buffer

    ST7735_FillScreen(0);                                   // Turning on LCD
    ST7735_FillScreen(bgColor);                             // Setting Background

    displayMenu();                                          // Houstan... our demo is starting
    while(1)
    {
        key = KEYPAD_getKey();
        Backlight();

        if(UART_Available(EUSCI_A2_BASE)) {
            MSPgets2(EUSCI_A2_BASE, Buffer, BUFFER_SIZE);
            /*Send data to serial terminal*/
            MSPrintf(EUSCI_A2_BASE, "Data received from HC-05: %s\r\n", Buffer);
            ST7735_DrawString(2, 8,Buffer, textColor);
            if (Buffer[0] == 'F')
               MOTORLIB_moveForward();
            else if (Buffer[0] == 'B')
               MOTORLIB_moveBackward();
            else if (Buffer[0] == 'R')
               MOTORLIB_moveRight();
            else if (Buffer[0] == 'L')
               MOTORLIB_moveLeft();
            memset(Buffer, 0, sizeof(Buffer));
        } else if(key != 0) {
            ST7735_DrawString(2, 10,"             ", bgColor);
            if (key == 1 ) {
                /* get input from keypad */
                /* send RTC data */
            } else if (key == 2) {
                /* loook into flash */
            } else if (key == 3) {
                /* chime & lcd go crazy */
            } else {
                ST7735_DrawString(2, 10,"Invalid Input", cusRed);
            }
        }   // end of if keypad or bluetooth

        sysTick_delay(500);         // delay 500 ms
        PWM();

    }   // end of while(1)

}   // end of main

void sysTick_Init()
{
    SysTick -> CTRL = 0;                            // Disable SysTick during setup
    SysTick -> LOAD = 0x00FFFFFF;                   // Max reload value
    SysTick -> VAL  = 0;                            // Clear current value
    SysTick -> CTRL = 0x00000005;                   // Enable Systick, CPU clk, no interrupts
}

void sysTick_delay(uint16_t delay)
{
    SysTick -> LOAD = ((delay*3000) - 1);           // 1 ms * delay, counts down to 0
    SysTick -> VAL  = 0;                            // clear value
    while((SysTick -> CTRL & 0x00010000) == 0);     // Wait for flag to be set
}

void Backlight(void)
{
    ADC14->CTL0 |=1; //Start a conversion
    while(!ADC14->IFGR0); //Wait for conversion to complete

    curADCResult = ADC14->MEM[0]; //Places result in memory register
    normalizedADCRes = (curADCResult * 3.3) / 16384; //Calculating the voltage (Scale is 0-100)
     //If voltage
    dc = normalizedADCRes; //Changing the LED brightness
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
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION); //Configuring GPIO2.4 as peripheral output for PWM  and P6.7 for button * interrupt
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig); // Configuring Timer_A to have a period of approximately 500ms and an initial duty cycle of 10% of that (3200 ticks)
}

/* 128x160; Default textSize = 1 (10px); 21*20 */
void displayMenu(void) {
    ST7735_DrawString(3, 1,"Welcome Back ;)", cusBlue);
    ST7735_DrawString(2, 2,"Make Selection(s)", cusGreen);
    ST7735_DrawString(2, 4,"1. Set Date & Time", cusGrey);
    ST7735_DrawString(2, 5,"2. Show Alarm History", cusGrey);
    ST7735_DrawString(2, 6,"3. It's my Birthday", cusGrey);
}

/*************************************************
 * Initializes MCLK to run from external crystal.
 * Code taken from lecture notes
 ************************************************/
void initClocks(void)
{
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ, GPIO_PIN3 | GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION); // Configuring pins for XTL usage
    MAP_CS_setExternalClockSourceFrequency(32000,48000000);                                                             // Setting external clock frequency
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);                                                                            // Starting HFXT
    MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
    MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);
    MAP_CS_startHFXT(false);
    MAP_CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);                                             // Initializing MCLK to HFXT (48MHz)
    MAP_CS_initClockSignal(CS_SMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_16);                                            // Setting SMCLK to 12MHz (HFXT/4)
    //MAP_CS_initClockSignal(CS_ACLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_2);
}

void port_Init(void) {

}

void ADC_Init(void) {

    ADC14->CTL0 = 0x00000010; //Power on and disabled during configuration
    ADC14->CTL0 |= 0x04D90300; //S/H Pulse Mode, MCLCK, 32 Samble Clocks, Sw Trigger, 4 Clock Divide
    ADC14->CTL1 = 0x00000030; //14 Bit Resolution
    ADC14->MCTL[0] = 0; //A0 Input, Single Ended, VRef = AVCC
    P5->SEL1 |= 0x20; //Configure Pin 5.5 for A0
    P5->SEL0 |= 0x20; //Configure Pin 5.5 for A0
    ADC14->CTL1 |= 0x00000000; //Convert for Mem Register 0
    ADC14->CTL0 |= 2; //Enable ADC after configuration

//    // Setting Flash wait state
//    MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
//    MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);
//
//    // Setting DCO to 48MHz
//    MAP_PCM_setPowerState(PCM_AM_LDO_VCORE1);
//    MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
//
//    // Enabling the FPU for floating point operation
//    MAP_FPU_enableModule();
//    MAP_FPU_enableLazyStacking();
//
//    // Initializing ADC (MCLK/1/4)
//    MAP_ADC14_enableModule();
//    MAP_ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_4, 0);
//
//    // Configuring GPIOs (5.5 A0)
//    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN5, GPIO_TERTIARY_MODULE_FUNCTION);
//
//    // Configuring ADC Memory
//    MAP_ADC14_configureSingleSampleMode(ADC_MEM0, true);
//    MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A0, false);
//
//    // Configuring Sample Timer
//    MAP_ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);
//
//    // Enabling/Toggling Conversion
//    MAP_ADC14_enableConversion();
//    MAP_ADC14_toggleConversionTrigger();
//
//    // Enabling interrupts
//    MAP_ADC14_enableInterrupt(ADC_INT0);
//    MAP_Interrupt_enableInterrupt(INT_ADC14);
//    MAP_Interrupt_enableMaster();
}


