//
// Created by RM UI Designer
//

#include "ui_default_GOLD_0.h"

#define FRAME_ID 0
#define GROUP_ID 3
#define START_ID 0
#define OBJ_NUM 5
#define FRAME_OBJ_NUM 5

CAT(ui_, CAT(FRAME_OBJ_NUM, _frame_t)) ui_default_GOLD_0;
ui_interface_line_t *ui_default_GOLD_Line1 = (ui_interface_line_t *)&(ui_default_GOLD_0.data[0]);
ui_interface_line_t *ui_default_GOLD_Line2 = (ui_interface_line_t *)&(ui_default_GOLD_0.data[1]);
ui_interface_line_t *ui_default_GOLD_Line3 = (ui_interface_line_t *)&(ui_default_GOLD_0.data[2]);
ui_interface_line_t *ui_default_GOLD_Line4 = (ui_interface_line_t *)&(ui_default_GOLD_0.data[3]);
ui_interface_line_t *ui_default_GOLD_Line5 = (ui_interface_line_t *)&(ui_default_GOLD_0.data[4]);

void _ui_init_default_GOLD_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_default_GOLD_0.data[i].figure_name[0] = FRAME_ID;
        ui_default_GOLD_0.data[i].figure_name[1] = GROUP_ID;
        ui_default_GOLD_0.data[i].figure_name[2] = i + START_ID;
        ui_default_GOLD_0.data[i].operate_tpyel = 1;
    }
    for (int i = OBJ_NUM; i < FRAME_OBJ_NUM; i++) {
        ui_default_GOLD_0.data[i].operate_tpyel = 0;
    }

    ui_default_GOLD_Line1->figure_tpye = 0;
    ui_default_GOLD_Line1->layer = 0;
    ui_default_GOLD_Line1->start_x = 580;
    ui_default_GOLD_Line1->start_y = 562;
    ui_default_GOLD_Line1->end_x = 659;
    ui_default_GOLD_Line1->end_y = 346;
    ui_default_GOLD_Line1->color = 1;
    ui_default_GOLD_Line1->width = 5;

    ui_default_GOLD_Line2->figure_tpye = 0;
    ui_default_GOLD_Line2->layer = 0;
    ui_default_GOLD_Line2->start_x = 1762;
    ui_default_GOLD_Line2->start_y = 700;
    ui_default_GOLD_Line2->end_x = 1687;
    ui_default_GOLD_Line2->end_y = 552;
    ui_default_GOLD_Line2->color = 1;
    ui_default_GOLD_Line2->width = 5;

    ui_default_GOLD_Line3->figure_tpye = 0;
    ui_default_GOLD_Line3->layer = 0;
    ui_default_GOLD_Line3->start_x = 580;
    ui_default_GOLD_Line3->start_y = 562;
    ui_default_GOLD_Line3->end_x = 1762;
    ui_default_GOLD_Line3->end_y = 699;
    ui_default_GOLD_Line3->color = 1;
    ui_default_GOLD_Line3->width = 5;

    ui_default_GOLD_Line4->figure_tpye = 0;
    ui_default_GOLD_Line4->layer = 0;
    ui_default_GOLD_Line4->start_x = 1766;
    ui_default_GOLD_Line4->start_y = 700;
    ui_default_GOLD_Line4->end_x = 1528;
    ui_default_GOLD_Line4->end_y = 845;
    ui_default_GOLD_Line4->color = 1;
    ui_default_GOLD_Line4->width = 5;

    ui_default_GOLD_Line5->figure_tpye = 0;
    ui_default_GOLD_Line5->layer = 0;
    ui_default_GOLD_Line5->start_x = 635;
    ui_default_GOLD_Line5->start_y = 813;
    ui_default_GOLD_Line5->end_x = 580;
    ui_default_GOLD_Line5->end_y = 562;
    ui_default_GOLD_Line5->color = 1;
    ui_default_GOLD_Line5->width = 5;


    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_default_GOLD_0);
    SEND_MESSAGE((uint8_t *) &ui_default_GOLD_0, sizeof(ui_default_GOLD_0));
    
}

void _ui_update_default_GOLD_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_default_GOLD_0.data[i].operate_tpyel = 2;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_default_GOLD_0);
    SEND_MESSAGE((uint8_t *) &ui_default_GOLD_0, sizeof(ui_default_GOLD_0));
}

void _ui_remove_default_GOLD_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_default_GOLD_0.data[i].operate_tpyel = 3;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_default_GOLD_0);
    SEND_MESSAGE((uint8_t *) &ui_default_GOLD_0, sizeof(ui_default_GOLD_0));
}
