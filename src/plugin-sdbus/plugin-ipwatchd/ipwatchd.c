/* IPwatchD - IP conflict detection tool for Linux
 * Copyright (C) 2007-2010 Jaroslav Imrich <jariq(at)jariq(dot)sk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 */

/** \file ipwatchd.c
 * \brief Main source file of the project
 */

#include "ipwatchd.h"
#include <pthread.h>


//! Flag indicating that output of program must be recorded by syslog
int syslog_flag = 1;

//! Flag indicating testing mode when every ARP packet is considered to be conflicting
int testing_flag = 0;

//! Structure that holds information about network interfaces
IPWD_S_DEVS devices;

//! Structure that holds values of particular configuration variables
IPWD_S_CONFIG config;

//! Handle for libpcap
pcap_t *h_pcap = NULL;

//! Flag indicating loop over
volatile int exit_flag =  0;

//! Structure that holds ip conflict check context
IPWD_S_CHECK_CONTEXT check_context;
IPCONFLICT_DEV_INFO *ipconflict_dev_info;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// 每100毫秒抓一次包
int timeout_ms = 100;


int start_ipwatchd(ipwatchd_args_t *args){
    /* Path to configuration file must be specified */
    if (args->config_file == NULL)
    {
        ipwd_message (IPWD_MSG_TYPE_ERROR, "You must specify path to configuration file.");
        return (IPWD_RV_ERROR);
    }

    /* Only root can run IPwatchD */
    if (getuid () != 0)
    {
        ipwd_message (IPWD_MSG_TYPE_ERROR, "You must be root to run IPwatchD");
        return (IPWD_RV_ERROR);
    }

    /* Read config file */
    if (ipwd_read_config (args->config_file) == IPWD_RV_ERROR)
    {
        ipwd_message (IPWD_MSG_TYPE_ERROR, "Unable to read configuration file");
        return (IPWD_RV_ERROR);
    }

    ipwd_message (IPWD_MSG_TYPE_INFO, "IPwatchD started");

    char errbuf[PCAP_ERRBUF_SIZE];
    struct bpf_program fp;

    /* Check if "any" pseudodevice is available */
    /* IPwatchD cannot be used on Debian GNU/kFreeBSD because of the lack of this device */
    pcap_if_t * pcap_alldevs = NULL;
    pcap_if_t * pcap_dev = NULL;
    int any_exists = 0;

    if (pcap_findalldevs(&pcap_alldevs, errbuf))
    {
        ipwd_message (IPWD_MSG_TYPE_ERROR, "Unable to get network device list - %s", errbuf);
        return (IPWD_RV_ERROR);
    }

    for (pcap_dev = pcap_alldevs; pcap_dev; pcap_dev = pcap_dev->next)
    {
        if (strcasecmp (pcap_dev->name, "any") == 0)
        {
            any_exists = 1;
            break;
        }
    }

    if (!any_exists)
    {
        ipwd_message (IPWD_MSG_TYPE_ERROR, "Pseudodevice \"any\" used by libpcap is not available");
        return (IPWD_RV_ERROR);
    }

    /* Initialize libpcap and listen on all interfaces */
    h_pcap = pcap_open_live ("any", BUFSIZ, 0, timeout_ms, errbuf);
    if (h_pcap == NULL)
    {
        ipwd_message (IPWD_MSG_TYPE_ERROR, "Unable to create packet capture object - %s", errbuf);
        return (IPWD_RV_ERROR);
    }

    /* Compile packet capture filter - only ARP packets will be captured */
    if (pcap_compile (h_pcap, &fp, "arp", 0, 0) == -1)
    {
        ipwd_message (IPWD_MSG_TYPE_ERROR, "Unable to compile packet capture filter - %s", pcap_geterr (h_pcap));
        return (IPWD_RV_ERROR);
    }

    /* Set packet capture filter */
    if (pcap_setfilter (h_pcap, &fp) == -1)
    {
        ipwd_message (IPWD_MSG_TYPE_ERROR, "Unable to set packet capture filter - %s", pcap_geterr (h_pcap));
        return (IPWD_RV_ERROR);
    }

    pcap_freecode (&fp);

    ipwd_message (IPWD_MSG_TYPE_DEBUG, "Entering pcap success");
    while (!exit_flag) {
        pthread_mutex_lock(&mutex);
        pcap_dispatch (h_pcap, -1, ipwd_analyse, NULL);
        // TODO: 空闲时1秒抓包一次，检查ip冲突
        if (timeout_ms != 1000) {
            timeout_ms = 1000;
            pcap_set_timeout(h_pcap,timeout_ms);
        }
        pthread_mutex_unlock(&mutex);
        // sleep 1ms，free cpu
        usleep(1000);
    }
    pthread_mutex_destroy(&mutex);
    pcap_close (h_pcap);

    /* Stop IPwatchD */
    ipwd_message (IPWD_MSG_TYPE_INFO, "IPwatchD stopped");

    closelog ();

    if (config.script != NULL)
    {
        free (config.script);
        config.script = NULL;
    }

    if (devices.dev != NULL)
    {
        free (devices.dev);
        devices.dev = NULL;
    }
    IPCONFLICT_DEV_INFO *ipconflictinfo = ipconflict_dev_info;
    while (ipconflictinfo)
    {
        IPCONFLICT_DEV_INFO *tmp = ipconflictinfo;
        ipconflictinfo = ipconflictinfo->next;
        free(tmp);
    }
    return (IPWD_RV_SUCCESS);
}

int ipwd_conflict_check (sd_bus_message *m, void *userdata,
                              sd_bus_error *ret_error){
    int ret = 0;
    pthread_mutex_lock(&mutex);
    // send arp packet
    timeout_ms = 10;
    pcap_set_timeout(h_pcap,timeout_ms);
    sd_bus_message_get_data(m,&check_context.ip,&check_context.misc);
    check_context.wait_reply = true;
    check_context.msg = m;
    ret = ipwd_check_context_verify();
    if (ret < 0) {
        if (ret = sd_bus_reply_method_return(check_context.msg, "s", ""),ret < 0){
            ipwd_message (IPWD_MSG_TYPE_ERROR, "dbus method reply err");
        }
        check_context.wait_reply = false;
        pthread_mutex_unlock(&mutex);
    }
    for (int i = 0;i < PCAP_MAX_TIMES;i++){
        ipwd_genarp(check_context.dev.device, "0.0.0.0", check_context.dev.mac, check_context.ip, "ff:ff:ff:ff:ff:ff", ARPOP_REQUEST);
        usleep(10000);
        if(pcap_dispatch (h_pcap, -1, ipwd_analyse, NULL) > 0){
            if (check_context.conflic_mac != NULL){
                break;
            }
        }
    }

    if (ret = sd_bus_reply_method_return(check_context.msg, "s", check_context.conflic_mac ? check_context.conflic_mac : ""),ret < 0){
        ipwd_message (IPWD_MSG_TYPE_ERROR, "dbus method reply err");
    }
    if (check_context.conflic_mac) {
        free(check_context.conflic_mac);
        check_context.conflic_mac = NULL;
    }
    check_context.wait_reply = false;
    pthread_mutex_unlock(&mutex);
    return 0;
}


int ipwd_check_context_verify ()
{
    if (check_context.ip == NULL || check_context.misc == NULL) {
        ipwd_message (IPWD_MSG_TYPE_ERROR, "Unable to convert source IP address %s", check_context.ip);
        return -1;
    }

    struct in_addr sip, dip;
    if (inet_aton(check_context.ip, &sip) == 0) {
        ipwd_message (IPWD_MSG_TYPE_ERROR, "Unable to convert source IP address %s", check_context.ip);
        return -1;
    }
    if (sip.s_addr == 0) {
        ipwd_message (IPWD_MSG_TYPE_ERROR, "Error source IP address %s", check_context.ip);
        return -1;
    }

    int i;
    memset(&check_context.dev, 0, sizeof(IPWD_S_DEV));
    check_context.dev.state = IPWD_DEVICE_STATE_UNUSABLE;

    if (check_context.misc[0] == '\0') {
        IPWD_S_DEV dev;
        for (i = 0; i < devices.devnum; i++) {
            memset(dev.device, 0, sizeof(dev.device));
            strcpy(dev.device, devices.dev[i].device);
            if (ipwd_devinfo (dev.device, dev.ip, dev.mac) == IPWD_RV_ERROR) {
                continue;
            }
            if (inet_aton(dev.ip, &dip) != 0) {
                uint8_t *p = (uint8_t*)&sip.s_addr;
                uint8_t *q = (uint8_t*)&dip.s_addr;

                if (p[0] == q[0] && p[1] == q[1] && p[2] == q[2]) {
                    memcpy(&check_context.dev, &dev, sizeof(IPWD_S_DEV));
                    check_context.dev.state = IPWD_DEVICE_STATE_USABLE;
                    break;
                }
            }

            memcpy(&check_context.dev, &dev, sizeof(IPWD_S_DEV));
            check_context.dev.state = IPWD_DEVICE_STATE_USABLE;
        }
    } else {
        strncpy(check_context.dev.device, check_context.misc, IPWD_MAX_DEVICE_NAME_LEN - 1);
        for (i = 0; i < devices.devnum; i++) {
            if (strcasecmp (check_context.dev.device, devices.dev[i].device) == 0) {
                if (ipwd_devinfo (check_context.dev.device, check_context.dev.ip, check_context.dev.mac) != IPWD_RV_ERROR) {
                    check_context.dev.state = IPWD_DEVICE_STATE_USABLE;
                }
                break;
            }
        }
    }

    if (check_context.dev.state == IPWD_DEVICE_STATE_UNUSABLE) {
        ipwd_message(IPWD_MSG_TYPE_ALERT, "can't find dev: %s - %s", check_context.ip, check_context.misc);
        return -1;
    } else {
        ipwd_message(IPWD_MSG_TYPE_DEBUG, "check ip %s use dev: %s", check_context.ip, check_context.dev.device);
    }
    return 0;
}

void stop_ipwatchd(){
    exit_flag = 1;
}