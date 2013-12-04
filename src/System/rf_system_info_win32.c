/*
 *    == START OF REFU LICENSE ==
 *
 * Copyright (c) 2011-2013, Karapetsas Eleftherios
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the Original Author of Refu nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *    == END OF REFU LICENSE ==
 */


/*------------- Outside Module inclusion -------------*/
#include <Definitions/types.h> //for exact size data types
#include <Definitions/retcodes.h> //for bool
#include <Utils/endianess.h> //for RFendianess
/*------------- System specific inclusion --------------*/
#include <windows.h> //for QueryPerformanceFrequency()
/*------------- End of includes -------------*/


/**
 ** System information holding structure
 */
struct RFsystem_info {
    /**
     * A byte holding the system's endianess. 
     *Look @ref RFendianess for legal values
     */
    enum RFendianess endianess;
    /**
     *A boolean flag indicating whether the system supports high
     * resolution performance counter in windows and clock_gettime in Linux
     */
    bool has_high_res_timer;
    /**
     *In Windows we should also keep the PCFrequency in case we want
     * to use a performance counter
     */
    int64_t pc_frequency;
};
//system info global definition
struct RFsystem_info g_sys_info;

void rf_system_init()
{
    int32_t anint;

    /* get endianess */
    anint= (int32_t)0xdeadbeef;
    g_sys_info.endianess = (*(char *)&anint == (char)0xef)?
    RF_LITTLE_ENDIAN : RF_BIG_ENDIAN;
    /* see if we can have High resolution timer */
    g_sys_info.hasHighResTimer = QueryPerformanceFrequency(
        (LARGE_INTEGER *)&g_sys_info.pc_frequency,
    );
}

enum RFendianess rf_system_get_endianess()
{
    return g_sys_info.endianess;
}

enum RFendianess rf_system_get_other_endianess()
{
    if(g_sys_info.endianess == RF_LITTLE_ENDIAN)
    {
        return RF_BIG_ENDIAN;
    }
    return RF_LITTLE_ENDIAN;
}

bool rf_system_has_high_res_timer()
{
    return g_sys_info.has_high_res_timer;
}
