/*
 * RTC.h
 *
 *  Created on: Nov 8, 2018
 *      Author: Collin Beaudoin
 */

#ifndef RTC_H_
#define RTC_H_

void RTC_iicInit(void);
void RTC_setTime(void);
void RTC_writeFromMaster(void);

#endif /* RTC_H_ */
