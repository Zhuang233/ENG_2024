//
// Created by RM UI Designer
//

#include "ui_default_menu3_2.h"

#define FRAME_ID 0
#define GROUP_ID 2
#define START_ID 2
#define OBJ_NUM 1
#define FRAME_OBJ_NUM 1

CAT(ui_, CAT(FRAME_OBJ_NUM, _frame_t)) ui_default_menu3_2;
ui_interface_rect_t *ui_default_menu3_select = (ui_interface_rect_t *)&(ui_default_menu3_2.data[0]);

void _ui_init_default_menu3_2() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_default_menu3_2.data[i].figure_name[0] = FRAME_ID;
        ui_default_menu3_2.data[i].figure_name[1] = GROUP_ID;
        ui_default_menu3_2.data[i].figure_name[2] = i + START_ID;
        ui_default_menu3_2.data[i].operate_tpyel = 1;
    }
    for (int i = OBJ_NUM; i < FRAME_OBJ_NUM; i++) {
        ui_default_menu3_2.data[i].operate_tpyel = 0;
    }

    ui_default_menu3_select->figure_tpye = 1;
    ui_default_menu3_select->layer = 3;
    ui_default_menu3_select->start_x = 239;
    ui_default_menu3_select->start_y = 832;
    ui_default_menu3_select->color = 1;
    ui_default_menu3_select->width = 5;
    ui_default_menu3_select->end_x = 339;
    ui_default_menu3_select->end_y = 882;


    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_default_menu3_2);
    SEND_MESSAGE((uint8_t *) &ui_default_menu3_2, sizeof(ui_default_menu3_2));
}

void _ui_update_default_menu3_2() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_default_menu3_2.data[i].operate_tpyel = 2;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_default_menu3_2);
    SEND_MESSAGE((uint8_t *) &ui_default_menu3_2, sizeof(ui_default_menu3_2));
}

void _ui_remove_default_menu3_2() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_default_menu3_2.data[i].operate_tpyel = 3;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_default_menu3_2);
    SEND_MESSAGE((uint8_t *) &ui_default_menu3_2, sizeof(ui_default_menu3_2));
}
