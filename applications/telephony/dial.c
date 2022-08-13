#include "telephony.h"
#include <rtthread.h>
#include <at.h>
#include <stdio.h>
#define DBG_TAG    "Dial"
#define DBG_LVL    DBG_LOG
#include <rtdbg.h>
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void ta_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * tv;
lv_obj_t * ta1;


static int dial_number(const char *number)
{
    at_response_t resp = RT_NULL;
    char cmd[64];
    resp = at_create_resp(512, 0, rt_tick_from_millisecond(5000));
    if (!resp)
    {
        LOG_E("No memory for response structure!");
        return -1;
    }
    rt_snprintf(cmd, 64, "ATD%s", number);
    if (at_exec_cmd(resp, cmd) != RT_EOK)
    {
        LOG_E("AT client send commands failed, response error or timeout !");
        return -2;
    }
    LOG_D("AT Client send commands to AT Server success!");
    // at_resp_parse_line_args(resp, 2,"%s", &iccid);
    // LOG_D("iccid: %s", iccid);
    // set_ICCID(iccid);

    at_delete_resp(resp);

    return RT_EOK;
}

static int hung_up()
{
    at_response_t resp = RT_NULL;

    resp = at_create_resp(512, 0, rt_tick_from_millisecond(5000));
    if (!resp)
    {
        LOG_E("No memory for response structure!");
        return -1;
    }

    if (at_exec_cmd(resp, "ATH") != RT_EOK)
    {
        LOG_E("AT client send commands failed, response error or timeout !");
        return -2;
    }
    LOG_D("AT Client send commands to AT Server success!");
    // at_resp_parse_line_args(resp, 2,"%s", &iccid);
    // LOG_D("iccid: %s", iccid);
    // set_ICCID(iccid);

    at_delete_resp(resp);

    return RT_EOK;
}
static void dial_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Dial to %s", lv_textarea_get_text(ta1));
        dial_number(lv_textarea_get_text(ta1));
        lv_obj_t * label = lv_label_create(lv_scr_act());
        lv_label_set_text(label, "Dial success");
        lv_obj_center(label);
        rt_thread_delay(2000);
        LV_LOG_USER("Dial success");
        lv_label_set_text(label, "");

    }
    
}

static void hang_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("send");
        lv_obj_t * label = lv_label_create(lv_scr_act());
        hung_up();
        lv_label_set_text(label, "Hung up");
        lv_obj_center(label);
        rt_thread_delay(2000);
        LV_LOG_USER("Hung success");
        lv_label_set_text(label, "");

    }
    
}

void create_dial(lv_obj_t * parent)
{
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    tv = parent;
    ta1 = lv_textarea_create(parent);
    lv_obj_set_width(ta1, LV_PCT(100));
    lv_textarea_set_one_line(ta1, true);
    lv_textarea_set_accepted_chars(ta1, "0123456789");
    lv_textarea_set_max_length(ta1, 11);
    lv_textarea_set_placeholder_text(ta1, "Contact");

    lv_obj_t *kb = lv_keyboard_create(lv_scr_act());
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    lv_keyboard_set_mode(kb,LV_KEYBOARD_MODE_NUMBER);

    lv_obj_add_event_cb(ta1, ta_event_cb, LV_EVENT_ALL, kb);



    lv_obj_t *sendBtn = lv_btn_create(parent);
    lv_obj_add_event_cb(sendBtn, dial_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(sendBtn, LV_ALIGN_CENTER, 0, -40);
    lv_obj_t * label = lv_label_create(sendBtn);
    lv_label_set_text(label, "Dial");
    lv_obj_center(label);

    lv_obj_t *hungBtn = lv_btn_create(parent);
    lv_obj_add_event_cb(hungBtn, hang_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(hungBtn, LV_ALIGN_CENTER, 0, -40);
    lv_obj_t * label_hung = lv_label_create(hungBtn);
    lv_label_set_text(label_hung, "Hung");
    lv_obj_center(label_hung);
}


static void ta_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = lv_event_get_user_data(e);
    if(code == LV_EVENT_FOCUSED) {
        if(lv_indev_get_type(lv_indev_get_act()) != LV_INDEV_TYPE_KEYPAD) {
            lv_keyboard_set_textarea(kb, ta);
            lv_obj_set_style_max_height(kb, LV_HOR_RES * 2 / 3, 0);
            lv_obj_update_layout(tv);   /*Be sure the sizes are recalculated*/
            lv_obj_set_height(tv, LV_VER_RES - lv_obj_get_height(kb));
            lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
            lv_obj_scroll_to_view_recursive(ta, LV_ANIM_OFF);
        }
    }
    else if(code == LV_EVENT_DEFOCUSED) {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_set_height(tv, LV_VER_RES);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_indev_reset(NULL, ta);

    }
    else if(code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
        lv_obj_set_height(tv, LV_VER_RES);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_state(ta, LV_STATE_FOCUSED);
        lv_indev_reset(NULL, ta);   /*To forget the last clicked object to make it focusable again*/
    }
}
