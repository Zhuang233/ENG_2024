//
// Created by RM UI Designer
//

#include "ui_default_Sliver_0.h"

#define FRAME_ID 1
#define GROUP_ID 0
#define START_ID 0
#define OBJ_NUM 5
#define FRAME_OBJ_NUM 5

CAT(ui_, CAT(FRAME_OBJ_NUM, _frame_t)) ui_default_Sliver_0;
ui_interface_line_t *ui_default_Sliver_SliverLine1 = (ui_interface_line_t *)&(ui_default_Sliver_0.data[0]);
ui_interface_line_t *ui_default_Sliver_SliverLine2 = (ui_interface_line_t *)&(ui_default_Sliver_0.data[1]);
ui_interface_line_t *ui_default_Sliver_SliverLine3 = (ui_interface_line_t *)&(ui_default_Sliver_0.data[2]);
ui_interface_line_t *ui_default_Sliver_SliverLine4 = (ui_interface_line_t *)&(ui_default_Sliver_0.data[3]);
ui_interface_line_t *ui_default_Sliver_SliverLine5 = (ui_interface_line_t *)&(ui_default_Sliver_0.data[4]);

void _ui_init_default_Sliver_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_default_Sliver_0.data[i].figure_name[0] = FRAME_ID;
        ui_default_Sliver_0.data[i].figure_name[1] = GROUP_ID;
        ui_default_Sliver_0.data[i].figure_name[2] = i + START_ID;
        ui_default_Sliver_0.data[i].operate_tpyel = 1;
    }
    for (int i = OBJ_NUM; i < FRAME_OBJ_NUM; i++) {
        ui_default_Sliver_0.data[i].operate_tpyel = 0;
    }

    ui_default_Sliver_SliverLine1->figure_tpye = 0;
    ui_default_Sliver_SliverLine1->layer = 0;
    ui_default_Sliver_SliverLine1->start_x = 1449;
    ui_default_Sliver_SliverLine1->start_y = 629;
    ui_default_Sliver_SliverLine1->end_x = 1531;
    ui_default_Sliver_SliverLine1->end_y = 769;
    ui_default_Sliver_SliverLine1->color = 1;
    ui_default_Sliver_SliverLine1->width = 5;

    ui_default_Sliver_SliverLine2->figure_tpye = 0;
    ui_default_Sliver_SliverLine2->layer = 0;
    ui_default_Sliver_SliverLine2->start_x = 283;
    ui_default_Sliver_SliverLine2->start_y = 627;
    ui_default_Sliver_SliverLine2->end_x = 589;
    ui_default_Sliver_SliverLine2->end_y = 507;
    ui_default_Sliver_SliverLine2->color = 1;
    ui_default_Sliver_SliverLine2->width = 5;

    ui_default_Sliver_SliverLine3->figure_tpye = 0;
    ui_default_Sliver_SliverLine3->layer = 0;
    ui_default_Sliver_SliverLine3->start_x = 589;
    ui_default_Sliver_SliverLine3->start_y = 507;
    ui_default_Sliver_SliverLine3->end_x = 718;
    ui_default_Sliver_SliverLine3->end_y = 200;
    ui_default_Sliver_SliverLine3->color = 1;
    ui_default_Sliver_SliverLine3->width = 5;

    ui_default_Sliver_SliverLine4->figure_tpye = 0;
    ui_default_Sliver_SliverLine4->layer = 0;
    ui_default_Sliver_SliverLine4->start_x = 589;
    ui_default_Sliver_SliverLine4->start_y = 507;
    ui_default_Sliver_SliverLine4->end_x = 1449;
    ui_default_Sliver_SliverLine4->end_y = 629;
    ui_default_Sliver_SliverLine4->color = 1;
    ui_default_Sliver_SliverLine4->width = 5;

    ui_default_Sliver_SliverLine5->figure_tpye = 0;
    ui_default_Sliver_SliverLine5->layer = 0;
    ui_default_Sliver_SliverLine5->start_x = 1449;
    ui_default_Sliver_SliverLine5->start_y = 630;
    ui_default_Sliver_SliverLine5->end_x = 1329;
    ui_default_Sliver_SliverLine5->end_y = 347;
    ui_default_Sliver_SliverLine5->color = 1;
    ui_default_Sliver_SliverLine5->width = 5;


    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_default_Sliver_0);
    SEND_MESSAGE((uint8_t *) &ui_default_Sliver_0, sizeof(ui_default_Sliver_0));
}

void _ui_update_default_Sliver_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_default_Sliver_0.data[i].operate_tpyel = 2;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_default_Sliver_0);
    SEND_MESSAGE((uint8_t *) &ui_default_Sliver_0, sizeof(ui_default_Sliver_0));
}

void _ui_remove_default_Sliver_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_default_Sliver_0.data[i].operate_tpyel = 3;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_default_Sliver_0);
    
    SEND_MESSAGE((uint8_t *) &ui_default_Sliver_0, sizeof(ui_default_Sliver_0));
}
