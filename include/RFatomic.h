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
** This is simply a fontend for gcc's builtin functions for atomic operations.
** http://gcc.gnu.org/onlinedocs/gcc-4.1.0/gcc/Atomic-Builtins.html
** Should look more closely at C11 progress and the atomic type flag.
** More details here: http://gcc.gnu.org/wiki/Atomic/C11
** When complete it should also be used.
*/

/**
 * An atomic int
 */
typedef struct atomic
{
    int val;
}RF_Atomic;

#define RF_ATOMIC_INIT(a_) {a_}

static inline int rfAtomic_inc_fetch(RF_Atomic* a)
{
    return __sync_add_and_fetch(a->val, 1);
}

static inline void rfAtomic_inc(RFAtomic* a)
{
    __sync_add_and_fetch(a->val, 1);
}

static inline int rfAtomic_dec_fetch(RFAtomic* a)
{
    return __sync_sub_and_fetch(a->val, 1);
}

static inline void rfAtomic_dec(RFatomic* a)
{
    __sync_sub_and_fetch(a->val, 1);
}
