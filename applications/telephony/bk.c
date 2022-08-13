/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2021-10-17     Meco Man      First version
 */
#include <rtthread.h>
#include <lvgl.h>
#define DBG_TAG    "LVGL"
#define DBG_LVL    DBG_INFO
#include <rtdbg.h>
#include "telephony.h"

#ifndef LV_THREAD_STACK_SIZE
    #define LV_THREAD_STACK_SIZE 4096
#endif

#ifndef LV_THREAD_PRIO
    #define LV_THREAD_PRIO (RT_THREAD_PRIORITY_MAX*2/3)
#endif

static lv_obj_t * tv;
static lv_obj_t * t_dial;
static lv_obj_t * t_msm;
static lv_obj_t * t_data;
static lv_obj_t * t_network;

static void lv_bk_widgets(void)
{
    tv = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, LV_DPI_DEF / 3);

    t_dial = lv_tabview_add_tab(tv, "Dial");
    t_msm = lv_tabview_add_tab(tv, "MSM");
    // t_data = lv_tabview_add_tab(tv, "Data");
    t_network = lv_tabview_add_tab(tv, "Network");


    create_network(t_network);
    create_msg(t_msm); //TODO
    create_dial(t_dial);

}


static void lvgl_thread(void *parameter)
{
    lv_bk_widgets();

    while (1)
    {
        lv_task_handler();
        rt_thread_mdelay(1);
    }
}

static int telephony_demo(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("telephony", lvgl_thread, RT_NULL, LV_THREAD_STACK_SIZE, LV_THREAD_PRIO, 10);
    if (tid == RT_NULL)
    {
        LOG_E("Fail to create 'telephony' thread");
    }
    rt_thread_startup(tid);

    return 0;
}
// INIT_APP_EXPORT(telephony_demo);
MSH_CMD_EXPORT(telephony_demo, tele demo init);