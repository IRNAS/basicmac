// Copyright (C) 2016-2019 Semtech (International) AG. All rights reserved.
//
// This file is subject to the terms and conditions defined in file 'LICENSE',
// which is part of this source code package.

#include "lmic.h"

#include "svcdefs.h"

#ifdef SVC_backtrace
#include "backtrace/backtrace.h"
#endif

void io_int_handler(void){
		
    // Application start hook
    SVCHOOK_interrupt();
		
}
