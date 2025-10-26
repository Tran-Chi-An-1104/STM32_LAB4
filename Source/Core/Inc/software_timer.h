/*
 * software_timer.h
 *
 *  Created on: Sep 19, 2025
 *      Author: Tran Chi An
 */

#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

extern int timer1_flag;
extern int timer2_flag;
extern int timer3_flag;
extern int timer4_flag;
extern int timer5_flag;
extern int timer6_flag;

void setTimer1(int duration);
void timerRun1();
void setTimer2(int duration);
void timerRun2();
void setTimer3(int duration);
void timerRun3();
void setTimer4(int duration);
void timerRun4();
void setTimer5(int duration);
void timerRun5();
void setTimer6(int duration);
void timerRun6();

#endif /* INC_SOFTWARE_TIMER_H_ */
