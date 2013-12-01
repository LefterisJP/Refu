/**
**      ==START OF REFU LICENSE==
**
** Copyright (c) 2011-2013, Karapetsas Eleftherios
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
**  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
**  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the distribution.
**  3. Neither the name of the Original Author of Refu nor the names of its contributors may be used to endorse or promote products derived from
** 
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**      ==END OF REFU LICENSE==
**
*/
#ifndef RF_PREPROCESSOR_TOKENS_H
#define RF_PREPROCESSOR_TOKENS_H

//for the preprocessor calls
#include "rf_xmacro_argcount.h"

//===The macros here help determine the fun start of each object of the refu library===//
//If ever, a preprocessor for Refu exists, these will automatically be created by following the naming convention
//so that they can be determined for any type of object
#define i_RF_GETTYPE_FUNCSTART(i_REFU_TYPE) \
    i_RP_PASTE3(i_TYPE_,i_REFU_TYPE,_FUNCSTART_)
#define i_TYPE_RFstringx_f_u_n_c_s_t_a_r_t   rfStringX
#define i_TYPE_RFstring_f_u_n_c_s_t_a_r_t   rfString
#define i_TYPE_RFlinked_list_f_u_n_c_s_t_a_r_t rfLinkedList
#define i_TYPE_RFlist_v_f_u_n_c_s_t_a_r_t   rfListV
#define i_TYPE_RFlist_p_f_u_n_c_s_t_a_r_t   rfListP
#define i_TYPE_RFarray_p_f_u_n_c_s_t_a_r_t   rfArrayP
#define i_TYPE_RFarray_v_f_u_n_c_s_t_a_r_t   rfArrayV
#define i_TYPE_RFbinary_array_f_u_n_c_s_t_a_r_t   rfBinaryArray
#define i_TYPE_RF_XMLTag_FUNCSTART_   rfXMLTag
#define i_TYPE_RF_XML_FUNCSTART_   rfXML
#define i_TYPE_RFthread_f_u_n_c_s_t_a_r_t   rfThread
#define i_TYPE_RFthread_x_f_u_n_c_s_t_a_r_t   rfThreadX
#define i_TYPE_RFmutex_f_u_n_c_s_t_a_r_t   rfMutex
#define i_TYPE_RFsemaphore_f_u_n_c_s_t_a_r_t   rfSemaphore
#define i_TYPE_RFdate_f_u_n_c_s_t_a_r_t   rfDate
#define i_TYPE_RFtimer_f_u_n_c_s_t_a_r_t   rfTimer
#define i_TYPE_RFtextfile_f_u_n_c_s_t_a_r_t   rfTextFile

//===The macros here determine the tokens for the destruction/definit and copy functions of the refu object types===//

#define i_RF_GET_DESTROY(i_REFU_TYPE)   i_NCRF_GET_DESTROY(i_RF_GETTYPE_FUNCSTART(i_REFU_TYPE))
#define i_NCRF_GET_DESTROY(i_FUNC_START_)   i_RP_PASTE2(i_FUNC_START_,_Destroy)
#define i_RF_GET_DEINIT(i_REFU_TYPE)   i_NCRF_GET_DEINIT(i_RF_GETTYPE_FUNCSTART(i_REFU_TYPE))
#define i_NCRF_GET_DEINIT(i_FUNC_START_)   i_RP_PASTE2(i_FUNC_START_,_Deinit)
#define i_RF_GET_COPYIN(i_REFU_TYPE)   i_NCRF_GET_COPYIN(i_RF_GETTYPE_FUNCSTART(i_REFU_TYPE))
#define i_NCRF_GET_COPYIN(i_FUNC_START_)   i_RP_PASTE2(i_FUNC_START_,_Copy_IN)
#define i_RF_GET_COPYOUT(i_REFU_TYPE)   i_NCRF_GET_COPYOUT(i_RF_GETTYPE_FUNCSTART(i_REFU_TYPE))
#define i_NCRF_GET_COPYOUT(i_FUNC_START_)   i_RP_PASTE2(i_FUNC_START_,_Copy_OUT)
#define i_RF_GET_EQUAL(i_REFU_TYPE) i_NCRF_GET_EQUAL(i_RF_GETTYPE_FUNCSTART(i_REFU_TYPE))
#define i_NCRF_GET_EQUAL(i_FUNC_START_) i_RP_PASTE2(i_FUNC_START_, _Equal)

#endif//include guards end
