// Copyright (C) 2016-2019 Semtech (International) AG. All rights reserved.
//
// This file is subject to the terms and conditions defined in file 'LICENSE',
// which is part of this source code package.

#include "lmic.h"
#include "lwmux/lwmux.h"

static lwm_job lj;
static osjob_t* mainjob;

static osjob_t timer_job;
static void next (osjob_t* job);

void init_job_function (osjob_t* job);
/*
static void io_int_handler(void){
		os_setCallback(&timer_job, init_job_function);
}
*/
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
		debug_printf("test send time:\r\n");
}

void init_job_function (osjob_t* job){
		debug_printf("test_job_printf: %d \r\n", os_getTime());
    //os_setTimedCallback(job, os_getTime() + sec2osticks(5), init_job_function);
}

void app_main (osjob_t* job) {
    debug_printf("Hello World! test7\r\n");


    //os_setTimedCallback(&timer_job, os_getTime() + sec2osticks(5), init_job_function);
		//os_setCallback(&init_job, init_job_function);
		debug_printf("test5.1\r\n");

    // Application start hook
    //SVCHOOK_test_job(job);


    // join network
    lwm_setmode(LWM_MODE_NORMAL);

    // re-use current job
    mainjob = job;

    // initiate first uplink
    next(mainjob);
}

void app_int (void){
	debug_printf("__interrupt: %d \r\n", os_getTime());
}

