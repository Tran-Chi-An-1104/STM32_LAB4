/*
 * scheduler_o1.c
 *
 * Phiên bản tối ưu O(1) dùng Delta Linked List
 * (Đã loại bỏ tính năng báo lỗi)
 */
#include "scheduler_o1.h" // <-- Đã đổi tên file include

#include "main.h"

// === Cấu trúc dữ liệu MỚI ===

// 1. Mảng Pool: Nơi lưu trữ vật lý của tất cả các task
static sTask SCH_tasks_Pool[SCH_MAX_TASKS];

// 2. Con trỏ Head: Trỏ đến task đầu tiên trong danh sách CHỜ (Sorted Linked List)
static sTask* SCH_Task_List_Head = NULL;


/**
 * @brief [HÀM MỚI] Khởi tạo mảng Pool
 */
void SCH_Init(void) {
    uint8_t i;
    for (i = 0; i < SCH_MAX_TASKS; i++) {
        // Đánh dấu slot là trống
        SCH_tasks_Pool[i].pTask = NULL;
        SCH_tasks_Pool[i].next = NULL;
    }
    // Danh sách chờ ban đầu rỗng
    SCH_Task_List_Head = NULL;
}

/**
 * @brief [HÀM MỚI] Tìm slot trống trong Pool - O(n)
 * @return Index của slot trống, hoặc NO_TASK_ID nếu Pool đầy
 */
static uint32_t find_empty_slot(void) {
    uint32_t Index = 0;
    while ((SCH_tasks_Pool[Index].pTask != NULL) && (Index < SCH_MAX_TASKS)) {
        Index++;
    }
    return Index;
}

/**
 * @brief [HÀM MỚI] Thêm task - O(n)
 * Tìm slot trống, sau đó chèn vào Sorted Linked List
 */
uint32_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD) {
    // 1. Tìm slot trống trong Pool
    uint32_t Index = find_empty_slot();
    if (Index == NO_TASK_ID) {
        // Pool đầy!
        return NO_TASK_ID;
    }

    // 2. Lấy con trỏ đến task mới và điền thông tin
    sTask* newTask = &SCH_tasks_Pool[Index];
    newTask->pTask = pFunction;
    newTask->Period = PERIOD;
    newTask->RunMe = 0;
    newTask->TaskID = Index;
    newTask->next = NULL;

    // 3. Chèn vào danh sách liên kết Delta

    // TRƯỜNG HỢP 1: List rỗng
    if (SCH_Task_List_Head == NULL) {
        newTask->Delay = DELAY;
        SCH_Task_List_Head = newTask;
        return Index;
    }

    // TRƯỜNG HỢP 2: Chèn vào ĐẦU (trước Head)
    if (DELAY < SCH_Task_List_Head->Delay) {
        newTask->Delay = DELAY;
        newTask->next = SCH_Task_List_Head;
        // Cập nhật Delta của Head cũ
        SCH_Task_List_Head->Delay = SCH_Task_List_Head->Delay - DELAY;
        // Cập nhật Head mới
        SCH_Task_List_Head = newTask;
        return Index;
    }

    // TRƯỜNG HỢP 3: Chèn vào GIỮA hoặc CUỐI
    sTask* current = SCH_Task_List_Head;
    uint32_t accumulatedDelay = current->Delay;
    uint32_t remainingDelay = DELAY - accumulatedDelay;

    // Duyệt list để tìm vị trí chèn
    while (current->next != NULL) {
        if (remainingDelay < current->next->Delay) {
            // Tìm thấy! Chèn 'newTask' vào giữa 'current' và 'current->next'
            newTask->Delay = remainingDelay;
            newTask->next = current->next;
            // Cập nhật delta của node sau nó
            current->next->Delay = current->next->Delay - remainingDelay;
            // Chèn vào list
            current->next = newTask;
            return Index;
        }

        // Đi tiếp
        accumulatedDelay += current->next->Delay;
        // Đảm bảo không bị tràn số nếu DELAY rất lớn
        if (accumulatedDelay > DELAY) {
            remainingDelay = 0; // Sẽ chèn vào ngay sau 'current'
        } else {
            remainingDelay = DELAY - accumulatedDelay;
        }
        current = current->next;
    }

    // Nếu đến đây, chèn vào CUỐI list
    newTask->Delay = remainingDelay;
    current->next = newTask;
    return Index;
}


/**
 * @brief [HÀM MỚI] Cập nhật task - O(1)
 * Chỉ làm việc với node HEAD
 */
void SCH_Update(void) {
    // 1. Nếu list rỗng, không làm gì cả
    if (SCH_Task_List_Head == NULL) {
        return;
    }

    // 2. Chỉ giảm Delay của node HEAD
    SCH_Task_List_Head->Delay--;

    // 3. Xử lý tất cả các task ở đầu list đã đến hạn
    while (SCH_Task_List_Head != NULL && SCH_Task_List_Head->Delay == 0) {
        // 3.1. Lấy task đã hết hạn ra
        sTask* expiredTask = SCH_Task_List_Head;

        // 3.2. Đánh dấu RunMe
        expiredTask->RunMe += 1;

        // 3.3. Cập nhật Head (ngắt task ra khỏi list chờ)
        SCH_Task_List_Head = expiredTask->next;
        expiredTask->next = NULL; // Dọn dẹp con trỏ

        // 3.4. Nếu là task định kỳ, thêm nó trở lại list
        if (expiredTask->Period > 0) {
            SCH_Add_Task(expiredTask->pTask, expiredTask->Period, expiredTask->Period);
        }
    }
}


/**
 * @brief [HÀM MỚI] Chạy task - O(n) duyệt Pool
 * Hàm này duyệt mảng POOL, không duyệt LINKED LIST
 */
void SCH_Dispatch_Tasks(void) {
    unsigned char Index;

    // Vòng lặp O(n) duyệt qua toàn bộ POOL
    for (Index = 0; Index < SCH_MAX_TASKS; Index++) {

        if (SCH_tasks_Pool[Index].RunMe > 0) {

            // 1. Chạy task
            (*SCH_tasks_Pool[Index].pTask)();

            // 2. Giảm cờ RunMe
            SCH_tasks_Pool[Index].RunMe -= 1;

            // 3. Nếu là task one-shot, giải phóng slot trong POOL
            if (SCH_tasks_Pool[Index].Period == 0) {
                 SCH_tasks_Pool[Index].pTask = NULL; // Đánh dấu slot là trống
            }
        }
    }
}


/**
 * @brief [HÀM MỚI] Xóa task - O(n)
 * Phải xóa task khỏi cả Linked List và Pool
 */
uint8_t SCH_Delete_Task(uint32_t taskID) {
    // 1. Kiểm tra ID hợp lệ
    if (taskID >= NO_TASK_ID || SCH_tasks_Pool[taskID].pTask == NULL) {
        return 0; // Task không tồn tại
    }

    sTask* taskToDelete = &SCH_tasks_Pool[taskID];

    // 2. Xóa khỏi Linked List (nếu nó đang nằm trong list chờ)

    // TRƯỜNG HỢP 1: Task cần xóa là HEAD
    if (taskToDelete == SCH_Task_List_Head) {
        // Cộng dồn delta cho node tiếp theo (nếu có)
        if (taskToDelete->next != NULL) {
            taskToDelete->next->Delay += taskToDelete->Delay;
        }
        SCH_Task_List_Head = taskToDelete->next;
    }
    // TRƯỜNG HỢP 2: Task nằm ở GIỮA hoặc CUỐI
    else {
        sTask* current = SCH_Task_List_Head;
        // Tìm node nằm TRƯỚC node cần xóa
        while (current != NULL && current->next != taskToDelete) {
            current = current->next;
        }

        if (current != NULL) {
            // Tìm thấy node 'current' nằm ngay trước 'taskToDelete'
            sTask* nextNode = taskToDelete->next;
            // Cộng dồn delta cho node tiếp theo (nếu có)
            if (nextNode != NULL) {
                nextNode->Delay += taskToDelete->Delay;
            }
            // Nối tắt qua node bị xóa
            current->next = nextNode;
        }
        // (Nếu current == NULL, task không có trong list chờ,
        // ví dụ nó đã RunMe > 0, nên không cần làm gì)
    }

    // 3. Xóa khỏi Pool (luôn luôn thực hiện)
    taskToDelete->pTask = NULL;
    taskToDelete->Delay = 0;
    taskToDelete->Period = 0;
    taskToDelete->RunMe = 0;
    taskToDelete->next = NULL;

    return 1; // Xóa thành công
}
