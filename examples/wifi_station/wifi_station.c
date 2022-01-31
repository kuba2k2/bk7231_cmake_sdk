/**
 ******************************************************************************
 * @file    wifi_station.c
 * @author
 * @version V1.0.0
 * @date
 * @brief   Connect to access point using core wlan APIs
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2017 Beken Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ******************************************************************************
 */
#include "include.h"
#include "wifi_station.h"

#include "rtos_pub.h"
#include "error.h"
#include "wlan_ui_pub.h"
#include "uart_pub.h"
#include "mem_pub.h"
#include "rw_pub.h"
#include "param_config.h"
#include "str_pub.h"

static beken_semaphore_t station_scan_handle = NULL;

void wifi_station_scan_ap_cb(void *ctxt, uint8_t param)
{
    if(station_scan_handle)
    {
        rtos_set_semaphore( &station_scan_handle );
    }
}


void show_wifi_station_status(rw_evt_type sts)
{
    switch(sts)
    {
    case RW_EVT_STA_IDLE:
        os_printf("%s\r\n", "MSG_IDLE");
        break;
    case RW_EVT_STA_CONNECTING:
        os_printf("%s\r\n", "MSG_CONNECTING");
        break;
    case RW_EVT_STA_PASSWORD_WRONG:
        os_printf("%s\r\n", "MSG_PASSWD_WRONG");
        break;
    case RW_EVT_STA_NO_AP_FOUND:
        os_printf("%s\r\n", "MSG_NO_AP_FOUND");
        break;
    case RW_EVT_STA_CONNECT_FAILED:
        os_printf("%s\r\n", "MSG_CONN_FAIL");
        break;
    case RW_EVT_STA_CONNECTED:
        os_printf("%s\r\n", "MSG_CONN_SUCCESS");
        break;
    case RW_EVT_STA_GOT_IP:
        os_printf("%s\r\n", "MSG_GOT_IP");
        break;
    default:
        os_printf("%s\r\n", "unknow");
        break;
    }
}

int target_wifi_station_connect(IN CONST char *ssid, IN CONST char *passwd)
{
    network_InitTypeDef_st wNetConfig;

    os_memset(&wNetConfig, 0x0, sizeof(network_InitTypeDef_st));

    os_strcpy((char *)wNetConfig.wifi_ssid, ssid);
    os_strcpy((char *)wNetConfig.wifi_key, passwd);

    wNetConfig.wifi_mode = STATION;
    wNetConfig.dhcp_mode = DHCP_CLIENT;
    wNetConfig.wifi_retry_interval = 100;

    bk_wlan_start(&wNetConfig);

    return 0;
}

void wifi_station_connect_check(void)
{
    rw_evt_type Sta_sts, STA_sts_prv;
    int looptims = 10;

    rtos_delay_milliseconds((TickType_t)500);  //mS

    STA_sts_prv =  mhdr_get_station_status();
    while(looptims)
    {
        Sta_sts = mhdr_get_station_status();
        if(Sta_sts == RW_EVT_STA_GOT_IP)
        {
            show_wifi_station_status(Sta_sts);
            os_printf("\r\n WIFI connect succeed!\r\n");
            goto exit;
        }
        else if(Sta_sts == RW_EVT_STA_NO_AP_FOUND)
        {
            show_wifi_station_status(Sta_sts);
            os_printf("\r\n none ap, stop connect!\r\n");
            bk_wlan_stop(STATION);
            goto exit;
        }
        else if(Sta_sts == RW_EVT_STA_CONNECT_FAILED)
        {
            show_wifi_station_status(Sta_sts);
            os_printf("\r\nconnect failed!\r\n");
            bk_wlan_stop(STATION);
            goto exit;
        }
        else if(Sta_sts == RW_EVT_STA_PASSWORD_WRONG)
        {
            show_wifi_station_status(Sta_sts);
            os_printf("\r\n wifi_password error!\r\n");
            bk_wlan_stop(STATION);
            goto exit;
        }

        if(Sta_sts == STA_sts_prv)
        {
            looptims--;
            rtos_delay_milliseconds((TickType_t)1000);  //mS
        }
        else
        {
            looptims = 5;
            STA_sts_prv = Sta_sts;
        }
    }

    show_wifi_station_status(Sta_sts);
    os_printf("\r\n timeout, stop connect!\r\n");
    bk_wlan_stop(STATION);

exit:
    return;
}


int wifi_station_show_scan_result(unsigned char *ssid)
{
    struct sta_scan_res *scan_rst_table;
    char scan_rst_ap_num = 0;       /**< The number of access points found in scanning. */
    int i;

    scan_rst_ap_num = bk_wlan_get_scan_ap_result_numbers();
    if(scan_rst_ap_num == 0)
    {
        os_printf("\r\n The ssid(%s) is nothingness!\r\n", ssid);
        return -1;
    }

    scan_rst_table = (struct sta_scan_res *)os_malloc(sizeof(struct sta_scan_res) * scan_rst_ap_num);
    if(scan_rst_table == NULL)
    {
        os_printf("scan result table malloc failed!\r\n");
        return 0;
    }

    bk_wlan_get_scan_ap_result(scan_rst_table, scan_rst_ap_num);

    os_printf("Assign Scan %d AP:\r\n", scan_rst_ap_num);
    for( i = 0; i < scan_rst_ap_num; i++ )
    {
        os_printf("%d: %s, ", i + 1, scan_rst_table[i].ssid);
        os_printf("Channal:%d, ", scan_rst_table[i].channel);
        switch(scan_rst_table[i].security)
        {
        case SECURITY_TYPE_NONE:
            os_printf(" %s, ", "OPEN");
            break;
        case SECURITY_TYPE_WEP:
            os_printf(" %s, ", "CIPHER_WEP");
            break;
        case SECURITY_TYPE_WPA_TKIP:
            os_printf(" %s, ", "WPA_TKIP");
            break;
        case SECURITY_TYPE_WPA_AES:
            os_printf(" %s, ", "WPA_AES");
            break;
        case SECURITY_TYPE_WPA2_TKIP:
            os_printf(" %s, ", "WPA2_TKIP");
            break;
        case SECURITY_TYPE_WPA2_AES:
            os_printf(" %s, ", "WPA2_AES");
            break;
        case SECURITY_TYPE_WPA2_MIXED:
            os_printf(" %s, ", "WPA2_MIXED");
            break;
        case SECURITY_TYPE_AUTO:
            os_printf(" %s, ", "CIPHER_AUTO");
            break;
        default:
            os_printf(" %s(%d), ", "unknown", scan_rst_table[i].security);
            break;
        }
        os_printf("RSSI=%d \r\n", scan_rst_table[i].level);
    }

    if( scan_rst_table != NULL )
    {
        os_free(scan_rst_table);
        scan_rst_table = NULL;
    }
}

int wifi_station_scan_ap(unsigned char *ssid)
{
    int status = -1;
    OSStatus err = kNoErr;

    bk_wlan_scan_ap_reg_cb(wifi_station_scan_ap_cb);
    bk_wlan_start_assign_scan(&ssid, 1);

    err = rtos_get_semaphore(&station_scan_handle, BEKEN_WAIT_FOREVER);
    if(err == kNoErr)
    {
        status = wifi_station_show_scan_result(ssid);
    }

    return status;
}

void wifi_station_thread( beken_thread_arg_t arg )
{
    (void) arg;
    int status;
    char *ap_ssid = "ssid";
    char *ap_key = "password";
    OSStatus err = kNoErr;

    err = rtos_init_semaphore( &station_scan_handle, 1 );
    if(err != kNoErr)
    {
        goto exit;
    }

    status = wifi_station_scan_ap((unsigned char *)ap_ssid);
    if(status < 0)
    {
        goto exit;
    }

    os_printf("\r\nconnect wifi_SSID:%s,wifi_Password:%s\r\n", ap_ssid, ap_key);

    target_wifi_station_connect(ap_ssid, ap_key);

    wifi_station_connect_check();

exit:

    rtos_delete_thread( NULL );
}

int demo_start( void )
{
    OSStatus err = kNoErr;

    /* Start a wifi_sta thread*/
    err = rtos_create_thread( NULL,
                              BEKEN_APPLICATION_PRIORITY,
                              "wifi_sta",
                              (beken_thread_function_t)wifi_station_thread,
                              0x800,
                              (beken_thread_arg_t)0 );

    return err;
}
