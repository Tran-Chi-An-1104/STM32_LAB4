/*
 * seven_segment.c
 *
 *  Created on: Oct 13, 2025
 *      Author: Asus
 */

#include "seven_segment.h"
#include "main.h"

const GPIO_PinState segmentMap[10][7] = {
    //A B C D E F G
    {GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET},   // 0
    {GPIO_PIN_SET,   GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET,   GPIO_PIN_SET,   GPIO_PIN_SET,   GPIO_PIN_SET},   // 1
    {GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET,   GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET,   GPIO_PIN_RESET}, // 2
    {GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET,   GPIO_PIN_SET,   GPIO_PIN_RESET}, // 3
    {GPIO_PIN_SET,   GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET,   GPIO_PIN_SET,   GPIO_PIN_RESET, GPIO_PIN_RESET}, // 4
    {GPIO_PIN_RESET, GPIO_PIN_SET,   GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET,   GPIO_PIN_RESET, GPIO_PIN_RESET}, // 5
    {GPIO_PIN_RESET, GPIO_PIN_SET,   GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET}, // 6
    {GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET,   GPIO_PIN_SET,   GPIO_PIN_SET,   GPIO_PIN_SET},   // 7
    {GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET}, // 8
    {GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET,   GPIO_PIN_RESET, GPIO_PIN_RESET}  // 9
};

//int led_buffer[4] = {0,0,0,0};

static void display7SEG(int num) {
    if (num < 0 || num > 9) return;
    HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, segmentMap[num][0]);
    HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, segmentMap[num][1]);
    HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, segmentMap[num][2]);
    HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, segmentMap[num][3]);
    HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, segmentMap[num][4]);
    HAL_GPIO_WritePin(F_GPIO_Port, F_Pin, segmentMap[num][5]);
    HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, segmentMap[num][6]);
}

/*static void display7SEG_Group2(int num) {
    if (num < 0 || num > 9) return;
    HAL_GPIO_WritePin(a2_GPIO_Port, a2_Pin, segmentMap[num][0]);
    HAL_GPIO_WritePin(b2_GPIO_Port, b2_Pin, segmentMap[num][1]);
    HAL_GPIO_WritePin(c2_GPIO_Port, c2_Pin, segmentMap[num][2]);
    HAL_GPIO_WritePin(d2_GPIO_Port, d2_Pin, segmentMap[num][3]);
    HAL_GPIO_WritePin(e2_GPIO_Port, e2_Pin, segmentMap[num][4]);
    HAL_GPIO_WritePin(f2_GPIO_Port, f2_Pin, segmentMap[num][5]);
    HAL_GPIO_WritePin(g2_GPIO_Port, g2_Pin, segmentMap[num][6]);
}*/

/*void seven_segment_scan() {
    static int current_led_index = 0;
    HAL_GPIO_WritePin(EN0_GPIO_Port, EN0_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(EN1_GPIO_Port, EN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(EN2_GPIO_Port, EN2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(EN3_GPIO_Port, EN3_Pin, GPIO_PIN_SET);

    switch (current_led_index) {
        case 0:
            display7SEG_Group1(led_buffer[0]);
            HAL_GPIO_WritePin(EN0_GPIO_Port, EN0_Pin, GPIO_PIN_RESET);
            break;
        case 1:
            display7SEG_Group1(led_buffer[1]);
            HAL_GPIO_WritePin(EN1_GPIO_Port, EN1_Pin, GPIO_PIN_RESET);
            break;
        case 2:
            display7SEG_Group2(led_buffer[2]);
            HAL_GPIO_WritePin(EN2_GPIO_Port, EN2_Pin, GPIO_PIN_RESET);
            break;
        case 3:
            display7SEG_Group2(led_buffer[3]);
            HAL_GPIO_WritePin(EN3_GPIO_Port, EN3_Pin, GPIO_PIN_RESET);
            break;
        default:
            break;
    }

    current_led_index++;
    if (current_led_index > 3) {
        current_led_index = 0;
    }
}*/

/*void update_led_buffer(int time) {
    if (time < 0 || time > 99) return;
    led_buffer[0] = time / 10;
    led_buffer[1] = time % 10;
}*/

/*void update_led_buffer2(int time) {
    if (time < 0 || time > 99) return;
    led_buffer[2] = time / 10;
    led_buffer[3] = time % 10;
}*/


