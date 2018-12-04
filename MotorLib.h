#ifndef _MOTORLIBH_
#define _MOTORLIBH_

void MOTORLIB_portInit(void);
void MOTORLIB_timerInit(void);
void MOTORLIB_moveForward(void);
void MOTORLIB_moveBackward(void);
void MOTORLIB_moveLeft(void);
void MOTORLIB_moveRight(void);
void MOTORLIB_setTimerALeftDC(int dc);
void MOTORLIB_setTimerARightDC(int dc);
void MOTORLIB_setLeftDC(int dc);
void MOTORLIB_setRightDC(int dc);

#endif
