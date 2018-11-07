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
//void KEYPAD_printDay(uint8_t day);
//void KEYPAD_printTime(uint8_t optionKey);
//uint16_t KEYPAD_promptUser(char* str);
//void KEYPAD_setTime();


#endif /* KEYPAD_H_ */
