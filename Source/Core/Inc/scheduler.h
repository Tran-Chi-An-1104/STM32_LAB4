/*
 * scheduler.h
 *
 *  Created on: Oct 26, 2025
 *      Author: Asus
 */

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include <stdint.h>

typedef struct{
	void (*pTask)(void);
	uint32_t Delay;
	uint32_t Period;
	uint8_t RunMe;

	uint32_t TaskID;
}sTasks;

#define SCH_MAX_TASKS 40
#define NO_TASK_ID 0

void SCH_Init(void);

uint32_t SCH_Add_Task ( void (*pFunction)() ,
					uint32_t DELAY,
					uint32_t PERIOD);

void SCH_Update(void);

void SCH_Dispatch_Tasks(void);

uint8_t SCH_Delete_Task(uint32_t taskID);


#endif /* INC_SCHEDULER_H_ */
