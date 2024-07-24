//
// Created by RM UI Designer
//

#include "ui_default_menu2_2.h"

#define FRAME_ID 0
#define GROUP_ID 1
#define START_ID 2
#define OBJ_NUM 1
#define FRAME_OBJ_NUM 1

CAT(ui_, CAT(FRAME_OBJ_NUM, _frame_t)) ui_default_menu2_2;
ui_interface_rect_t *ui_default_menu2_select = (ui_interface_rect_t *)&(ui_default_menu2_2.data[0]);

void _ui_init_default_menu2_2() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_default_menu2_2.data[i].figure_name[0] = FRAME_ID;
        ui_default_menu2_2.data[i].figure_name[1] = GROUP_ID;
        ui_default_menu2_2.data[i].figure_name[2] = i + START_ID;
        ui_default_menu2_2.data[i].operate_tpyel = 1;
    }
    for (int i = OBJ_NUM; i < FRAME_OBJ_NUM; i++) {
        ui_default_menu2_2.data[i].operate_tpyel = 0;
    }

    ui_default_menu2_select->figure_tpye = 1;
    ui_default_menu2_select->layer = 2;
    ui_default_menu2_select->start_x = 99;
    ui_default_menu2_select->start_y = 727;
    ui_default_menu2_select->color = 1;
    ui_default_menu2_select->width = 5;
    ui_default_menu2_select->end_x = 199;
    ui_default_menu2_select->end_y = 777;


    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_default_menu2_2);
    SEND_MESSAGE((uint8_t *) &ui_default_menu2_2, sizeof(ui_default_menu2_2));
}

void _ui_update_default_menu2_2() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_default_menu2_2.data[i].operate_tpyel = 2;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_default_menu2_2);
    SEND_MESSAGE((uint8_t *) &ui_default_menu2_2, sizeof(ui_default_menu2_2));
}

void _ui_remove_default_menu2_2() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_default_menu2_2.data[i].operate_tpyel = 3;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_default_menu2_2);
    SEND_MESSAGE((uint8_t *) &ui_default_menu2_2, sizeof(ui_default_menu2_2));
}
