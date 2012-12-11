/*
** Copyright (c) 2011-2012, Karapetsas Eleftherios
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
**  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
**  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the distribution.
**  3. Neither the name of the Original Author of Refu nor the names of its contributors may be used to endorse or promote products derived from
**
**  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
**  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
**  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
**  SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
**  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
**  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**
**
** --Utils/localmem.h
**
** Contains the local memory stack functions
**
**
---------------------For internal library include make sure to have----------------------------
#include <Definitions/types.h> //fixed size data types
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/threadspecific.h> //for the thread specific attribute
#include <Utils/localmem_decl.h> // for RF_LocalMemoryStack
#include <Utils/localmem.h>
---------------------For internal library include make sure to have----------------------------
*/
#ifndef RF_LOCAL_MEMORY_H
#define RF_LOCAL_MEMORY_H


#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

//! @memberof RF_LocalMemoryStack
//! @brief Initializes the local memory stack
//!
//! This function simply initializes a local memory stack and should not be used by
//! ther user directly. It is used in 2 occasions inside the library. Once inside the library's
//! initialization function @ref rfInit() and at every new @ref RF_Thread initialization since
//! this stack is thread specific.
//! @param lms The Local Memory stack to initialize
//! @param size The size that the local memory stack will have
void rfLMS_Init(RF_LocalMemoryStack* lms,uint64_t size);

//! @memberof RF_LocalMemoryStack
//! @brief Pushes the local memory stack
//!
//! This function simply pushes the local memory stack for @c size bytes so that the stack pointer
//! gets moved up by that amount of bytes. It also returns a pointer to the position of the stack
//! pointer before the push which will be the allocated pointer memory position. In that respect
//! the function works similarly to malloc()
//! @param size The size of the block to allocate
//! @return Returns a pointer to the allocated block or 0 for no more local memory available
i_DECLIMEX_ void* rfLMS_Push(uint64_t size);

//! @memberof RF_LocalMemoryStack
//! @brief Pops the local stack memory back to a certain value
//!
//! This function puts the local stack pointer back to a specific value
//! and in doing so essentially frees all the objects that were allocated to
//! parts of the stack before that movement.
//! @param t The stack value to which to pop the stack back to
i_DECLIMEX_ void rfLMS_Pop(uint64_t t);

//! @memberof RF_LocalMemoryStack
//! @brief Keeps the stack pointer before the specific macro evaluation
//!
//! This function remembers the stack pointer right before a given
//! local macro evaluation. Because of the sequence points there is not a given
//! method to see which macro gets evaluated first so at the beginning of the function
//! wrapper we will use all the saved pointer positions to find the one that is
//! closer at the beginning and this will be determined as the local memory stack
//! pointer before the function call
i_DECLIMEX_ void rfLMS_ArgsEval();




#ifdef __cplusplus
} //closing bracket for C++ calling
#endif

#endif //include guards end
