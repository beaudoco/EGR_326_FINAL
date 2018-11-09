/*
 * KEYPAD.h
 *
 *  Created on: Nov 3, 2018
 *      Author: Collin Beaudoin
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

void KEYPAD_port_Init();
int KEYPAD_getKey();
uint16_t KEYPAD_promptUser(char* str);
void KEYPAD_printDay(uint8_t day);
uint16_t KEYPAD_promptUser(char* str);
void *KEYPAD_printTime(int * firstRead, char timeArr1[], char timeArr2[], char timeArr3[], char timeArr4[], char timeArr5[]);
void KEYPAD_readFromSlave();



#endif /* KEYPAD_H_ */
