//
// Created by RM UI Designer
//

#include "ui_gold_island_arm_0.h"

#define FRAME_ID 1
#define GROUP_ID 2
#define START_ID 0
#define OBJ_NUM 4
#define FRAME_OBJ_NUM 5

CAT(ui_, CAT(FRAME_OBJ_NUM, _frame_t)) ui_gold_island_arm_0;
ui_interface_line_t *ui_gold_island_arm_Line1 = (ui_interface_line_t *)&(ui_gold_island_arm_0.data[0]);
ui_interface_line_t *ui_gold_island_arm_Line2 = (ui_interface_line_t *)&(ui_gold_island_arm_0.data[1]);
ui_interface_line_t *ui_gold_island_arm_Line3 = (ui_interface_line_t *)&(ui_gold_island_arm_0.data[2]);
ui_interface_round_t *ui_gold_island_arm_point = (ui_interface_round_t *)&(ui_gold_island_arm_0.data[3]);

void _ui_init_gold_island_arm_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_gold_island_arm_0.data[i].figure_name[0] = FRAME_ID;
        ui_gold_island_arm_0.data[i].figure_name[1] = GROUP_ID;
        ui_gold_island_arm_0.data[i].figure_name[2] = i + START_ID;
        ui_gold_island_arm_0.data[i].operate_tpyel = 1;
    }
    for (int i = OBJ_NUM; i < FRAME_OBJ_NUM; i++) {
        ui_gold_island_arm_0.data[i].operate_tpyel = 0;
    }

    ui_gold_island_arm_Line1->figure_tpye = 0;
    ui_gold_island_arm_Line1->layer = 0;
    ui_gold_island_arm_Line1->start_x = 104;
    ui_gold_island_arm_Line1->start_y = 359;
    ui_gold_island_arm_Line1->end_x = 104;
    ui_gold_island_arm_Line1->end_y = 700;
    ui_gold_island_arm_Line1->color = 2;
    ui_gold_island_arm_Line1->width = 10;

    ui_gold_island_arm_Line2->figure_tpye = 0;
    ui_gold_island_arm_Line2->layer = 0;
    ui_gold_island_arm_Line2->start_x = 37;
    ui_gold_island_arm_Line2->start_y = 481;
    ui_gold_island_arm_Line2->end_x = 187;
    ui_gold_island_arm_Line2->end_y = 481;
    ui_gold_island_arm_Line2->color = 2;
    ui_gold_island_arm_Line2->width = 10;

    ui_gold_island_arm_Line3->figure_tpye = 0;
    ui_gold_island_arm_Line3->layer = 0;
    ui_gold_island_arm_Line3->start_x = 106;
    ui_gold_island_arm_Line3->start_y = 481;
    ui_gold_island_arm_Line3->end_x = 249;
    ui_gold_island_arm_Line3->end_y = 624;
    ui_gold_island_arm_Line3->color = 2;
    ui_gold_island_arm_Line3->width = 10;

    ui_gold_island_arm_point->figure_tpye = 2;
    ui_gold_island_arm_point->layer = 0;
    ui_gold_island_arm_point->r = 14;
    ui_gold_island_arm_point->start_x = 217;
    ui_gold_island_arm_point->start_y = 588;
    ui_gold_island_arm_point->color = 2;
    ui_gold_island_arm_point->width = 25;


    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_gold_island_arm_0);
    SEND_MESSAGE((uint8_t *) &ui_gold_island_arm_0, sizeof(ui_gold_island_arm_0));
}

void _ui_update_gold_island_arm_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_gold_island_arm_0.data[i].operate_tpyel = 2;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_gold_island_arm_0);
    SEND_MESSAGE((uint8_t *) &ui_gold_island_arm_0, sizeof(ui_gold_island_arm_0));
}

void _ui_remove_gold_island_arm_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_gold_island_arm_0.data[i].operate_tpyel = 3;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_gold_island_arm_0);
    SEND_MESSAGE((uint8_t *) &ui_gold_island_arm_0, sizeof(ui_gold_island_arm_0));
}
