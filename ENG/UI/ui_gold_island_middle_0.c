//
// Created by RM UI Designer
//

#include "ui_gold_island_middle_0.h"

#define FRAME_ID 1
#define GROUP_ID 0
#define START_ID 0
#define OBJ_NUM 4
#define FRAME_OBJ_NUM 5

CAT(ui_, CAT(FRAME_OBJ_NUM, _frame_t)) ui_gold_island_middle_0;

// 将图形对象分配到帧数据段的空间中
ui_interface_rect_t *ui_gold_island_middle_rect = (ui_interface_rect_t *)&(ui_gold_island_middle_0.data[0]);
ui_interface_line_t *ui_gold_island_middle_Line1 = (ui_interface_line_t *)&(ui_gold_island_middle_0.data[1]);
ui_interface_line_t *ui_gold_island_middle_Line2 = (ui_interface_line_t *)&(ui_gold_island_middle_0.data[2]);
ui_interface_line_t *ui_gold_island_middle_Line3 = (ui_interface_line_t *)&(ui_gold_island_middle_0.data[3]);

void _ui_init_gold_island_middle_0() {
		// 给所有对象取名字，定义增加操作
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_gold_island_middle_0.data[i].figure_name[0] = FRAME_ID;
        ui_gold_island_middle_0.data[i].figure_name[1] = GROUP_ID;
        ui_gold_island_middle_0.data[i].figure_name[2] = i + START_ID;
        ui_gold_island_middle_0.data[i].operate_tpyel = 1;
    }
		// 剩下的对象空操作
    for (int i = OBJ_NUM; i < FRAME_OBJ_NUM; i++) {
        ui_gold_island_middle_0.data[i].operate_tpyel = 0;
    }
		
		// 定义所有obj类型和对应属性
    ui_gold_island_middle_rect->figure_tpye = 1;
    ui_gold_island_middle_rect->layer = 0;
    ui_gold_island_middle_rect->start_x = 538;
    ui_gold_island_middle_rect->start_y = 110;
    ui_gold_island_middle_rect->color = 1;
    ui_gold_island_middle_rect->width = 5;
    ui_gold_island_middle_rect->end_x = 1137;
    ui_gold_island_middle_rect->end_y = 711;

    ui_gold_island_middle_Line1->figure_tpye = 0;
    ui_gold_island_middle_Line1->layer = 0;
    ui_gold_island_middle_Line1->start_x = 539;
    ui_gold_island_middle_Line1->start_y = 711;
    ui_gold_island_middle_Line1->end_x = 795;
    ui_gold_island_middle_Line1->end_y = 619;
    ui_gold_island_middle_Line1->color = 1;
    ui_gold_island_middle_Line1->width = 5;

    ui_gold_island_middle_Line2->figure_tpye = 0;
    ui_gold_island_middle_Line2->layer = 0;
    ui_gold_island_middle_Line2->start_x = 1139;
    ui_gold_island_middle_Line2->start_y = 710;
    ui_gold_island_middle_Line2->end_x = 1033;
    ui_gold_island_middle_Line2->end_y = 628;
    ui_gold_island_middle_Line2->color = 1;
    ui_gold_island_middle_Line2->width = 5;

    ui_gold_island_middle_Line3->figure_tpye = 0;
    ui_gold_island_middle_Line3->layer = 0;
    ui_gold_island_middle_Line3->start_x = 1060;
    ui_gold_island_middle_Line3->start_y = 341;
    ui_gold_island_middle_Line3->end_x = 1137;
    ui_gold_island_middle_Line3->end_y = 110;
    ui_gold_island_middle_Line3->color = 1;
    ui_gold_island_middle_Line3->width = 5;


    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_gold_island_middle_0);
    SEND_MESSAGE((uint8_t *) &ui_gold_island_middle_0, sizeof(ui_gold_island_middle_0));
}

void _ui_update_gold_island_middle_0() {
		//定义修改操作
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_gold_island_middle_0.data[i].operate_tpyel = 2;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_gold_island_middle_0);
    SEND_MESSAGE((uint8_t *) &ui_gold_island_middle_0, sizeof(ui_gold_island_middle_0));
}

void _ui_remove_gold_island_middle_0() {
	  //定义删除操作
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_gold_island_middle_0.data[i].operate_tpyel = 3;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_gold_island_middle_0);
    SEND_MESSAGE((uint8_t *) &ui_gold_island_middle_0, sizeof(ui_gold_island_middle_0));
}
