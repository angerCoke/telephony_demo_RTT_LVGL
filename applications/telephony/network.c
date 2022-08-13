#include "telephony.h"
#include <rtthread.h>
#include <at.h>
#include <stdio.h>

#define DBG_TAG    "Network"
#define DBG_LVL    DBG_LOG
#include <rtdbg.h>

lv_obj_t * mcc_label;
lv_obj_t * mnc_label;
lv_obj_t * imei_label;
lv_obj_t * imsi_label;
lv_obj_t * iccid_label;
lv_obj_t * cid_label;
lv_obj_t * signal_strength;

static void network_test(void *parameter);


static void set_strength(int32_t strength)
{
    lv_bar_set_value(signal_strength, strength, LV_ANIM_ON);
}

static void set_MCC(char *mcc)
{
    lv_label_set_text_fmt(mcc_label, mcc);
}
static void set_MNC(char *mnc)
{
    lv_label_set_text_fmt(mnc_label, mnc);
}
static void set_IMEI(char *imei)
{
    lv_label_set_text_fmt(imei_label, imei);
}
static void set_IMSI(char *imsi)
{
    lv_label_set_text_fmt(imsi_label, imsi);
}
static void set_ICCID(char *iccid)
{
    lv_label_set_text_fmt(iccid_label, iccid);
}
static void set_CID(char *cid)
{
    lv_label_set_text_fmt(cid_label, cid);
}

static int refresh_strength()
{
    at_response_t resp = RT_NULL;
    char imei[64];
    resp = at_create_resp(512, 0, rt_tick_from_millisecond(5000));
    if (!resp)
    {
        LOG_E("No memory for response structure!");
        return -1;
    }

    if (at_exec_cmd(resp, "AT+CSQ") != RT_EOK)
    {
        LOG_E("AT client send commands failed, response error or timeout !");
        return -2;
    }
    LOG_D("AT Client send commands to AT Server success!, resp buf: %s", resp->buf);
    at_resp_parse_line_args(resp, 2,"%s", &imei);
    LOG_D("imei: %s", imei);
    set_IMEI(imei);

    at_delete_resp(resp);

    return RT_EOK;
}


static int refresh_imei()
{
    at_response_t resp = RT_NULL;
    char imei[64];
    resp = at_create_resp(512, 0, rt_tick_from_millisecond(5000));
    if (!resp)
    {
        LOG_E("No memory for response structure!");
        return -1;
    }

    if (at_exec_cmd(resp, "AT+CGSN") != RT_EOK)
    {
        LOG_E("AT client send commands failed, response error or timeout !");
        return -2;
    }
    LOG_D("AT Client send commands to AT Server success!, resp buf: %s", resp->buf);
    at_resp_parse_line_args(resp, 2,"%s", &imei);
    LOG_D("imei: %s", imei);
    set_IMEI(imei);

    at_delete_resp(resp);

    return RT_EOK;
}

static int refresh_imsi()
{
    at_response_t resp = RT_NULL;
    char imsi[64];
    resp = at_create_resp(512, 0, rt_tick_from_millisecond(5000));
    if (!resp)
    {
        LOG_E("No memory for response structure!");
        return -1;
    }

    if (at_exec_cmd(resp, "AT+CIMI") != RT_EOK)
    {
        LOG_E("AT client send commands failed, response error or timeout !");
        return -2;
    }
    LOG_D("AT Client send commands to AT Server success!");
    at_resp_parse_line_args(resp, 2,"%s", &imsi);
    LOG_D("imsi: %s", imsi);
    set_IMSI(imsi);

    at_delete_resp(resp);

    return RT_EOK;
}
static int refresh_iccid()
{
    at_response_t resp = RT_NULL;
    char iccid[64];
    resp = at_create_resp(512, 0, rt_tick_from_millisecond(5000));
    if (!resp)
    {
        LOG_E("No memory for response structure!");
        return -1;
    }

    if (at_exec_cmd(resp, "AT+CCID") != RT_EOK)
    {
        LOG_E("AT client send commands failed, response error or timeout !");
        return -2;
    }
    LOG_D("AT Client send commands to AT Server success!");
    at_resp_parse_line_args(resp, 2,"%s", &iccid);
    LOG_D("iccid: %s", iccid);
    set_ICCID(iccid);

    at_delete_resp(resp);

    return RT_EOK;
}
static int refresh_plmn()
{
    at_response_t resp = RT_NULL;
    char plmn[64];
    int mode;
    int reg_status;
    int tech;
    char mcc[4] = {0};
    char mnc[3] = {0};
    resp = at_create_resp(512, 0, rt_tick_from_millisecond(5000));
    if (!resp)
    {
        LOG_E("No memory for response structure!");
        return -1;
    }
    

    if (at_exec_cmd(resp, "AT+COPS?") != RT_EOK)
    {
        LOG_E("AT client send commands failed, response error or timeout !");
        return -2;
    }
    LOG_D("AT Client send commands to AT Server success!");
    at_resp_parse_line_args(resp, 2,"+COPS: %d,%d,%s,%d", &mode, &reg_status, plmn, &tech);

    rt_strncpy(mcc, plmn+1, 3);
    rt_strncpy(mnc, plmn+4, 2);

    set_MCC(mcc);
    set_MNC(mnc);

    at_delete_resp(resp);

    return RT_EOK;
}

static int refresh_cid()
{
    at_response_t resp = RT_NULL;
    char cid[64];
    char tac[64];
    int n;
    int state;
    int tech;
    resp = at_create_resp(512, 0, rt_tick_from_millisecond(5000));
    if (!resp)
    {
        LOG_E("No memory for response structure!");
        return -1;
    }

    if (at_exec_cmd(resp, "AT+CEREG=2") != RT_EOK)
    {
        LOG_E("AT client send commands failed, response error or timeout !");
        return -2;
    }

    if (at_exec_cmd(resp, "AT+CEREG?") != RT_EOK)
    {
        LOG_E("AT client send commands failed, response error or timeout !");
        return -2;
    }
    LOG_D("AT Client send commands to AT Server success!");
    at_resp_parse_line_args_by_kw(resp, "+CEREG","%*[^=]: %d,%d,%s,%s,%d", &n, &state, tac, cid, &tech);


    LOG_D("+CEREG: %d,%d,%s,%s,%d", n, state, tac, cid, tech);
    set_CID(cid);

    at_delete_resp(resp);

    return RT_EOK;
}

static lv_obj_t* signal_bar_create(lv_obj_t * parent)
{
    static lv_style_t style_indic;

    lv_style_init(&style_indic);
    lv_style_set_bg_opa(&style_indic, LV_OPA_COVER);
    lv_style_set_bg_color(&style_indic, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_bg_grad_color(&style_indic, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_bg_grad_dir(&style_indic, LV_GRAD_DIR_VER);

    lv_obj_t * bar = lv_bar_create(parent);
    lv_obj_add_style(bar, &style_indic, LV_PART_INDICATOR);
    lv_obj_set_size(bar, 20, 200);
    lv_obj_center(bar);
    lv_bar_set_range(bar, 0, 100);

    return bar;
}

void create_network(lv_obj_t * parent)
{
    lv_obj_t * cont_row;
    lv_obj_t * cont_col;
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    cont_col = lv_obj_create(parent);
    lv_obj_set_size(cont_col, 500, 400);
    lv_obj_align_to(cont_col, cont_row, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);

    cont_row = lv_obj_create(cont_col);
    lv_obj_set_size(cont_row, 450, 50);
    lv_obj_align(cont_row, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_flex_flow(cont_row, LV_FLEX_FLOW_ROW);

    lv_obj_t * label;
    label = lv_label_create(cont_row);
    lv_label_set_text_fmt(label, "MCC: ");
    mcc_label = lv_label_create(cont_row);


    cont_row = lv_obj_create(cont_col);
    lv_obj_set_size(cont_row, 450, 50);
    lv_obj_align(cont_row, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_flex_flow(cont_row, LV_FLEX_FLOW_ROW);

    label = lv_label_create(cont_row);
    lv_label_set_text_fmt(label, "MNC: ");
    mnc_label = lv_label_create(cont_row);


    cont_row = lv_obj_create(cont_col);
    lv_obj_set_size(cont_row, 450, 50);
    lv_obj_align(cont_row, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_flex_flow(cont_row, LV_FLEX_FLOW_ROW);

    label = lv_label_create(cont_row);
    lv_label_set_text_fmt(label, "IMEI: ");
    imei_label = lv_label_create(cont_row);


    cont_row = lv_obj_create(cont_col);
    lv_obj_set_size(cont_row, 450, 50);
    lv_obj_align(cont_row, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_flex_flow(cont_row, LV_FLEX_FLOW_ROW);

    label = lv_label_create(cont_row);
    lv_label_set_text_fmt(label, "IMSI: ");
    imsi_label = lv_label_create(cont_row);


    cont_row = lv_obj_create(cont_col);
    lv_obj_set_size(cont_row, 450, 50);
    lv_obj_align(cont_row, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_flex_flow(cont_row, LV_FLEX_FLOW_ROW);

    label = lv_label_create(cont_row);
    lv_label_set_text_fmt(label, "ICCID: ");
    iccid_label = lv_label_create(cont_row);


    cont_row = lv_obj_create(cont_col);
    lv_obj_set_size(cont_row, 450, 50);
    lv_obj_align(cont_row, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_flex_flow(cont_row, LV_FLEX_FLOW_ROW);

    label = lv_label_create(cont_row);
    lv_label_set_text_fmt(label, "Cell ID: ");
    cid_label = lv_label_create(cont_row);


    signal_strength = signal_bar_create(parent);

    rt_thread_t tid;

    tid = rt_thread_create("network_test", network_test, RT_NULL, 4096, 23, 10);
    if (tid == RT_NULL)
    {
        LOG_E("Fail to create 'network_test' thread");
    }
    rt_thread_startup(tid);

}

static void urc_csq_func(const char *data, rt_size_t size)
{
    LOG_D("Recive strength change URC, %d!",  size);
}

static struct at_urc network_urc_table[] = {
    {"+CSQ",   "\r\n",     urc_csq_func},
};

int network_urc_register(void)
{
    /* 添加多种 URC 数据至 URC 列表中，当接收到同时匹配 URC 前缀和后缀的数据，执行 URC 函数  */
    at_set_urc_table(network_urc_table, sizeof(network_urc_table) / sizeof(network_urc_table[0]));
    return RT_EOK;
}

static void network_test(void *parameter)
{
    int flag = 0;
    char *mcc1 = "460";
    char *mcc2 = "462";
    network_urc_register();
    refresh_imei();
    refresh_cid();
    refresh_iccid();
    refresh_imsi();
    refresh_plmn();

    while (1)
    {

        // set_strength(flag%100);

        flag++;
        rt_thread_mdelay(1000);
    }
}
