#include "telephony.h"
#include <rtthread.h>
#include <at.h>
#include <stdio.h>

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

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("send");
        lv_obj_t * label = lv_label_create(lv_scr_act());
        lv_label_set_text(label, "Success");
        lv_obj_center(label);
        rt_thread_delay(2000);
        LV_LOG_USER("send success");
        lv_label_set_text(label, "");

    }
    
}

void create_msg(lv_obj_t * parent)
{
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    tv = parent;
    lv_obj_t * ta1 = lv_textarea_create(parent);
    lv_obj_set_width(ta1, LV_PCT(100));
    lv_textarea_set_one_line(ta1, true);
    lv_textarea_set_placeholder_text(ta1, "contector");

    lv_obj_t * ta2 = lv_textarea_create(parent);
    lv_obj_set_width(ta2, LV_PCT(100));
    // lv_textarea_set_one_line(ta2, true);
    lv_textarea_set_placeholder_text(ta2, "Your message");

    lv_obj_t *kb = lv_keyboard_create(lv_scr_act());
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);

    lv_obj_add_event_cb(ta1, ta_event_cb, LV_EVENT_ALL, kb);
    lv_obj_add_event_cb(ta2, ta_event_cb, LV_EVENT_ALL, kb);


    lv_obj_t *sendBtn = lv_btn_create(parent);
    lv_obj_add_event_cb(sendBtn, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(sendBtn, LV_ALIGN_CENTER, 0, -40);
    lv_obj_t * label = lv_label_create(sendBtn);
    lv_label_set_text(label, "Send");
    lv_obj_center(label);
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
