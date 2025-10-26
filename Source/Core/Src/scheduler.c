/*
 * scheduler.c
 *
 *  Created on: Oct 26, 2025
 *      Author: Asus
 */
#include "scheduler.h"
static sTasks SCH_tasks_G[SCH_MAX_TASKS];
uint8_t current_index_task = 0;

uint8_t SCH_Delete_Task(uint32_t taskID) {
    // Kiểm tra taskID có hợp lệ không
    if (taskID >= SCH_MAX_TASKS) {
        return 0; // Thất bại: ID ngoài phạm vi
    }

    // Kiểm tra xem có task ở vị trí đó không
    if (SCH_tasks_G[taskID].pTask == 0) {
        return 0; // Thất bại: Không có task để xóa
    }

    // Xóa task: Chỉ cần reset các trường về 0
    SCH_tasks_G[taskID].pTask = 0x0000;
    SCH_tasks_G[taskID].Delay = 0;
    SCH_tasks_G[taskID].Period = 0;
    SCH_tasks_G[taskID].RunMe = 0;

    return 1; // Xóa thành công
}

void SCH_Init(void) {
    uint8_t i;
    for (i = 0; i < SCH_MAX_TASKS; i++) {
        // Xóa tất cả các task đã có
        SCH_Delete_Task(i);
    }
    // (Bỏ qua phần khởi tạo Watchdog và Error code)
}

uint32_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD) {
    uint32_t Index = 0;

    // 1. Tìm vị trí trống đầu tiên (O(n))
    // Vị trí trống là nơi pTask == 0
    while ((SCH_tasks_G[Index].pTask != 0) && (Index < SCH_MAX_TASKS)) {
        Index++;
    }

    // 2. Kiểm tra mảng có bị đầy không
    if (Index == SCH_MAX_TASKS) {
        // (Trong code PDF có xử lý lỗi, ở đây ta trả về ID không hợp lệ)
        return SCH_MAX_TASKS; // Hoặc NO_TASK_ID
    }

    // 3. Nếu tìm thấy chỗ trống, thêm task vào
    SCH_tasks_G[Index].pTask = pFunction;
    SCH_tasks_G[Index].Delay = DELAY;
    SCH_tasks_G[Index].Period = PERIOD;
    SCH_tasks_G[Index].RunMe = 0; // Ban đầu chưa chạy
    SCH_tasks_G[Index].TaskID = Index; // Lưu lại ID

    // 4. Trả về ID của task
    return Index;
}

void SCH_Update(void) {
    unsigned char Index;

    // Vòng lặp O(n) duyệt qua toàn bộ mảng task
    for (Index = 0; Index < SCH_MAX_TASKS; Index++) {
        // Kiểm tra xem có task ở vị trí này không
        if (SCH_tasks_G[Index].pTask) {
            // Nếu có, kiểm tra Delay
            if (SCH_tasks_G[Index].Delay == 0) {
                // Task đã đến hạn chạy
                // Tăng cờ RunMe
                SCH_tasks_G[Index].RunMe += 1;

                // Nếu là task định kỳ, nạp lại Delay
                if (SCH_tasks_G[Index].Period) {
                    SCH_tasks_G[Index].Delay = SCH_tasks_G[Index].Period;
                }
            } else {
                // Nếu chưa đến hạn, giảm Delay đi 1
                SCH_tasks_G[Index].Delay -= 1;
            }
        }
    }
}

    void SCH_Dispatch_Tasks(void) {
        unsigned char Index;

        // Vòng lặp O(n) duyệt qua toàn bộ mảng task
        for (Index = 0; Index < SCH_MAX_TASKS; Index++) {
            // Kiểm tra cờ RunMe
            if (SCH_tasks_G[Index].RunMe > 0) {
                // 1. Chạy task
                (*SCH_tasks_G[Index].pTask)();

                // 2. Giảm cờ RunMe
                SCH_tasks_G[Index].RunMe -= 1;

                // 3. Xử lý task one-shot (theo yêu cầu PDF và Video 2)
                if (SCH_tasks_G[Index].Period == 0) {
                    // Nếu Period = 0, xóa task sau khi chạy
                    SCH_Delete_Task(Index);
                }
            }
        }
    }
