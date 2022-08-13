#ifndef TELEPHONY_H
#define TELEPHONY_H

#ifdef __cplusplus
extern "C" {
#endif
#include "lvgl.h"

void create_network(lv_obj_t * parent);
void create_msg(lv_obj_t * parent);
void create_dial(lv_obj_t * parent);
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
