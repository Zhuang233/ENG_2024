//
// Created by RM UI Designer
//

#include "ui_gold_island_Ungroup_0.h"

#define FRAME_ID 1
#define GROUP_ID 2
#define START_ID 0
#define OBJ_NUM 1
#define FRAME_OBJ_NUM 1

CAT(ui_, CAT(FRAME_OBJ_NUM, _frame_t)) ui_gold_island_Ungroup_0;
ui_interface_round_t *ui_gold_island_Ungroup_NewRound = (ui_interface_round_t *)&(ui_gold_island_Ungroup_0.data[0]);

void _ui_init_gold_island_Ungroup_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_gold_island_Ungroup_0.data[i].figure_name[0] = FRAME_ID;
        ui_gold_island_Ungroup_0.data[i].figure_name[1] = GROUP_ID;
        ui_gold_island_Ungroup_0.data[i].figure_name[2] = i + START_ID;
        ui_gold_island_Ungroup_0.data[i].operate_tpyel = 1;
    }
    for (int i = OBJ_NUM; i < FRAME_OBJ_NUM; i++) {
        ui_gold_island_Ungroup_0.data[i].operate_tpyel = 0;
    }

    ui_gold_island_Ungroup_NewRound->figure_tpye = 2;
    ui_gold_island_Ungroup_NewRound->layer = 0;
    ui_gold_island_Ungroup_NewRound->r = 33;
    ui_gold_island_Ungroup_NewRound->start_x = 955;
    ui_gold_island_Ungroup_NewRound->start_y = 534;
    ui_gold_island_Ungroup_NewRound->color = 0;
    ui_gold_island_Ungroup_NewRound->width = 15;


    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_gold_island_Ungroup_0);
    SEND_MESSAGE((uint8_t *) &ui_gold_island_Ungroup_0, sizeof(ui_gold_island_Ungroup_0));
}

void _ui_update_gold_island_Ungroup_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_gold_island_Ungroup_0.data[i].operate_tpyel = 2;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_gold_island_Ungroup_0);
    SEND_MESSAGE((uint8_t *) &ui_gold_island_Ungroup_0, sizeof(ui_gold_island_Ungroup_0));
}

void _ui_remove_gold_island_Ungroup_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_gold_island_Ungroup_0.data[i].operate_tpyel = 3;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_gold_island_Ungroup_0);
    SEND_MESSAGE((uint8_t *) &ui_gold_island_Ungroup_0, sizeof(ui_gold_island_Ungroup_0));
}
