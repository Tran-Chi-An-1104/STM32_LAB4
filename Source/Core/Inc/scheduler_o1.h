/*
 * scheduler_o1.h
 *
 * Phiên bản tối ưu O(1) dùng Delta Linked List
 * (Đã loại bỏ tính năng báo lỗi)
 */

#ifndef INC_SCHEDULER_O1_H_
#define INC_SCHEDULER_O1_H_

#include "stdint.h"
#include "stddef.h" // Để dùng NULL

// === Định nghĩa cấu trúc Task MỚI ===
// Thêm con trỏ 'next' để tạo danh sách liên kết
typedef struct sTask {
    void (* pTask)(void);
    uint32_t Delay;     // Bây giờ là "Delta Delay" (thời gian chờ so với task trước)
    uint32_t Period;
    uint8_t RunMe;
    uint32_t TaskID;    // Vẫn là index trong mảng Pool

    struct sTask* next; // Con trỏ 'next'
} sTask;


// === Các hằng số ===
#define SCH_MAX_TASKS 40 // Kích thước của mảng Pool
#define NO_TASK_ID    (SCH_MAX_TASKS)


// === Khai báo các hàm public ===

/**
 * @brief Khởi tạo Pool và danh sách task.
 */
void SCH_Init(void);

/**
 * @brief Thêm task vào danh sách (đã tối ưu O(n) để chèn vào list)
 */
uint32_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD);

/**
 * @brief Cập nhật task (đã tối ưu O(1), chỉ giảm delay của HEAD)
 */
void SCH_Update(void);

/**
 * @brief Chạy các task đã đến hạn (vẫn là O(n) duyệt Pool)
 */
void SCH_Dispatch_Tasks(void);

/**
 * @brief Xóa task (đã tối ưu O(n) để tìm và xóa khỏi list)
 */
uint8_t SCH_Delete_Task(uint32_t taskID);


#endif /* HEHE_SCHEDULER_O1_H_ */
