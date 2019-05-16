// Copyright (C) 2016-2019 Semtech (International) AG. All rights reserved.
//
// This file is subject to the terms and conditions defined in file 'LICENSE',
// which is part of this source code package.

#include "lmic.h"
#include "lwmux/lwmux.h"

static lwm_job lj;
static osjob_t* mainjob;

//new job added to the stack
static osjob_t timer_job;
static void next (osjob_t* job);

static void txc (void) {
    os_setApproxTimedCallback(mainjob, os_getTime() + sec2osticks(5), next);
}

static bool tx (lwm_txinfo* txinfo) {
    txinfo->data = (unsigned char*) "hello";
    txinfo->dlen = 5;
    txinfo->port = 15;
    txinfo->txcomplete = txc;
    return true;
}

static void next (osjob_t* job) {
    lwm_request_send(&lj, 0, tx);
}

void app_dl (int port, unsigned char* data, int dlen, unsigned int flags) {
    debug_printf("DL[%d]: %h\r\n", port, data, dlen);
		debug_printf("app_dl:test send time:\r\n");
}

void repeat_job_function (osjob_t* job){
		debug_printf("repeat_job_function:os_time_in_sec= %d \r\n", (int)(os_getTime() / OSTICKS_PER_SEC));
    os_setTimedCallback(job, os_getTime() + sec2osticks(5), repeat_job_function);
}

void app_main (osjob_t* job) {
    debug_printf("app_main:Hello World! new job test\r\n");

		//set callback for repeat_job
    os_setTimedCallback(&timer_job, os_getTime() + sec2osticks(5), repeat_job_function);
		//os_setCallback(&timer_job, repeat_job_function);
		debug_printf("app_main:test5.1\r\n");

    // join network
    lwm_setmode(LWM_MODE_NORMAL);

    // re-use current job
    mainjob = job;

    // initiate first uplink
    next(mainjob);
}

