/*
 * scheduler_o1.c
 *
 */
#include "scheduler_o1.h"
#include "main.h"

static sTask SCH_tasks_Pool[SCH_MAX_TASKS];
static sTask* SCH_Task_List_Head = NULL;

static void SCH_Insert_Task_Into_List(sTask* taskToInsert) {
    uint32_t DELAY = taskToInsert->Delay;

    // NULL list
    if (SCH_Task_List_Head == NULL) {
        taskToInsert->next = NULL;
        SCH_Task_List_Head = taskToInsert;
        return;
    }

    // Insert before head
    if (DELAY < SCH_Task_List_Head->Delay) {
        taskToInsert->next = SCH_Task_List_Head;
        SCH_Task_List_Head->Delay = SCH_Task_List_Head->Delay - DELAY;
        SCH_Task_List_Head = taskToInsert;
        return;
    }

    // Insert in the mid or tail
    sTask* current = SCH_Task_List_Head;
    uint32_t accumulatedDelay = current->Delay;
    uint32_t remainingDelay = DELAY;

    if (remainingDelay >= accumulatedDelay) {
        remainingDelay -= accumulatedDelay;
    }

    while (current->next != NULL) {
        if (remainingDelay < current->next->Delay) {
            taskToInsert->Delay = remainingDelay;
            taskToInsert->next = current->next;
            current->next->Delay = current->next->Delay - remainingDelay;
            current->next = taskToInsert;
            return;
        }

        accumulatedDelay += current->next->Delay;
        if (remainingDelay >= current->next->Delay) {
             remainingDelay -= current->next->Delay;
        }
        current = current->next;
    }

    // Insert in to tail
    taskToInsert->Delay = remainingDelay;
    taskToInsert->next = NULL;
    current->next = taskToInsert;
}

void SCH_Init(void) {
    uint8_t i;
    for (i = 0; i < SCH_MAX_TASKS; i++) {
        SCH_tasks_Pool[i].pTask = NULL;
        SCH_tasks_Pool[i].next = NULL;
    }
    SCH_Task_List_Head = NULL;
}

static uint32_t find_empty_slot(void) {
    uint32_t Index = 0;
    while ((SCH_tasks_Pool[Index].pTask != NULL) && (Index < SCH_MAX_TASKS)) {
        Index++;
    }
    return Index;
}

uint32_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD) {
    uint32_t Index = find_empty_slot();
    if (Index == NO_TASK_ID) {
        return NO_TASK_ID;
    }

    sTask* newTask = &SCH_tasks_Pool[Index];
    newTask->pTask = pFunction;
    newTask->Period = PERIOD;
    newTask->RunMe = 0;
    newTask->TaskID = Index;
    newTask->Delay = DELAY;

    SCH_Insert_Task_Into_List(newTask);

    return Index;
}

void SCH_Update(void) {
    if (SCH_Task_List_Head == NULL) {
        return;
    }

    if (SCH_Task_List_Head->Delay == 0) {
        while (SCH_Task_List_Head != NULL && SCH_Task_List_Head->Delay == 0) {

            sTask* expiredTask = SCH_Task_List_Head;
            expiredTask->RunMe += 1;

            SCH_Task_List_Head = expiredTask->next;
            expiredTask->next = NULL;

            if (expiredTask->Period > 0) {
                expiredTask->Delay = expiredTask->Period;
                SCH_Insert_Task_Into_List(expiredTask);
            }
        }
    }
    if (SCH_Task_List_Head != NULL) {
        SCH_Task_List_Head->Delay--;
    }
}

void SCH_Dispatch_Tasks(void) {
    unsigned char Index;
    for (Index = 0; Index < SCH_MAX_TASKS; Index++) {

        if (SCH_tasks_Pool[Index].RunMe > 0) {

            (*SCH_tasks_Pool[Index].pTask)();
            SCH_tasks_Pool[Index].RunMe -= 1;

            if (SCH_tasks_Pool[Index].Period == 0) {
                 SCH_tasks_Pool[Index].pTask = NULL;
            }
        }
    }
}

uint8_t SCH_Delete_Task(uint32_t taskID) {
    if (taskID >= NO_TASK_ID || SCH_tasks_Pool[taskID].pTask == NULL) {
        return 0;
    }

    sTask* taskToDelete = &SCH_tasks_Pool[taskID];

    if (taskToDelete == SCH_Task_List_Head) {
        if (taskToDelete->next != NULL) {
            taskToDelete->next->Delay += taskToDelete->Delay;
        }
        SCH_Task_List_Head = taskToDelete->next;
    }
    else {
        sTask* current = SCH_Task_List_Head;
        while (current != NULL && current->next != taskToDelete) {
            current = current->next;
        }

        if (current != NULL) {
            sTask* nextNode = taskToDelete->next;
            if (nextNode != NULL) {
                nextNode->Delay += taskToDelete->Delay;
            }
            current->next = nextNode;
        }
    }

    taskToDelete->pTask = NULL;
    taskToDelete->Delay = 0;
    taskToDelete->Period = 0;
    taskToDelete->RunMe = 0;
    taskToDelete->next = NULL;

    return 1;
}
