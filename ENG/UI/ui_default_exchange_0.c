//
// Created by RM UI Designer
//

#include "ui_default_exchange_0.h"

#define FRAME_ID 0
#define GROUP_ID 4
#define START_ID 0
#define OBJ_NUM 7
#define FRAME_OBJ_NUM 7

CAT(ui_, CAT(FRAME_OBJ_NUM, _frame_t)) ui_default_exchange_0;
ui_interface_round_t *ui_default_exchange_leftRound = (ui_interface_round_t *)&(ui_default_exchange_0.data[0]);
ui_interface_round_t *ui_default_exchange_rightRound = (ui_interface_round_t *)&(ui_default_exchange_0.data[1]);
ui_interface_line_t *ui_default_exchange_rightLine = (ui_interface_line_t *)&(ui_default_exchange_0.data[2]);
ui_interface_line_t *ui_default_exchange_leftLine = (ui_interface_line_t *)&(ui_default_exchange_0.data[3]);
ui_interface_line_t *ui_default_exchange_topLine = (ui_interface_line_t *)&(ui_default_exchange_0.data[4]);
ui_interface_line_t *ui_default_exchange_rlimitLine = (ui_interface_line_t *)&(ui_default_exchange_0.data[5]);
ui_interface_line_t *ui_default_exchange_llimitLine = (ui_interface_line_t *)&(ui_default_exchange_0.data[6]);

void _ui_init_default_exchange_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_default_exchange_0.data[i].figure_name[0] = FRAME_ID;
        ui_default_exchange_0.data[i].figure_name[1] = GROUP_ID;
        ui_default_exchange_0.data[i].figure_name[2] = i + START_ID;
        ui_default_exchange_0.data[i].operate_tpyel = 1;
    }
    for (int i = OBJ_NUM; i < FRAME_OBJ_NUM; i++) {
        ui_default_exchange_0.data[i].operate_tpyel = 0;
    }

    ui_default_exchange_leftRound->figure_tpye = 2;
    ui_default_exchange_leftRound->layer = 4;
    ui_default_exchange_leftRound->r = 190;
    ui_default_exchange_leftRound->start_x = 529;
    ui_default_exchange_leftRound->start_y = 585;
    ui_default_exchange_leftRound->color = 2;
    ui_default_exchange_leftRound->width = 3;

    ui_default_exchange_rightRound->figure_tpye = 2;
    ui_default_exchange_rightRound->layer = 4;
    ui_default_exchange_rightRound->r = 167;
    ui_default_exchange_rightRound->start_x = 1142;
    ui_default_exchange_rightRound->start_y = 603;
    ui_default_exchange_rightRound->color = 2;
    ui_default_exchange_rightRound->width = 3;

    ui_default_exchange_rightLine->figure_tpye = 0;
    ui_default_exchange_rightLine->layer = 4;
    ui_default_exchange_rightLine->start_x = 1142;
    ui_default_exchange_rightLine->start_y = 128;
    ui_default_exchange_rightLine->end_x = 1142;
    ui_default_exchange_rightLine->end_y = 945;
    ui_default_exchange_rightLine->color = 2;
    ui_default_exchange_rightLine->width = 3;

    ui_default_exchange_leftLine->figure_tpye = 0;
    ui_default_exchange_leftLine->layer = 4;
    ui_default_exchange_leftLine->start_x = 529;
    ui_default_exchange_leftLine->start_y = 117;
    ui_default_exchange_leftLine->end_x = 529;
    ui_default_exchange_leftLine->end_y = 934;
    ui_default_exchange_leftLine->color = 2;
    ui_default_exchange_leftLine->width = 3;

    ui_default_exchange_topLine->figure_tpye = 0;
    ui_default_exchange_topLine->layer = 5;
    ui_default_exchange_topLine->start_x = 0;
    ui_default_exchange_topLine->start_y = 0;
    ui_default_exchange_topLine->end_x = 0;
    ui_default_exchange_topLine->end_y = 0;
    ui_default_exchange_topLine->color = 5;
    ui_default_exchange_topLine->width = 15;

    ui_default_exchange_rlimitLine->figure_tpye = 0;
    ui_default_exchange_rlimitLine->layer = 5;
    ui_default_exchange_rlimitLine->start_x = 0;
    ui_default_exchange_rlimitLine->start_y = 0;
    ui_default_exchange_rlimitLine->end_x = 0;
    ui_default_exchange_rlimitLine->end_y = 0;
    ui_default_exchange_rlimitLine->color = 5;
    ui_default_exchange_rlimitLine->width = 15;

    ui_default_exchange_llimitLine->figure_tpye = 0;
    ui_default_exchange_llimitLine->layer = 5;
    ui_default_exchange_llimitLine->start_x = 0;
    ui_default_exchange_llimitLine->start_y = 0;
    ui_default_exchange_llimitLine->end_x = 0;
    ui_default_exchange_llimitLine->end_y = 0;
    ui_default_exchange_llimitLine->color = 5;
    ui_default_exchange_llimitLine->width = 15;


    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_default_exchange_0);
    SEND_MESSAGE((uint8_t *) &ui_default_exchange_0, sizeof(ui_default_exchange_0));
}

void _ui_update_default_exchange_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_default_exchange_0.data[i].operate_tpyel = 2;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_default_exchange_0);
    SEND_MESSAGE((uint8_t *) &ui_default_exchange_0, sizeof(ui_default_exchange_0));
}

void _ui_remove_default_exchange_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_default_exchange_0.data[i].operate_tpyel = 3;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_default_exchange_0);
    SEND_MESSAGE((uint8_t *) &ui_default_exchange_0, sizeof(ui_default_exchange_0));
}
