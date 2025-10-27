/*
 * scheduler_o1.c
 *
 * Phiên bản tối ưu O(1) dùng Delta Linked List
 * (Đã SỬA LỖI "TASK LEAK")
 */
#include "scheduler_o1.h" // <-- Đã đổi tên file include
#include "main.h"

// === Cấu trúc dữ liệu MỚI ===

static sTask SCH_tasks_Pool[SCH_MAX_TASKS];
static sTask* SCH_Task_List_Head = NULL;


/**
 * @brief [HÀM PHỤ MỚI] Chèn 1 task (đã có) vào Linked List
 * Hàm này xử lý logic chèn O(n)
 */
static void SCH_Insert_Task_Into_List(sTask* taskToInsert) {
    uint32_t DELAY = taskToInsert->Delay;

    // TRƯỜNG HỢP 1: List rỗng
    if (SCH_Task_List_Head == NULL) {
        taskToInsert->next = NULL;
        SCH_Task_List_Head = taskToInsert;
        return;
    }

    // TRƯỜNG HỢP 2: Chèn vào ĐẦU (trước Head)
    if (DELAY < SCH_Task_List_Head->Delay) {
        taskToInsert->next = SCH_Task_List_Head;
        SCH_Task_List_Head->Delay = SCH_Task_List_Head->Delay - DELAY;
        SCH_Task_List_Head = taskToInsert;
        return;
    }

    // TRƯỜNG HỢP 3: Chèn vào GIỮA hoặc CUỐI
    sTask* current = SCH_Task_List_Head;
    uint32_t accumulatedDelay = current->Delay;
    uint32_t remainingDelay = DELAY;

    // DELAY có thể bằng accumulatedDelay, nên trừ trước
    if (remainingDelay >= accumulatedDelay) {
        remainingDelay -= accumulatedDelay;
    }

    while (current->next != NULL) {
        if (remainingDelay < current->next->Delay) {
            // Tìm thấy! Chèn 'taskToInsert'
            taskToInsert->Delay = remainingDelay;
            taskToInsert->next = current->next;
            current->next->Delay = current->next->Delay - remainingDelay;
            current->next = taskToInsert;
            return;
        }

        // Đi tiếp
        accumulatedDelay += current->next->Delay;
        if (remainingDelay >= current->next->Delay) {
             remainingDelay -= current->next->Delay;
        }
        current = current->next;
    }

    // Chèn vào CUỐI list
    taskToInsert->Delay = remainingDelay;
    taskToInsert->next = NULL;
    current->next = taskToInsert;
}


/**
 * @brief Khởi tạo mảng Pool
 */
void SCH_Init(void) {
    uint8_t i;
    for (i = 0; i < SCH_MAX_TASKS; i++) {
        SCH_tasks_Pool[i].pTask = NULL;
        SCH_tasks_Pool[i].next = NULL;
    }
    SCH_Task_List_Head = NULL;
}

/**
 * @brief Tìm slot trống trong Pool - O(n)
 */
static uint32_t find_empty_slot(void) {
    uint32_t Index = 0;
    while ((SCH_tasks_Pool[Index].pTask != NULL) && (Index < SCH_MAX_TASKS)) {
        Index++;
    }
    return Index;
}

/**
 * @brief Thêm task MỚI - O(n)
 */
uint32_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD) {
    // 1. Tìm slot trống
    uint32_t Index = find_empty_slot();
    if (Index == NO_TASK_ID) {
        return NO_TASK_ID;
    }

    // 2. Điền thông tin vào slot
    sTask* newTask = &SCH_tasks_Pool[Index];
    newTask->pTask = pFunction;
    newTask->Period = PERIOD;
    newTask->RunMe = 0;
    newTask->TaskID = Index;
    newTask->Delay = DELAY; // Đặt delay ban đầu

    // 3. Chèn task mới vào list
    SCH_Insert_Task_Into_List(newTask);

    return Index;
}


/**
 * @brief Cập nhật task - O(1) (Đã SỬA LỖI)
 */
void SCH_Update(void) {
    if (SCH_Task_List_Head == NULL) {
        return;
    }

    // Xử lý logic `Delay = 0` trước
    if (SCH_Task_List_Head->Delay == 0) {
        while (SCH_Task_List_Head != NULL && SCH_Task_List_Head->Delay == 0) {

            sTask* expiredTask = SCH_Task_List_Head;
            expiredTask->RunMe += 1;

            SCH_Task_List_Head = expiredTask->next;
            expiredTask->next = NULL;

            // === PHẦN SỬA LỖI QUAN TRỌNG ===
            if (expiredTask->Period > 0) {
                // 1. Đặt lại delay cho CHÍNH TASK CŨ
                expiredTask->Delay = expiredTask->Period;
                // 2. Chèn CHÍNH TASK CŨ (expiredTask) trở lại list
                SCH_Insert_Task_Into_List(expiredTask);
            }
            // Nếu Period = 0 (one-shot), task sẽ không được chèn lại
        }
    }

    // Sau khi xử lý xong, *mới* giảm delay của Head (nếu còn)
    if (SCH_Task_List_Head != NULL) {
        SCH_Task_List_Head->Delay--;
    }
}


/**
 * @brief Chạy task - O(n) duyệt Pool
 */
void SCH_Dispatch_Tasks(void) {
    unsigned char Index;
    for (Index = 0; Index < SCH_MAX_TASKS; Index++) {

        if (SCH_tasks_Pool[Index].RunMe > 0) {

            (*SCH_tasks_Pool[Index].pTask)();
            SCH_tasks_Pool[Index].RunMe -= 1;

            if (SCH_tasks_Pool[Index].Period == 0) {
                 SCH_tasks_Pool[Index].pTask = NULL; // Giải phóng slot
            }
        }
    }
}


/**
 * @brief Xóa task - O(n)
 */
uint8_t SCH_Delete_Task(uint32_t taskID) {
    if (taskID >= NO_TASK_ID || SCH_tasks_Pool[taskID].pTask == NULL) {
        return 0;
    }

    sTask* taskToDelete = &SCH_tasks_Pool[taskID];

    // Xóa khỏi Linked List (nếu nó đang nằm trong list chờ)
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

    // Xóa khỏi Pool
    taskToDelete->pTask = NULL;
    taskToDelete->Delay = 0;
    taskToDelete->Period = 0;
    taskToDelete->RunMe = 0;
    taskToDelete->next = NULL;

    return 1;
}
