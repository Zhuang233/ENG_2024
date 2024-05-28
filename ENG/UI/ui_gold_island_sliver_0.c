//
// Created by RM UI Designer
//

#include "ui_gold_island_sliver_0.h"

#define FRAME_ID 1
#define GROUP_ID 1
#define START_ID 0
#define OBJ_NUM 5
#define FRAME_OBJ_NUM 5

CAT(ui_, CAT(FRAME_OBJ_NUM, _frame_t)) ui_gold_island_sliver_0;
ui_interface_rect_t *ui_gold_island_sliver_NewRect = (ui_interface_rect_t *)&(ui_gold_island_sliver_0.data[0]);
ui_interface_line_t *ui_gold_island_sliver_Line1 = (ui_interface_line_t *)&(ui_gold_island_sliver_0.data[1]);
ui_interface_line_t *ui_gold_island_sliver_Line2 = (ui_interface_line_t *)&(ui_gold_island_sliver_0.data[2]);
ui_interface_line_t *ui_gold_island_sliver_Line3 = (ui_interface_line_t *)&(ui_gold_island_sliver_0.data[3]);
ui_interface_line_t *ui_gold_island_sliver_Line4 = (ui_interface_line_t *)&(ui_gold_island_sliver_0.data[4]);

void _ui_init_gold_island_sliver_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_gold_island_sliver_0.data[i].figure_name[0] = FRAME_ID;
        ui_gold_island_sliver_0.data[i].figure_name[1] = GROUP_ID;
        ui_gold_island_sliver_0.data[i].figure_name[2] = i + START_ID;
        ui_gold_island_sliver_0.data[i].operate_tpyel = 1;
    }
    for (int i = OBJ_NUM; i < FRAME_OBJ_NUM; i++) {
        ui_gold_island_sliver_0.data[i].operate_tpyel = 0;
    }

    ui_gold_island_sliver_NewRect->figure_tpye = 1;
    ui_gold_island_sliver_NewRect->layer = 0;
    ui_gold_island_sliver_NewRect->start_x = 639;
    ui_gold_island_sliver_NewRect->start_y = 824;
    ui_gold_island_sliver_NewRect->color = 1;
    ui_gold_island_sliver_NewRect->width = 5;
    ui_gold_island_sliver_NewRect->end_x = 1057;
    ui_gold_island_sliver_NewRect->end_y = 975;

    ui_gold_island_sliver_Line1->figure_tpye = 0;
    ui_gold_island_sliver_Line1->layer = 0;
    ui_gold_island_sliver_Line1->start_x = 208;
    ui_gold_island_sliver_Line1->start_y = 762;
    ui_gold_island_sliver_Line1->end_x = 228;
    ui_gold_island_sliver_Line1->end_y = 885;
    ui_gold_island_sliver_Line1->color = 1;
    ui_gold_island_sliver_Line1->width = 5;

    ui_gold_island_sliver_Line2->figure_tpye = 0;
    ui_gold_island_sliver_Line2->layer = 0;
    ui_gold_island_sliver_Line2->start_x = 465;
    ui_gold_island_sliver_Line2->start_y = 793;
    ui_gold_island_sliver_Line2->end_x = 481;
    ui_gold_island_sliver_Line2->end_y = 932;
    ui_gold_island_sliver_Line2->color = 1;
    ui_gold_island_sliver_Line2->width = 5;

    ui_gold_island_sliver_Line3->figure_tpye = 0;
    ui_gold_island_sliver_Line3->layer = 0;
    ui_gold_island_sliver_Line3->start_x = 1260;
    ui_gold_island_sliver_Line3->start_y = 818;
    ui_gold_island_sliver_Line3->end_x = 1253;
    ui_gold_island_sliver_Line3->end_y = 950;
    ui_gold_island_sliver_Line3->color = 1;
    ui_gold_island_sliver_Line3->width = 5;

    ui_gold_island_sliver_Line4->figure_tpye = 0;
    ui_gold_island_sliver_Line4->layer = 0;
    ui_gold_island_sliver_Line4->start_x = 1569;
    ui_gold_island_sliver_Line4->start_y = 791;
    ui_gold_island_sliver_Line4->end_x = 1554;
    ui_gold_island_sliver_Line4->end_y = 906;
    ui_gold_island_sliver_Line4->color = 1;
    ui_gold_island_sliver_Line4->width = 5;


    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_gold_island_sliver_0);
    SEND_MESSAGE((uint8_t *) &ui_gold_island_sliver_0, sizeof(ui_gold_island_sliver_0));
}

void _ui_update_gold_island_sliver_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_gold_island_sliver_0.data[i].operate_tpyel = 2;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_gold_island_sliver_0);
    SEND_MESSAGE((uint8_t *) &ui_gold_island_sliver_0, sizeof(ui_gold_island_sliver_0));
}

void _ui_remove_gold_island_sliver_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_gold_island_sliver_0.data[i].operate_tpyel = 3;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_gold_island_sliver_0);
    SEND_MESSAGE((uint8_t *) &ui_gold_island_sliver_0, sizeof(ui_gold_island_sliver_0));
}
