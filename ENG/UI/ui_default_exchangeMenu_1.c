//
// Created by RM UI Designer
//

#include "ui_default_exchangeMenu_1.h"

#define FRAME_ID 0
#define GROUP_ID 0
#define START_ID 1
#define OBJ_NUM 1
#define FRAME_OBJ_NUM 1

CAT(ui_, CAT(FRAME_OBJ_NUM, _frame_t)) ui_default_exchangeMenu_1;
ui_interface_rect_t *ui_default_exchangeMenu_selectexchange = (ui_interface_rect_t *)&(ui_default_exchangeMenu_1.data[0]);

void _ui_init_default_exchangeMenu_1() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_default_exchangeMenu_1.data[i].figure_name[0] = FRAME_ID;
        ui_default_exchangeMenu_1.data[i].figure_name[1] = GROUP_ID;
        ui_default_exchangeMenu_1.data[i].figure_name[2] = i + START_ID;
        ui_default_exchangeMenu_1.data[i].operate_tpyel = 1;
    }
    for (int i = OBJ_NUM; i < FRAME_OBJ_NUM; i++) {
        ui_default_exchangeMenu_1.data[i].operate_tpyel = 0;
    }

    ui_default_exchangeMenu_selectexchange->figure_tpye = 1;
    ui_default_exchangeMenu_selectexchange->layer = 6;
    ui_default_exchangeMenu_selectexchange->start_x = 34;
    ui_default_exchangeMenu_selectexchange->start_y = 755;
    ui_default_exchangeMenu_selectexchange->color = 1;
    ui_default_exchangeMenu_selectexchange->width = 5;
    ui_default_exchangeMenu_selectexchange->end_x = 106;
    ui_default_exchangeMenu_selectexchange->end_y = 805;


    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_default_exchangeMenu_1);
    SEND_MESSAGE((uint8_t *) &ui_default_exchangeMenu_1, sizeof(ui_default_exchangeMenu_1));
}

void _ui_update_default_exchangeMenu_1() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_default_exchangeMenu_1.data[i].operate_tpyel = 2;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_default_exchangeMenu_1);
    SEND_MESSAGE((uint8_t *) &ui_default_exchangeMenu_1, sizeof(ui_default_exchangeMenu_1));
}

void _ui_remove_default_exchangeMenu_1() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_default_exchangeMenu_1.data[i].operate_tpyel = 3;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_default_exchangeMenu_1);
    SEND_MESSAGE((uint8_t *) &ui_default_exchangeMenu_1, sizeof(ui_default_exchangeMenu_1));
}
