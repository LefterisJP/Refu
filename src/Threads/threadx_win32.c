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

//*---------------------Corrensponding Header inclusion---------------------------------
#include <Definitions/types.h> //for fixed size data types
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/defarg.h> //for enabling default arguments
#include <windows.h> //for the WinThread HANDLE definition
#include <Threads/thread_flags.h> //for common Thread flags
#include <Threads/thread_decl.h> //for RF_Thread
#include <Threads/threadx_decl.h> //for RF_ThreadX
#include <Threads/threadx.h>
//*---------------------Outside module inclusion----------------------------------------
//for io buffer initialization
    #include <Definitions/threadspecific.h> // for the thread specific keyword used in the ioBuffer
    #include <String/string_decl.h> //for RF_String (ioBuffer type)
    #include <String/stringx_decl.h> //for RF_StringX (ioBuffer type)
    #include "../IO/buff.ph"//stdio buffer thread-specific initialization
//for error logging
    #include <Utils/log.h>
//for memory allocation macros
    #include <Utils/memory.h> //for refu memory allocation
//for local scope macros
    #include <Utils/localmem_decl.h> // for RF_LocalMemoryStack
    #include <Utils/localmem.h> //for initializing the local memory stack of this thread
//*----------------------------End of Includes------------------------------------------

/**
** @internal
** The thread signal is what RF_ThreadX use in order
** to communicate with other threads. Using this Signal a
** message queue for a Thread is implemented.
** @author Lefteris
** @date 11/04/2012
*/
typedef struct RF_SignalThread
{
    //! The data sent to the thread
    void *data;
    //! The size of the data in bytes
    uint32_t size;
}RF_SignalThread;

//The function that serves as the starting address for a threadX in win32
static DWORD WINAPI RF_THREADX_FUNCTION(LPVOID  t)
{
    DWORD ret;//the return value
    //the void pointer is actually a thread
    RF_ThreadX* thread = (RF_ThreadX*)t;
    //initialize the local memory stack of the thread
    RF_LocalMemoryStack lms;
    if(!rfLMS_Init(&lms, thread->INH_Thread.lmsSize))
    {
        //TODO: Add the thread's address in the msg
        LOG_ERROR(
            "Failure to initialize a thread because its local memory "
            "stack could not be initialized", RE_LOCALMEMSTACK_INIT)
        return RE_LOCALMEMSTACK_INIT;
    }
    //initialize the stdio for this thread
    rfInitStdio();
    //in RF_ThreadX the parameter to the main thread function is a pointer to the thread itself
    ret = (DWORD) thread->INH_Thread.ptr2onExecution(thread);
    //free the local memory stack and return
    free(lms.stack);
    return ret;
}

// Allocates and returns a ThreadX
RF_ThreadX* rfThreadX_Create(uint32_t flags, void* (*ptr2onExecution)(RF_ThreadX*),
                             void* data,uint64_t lmsSize,uint32_t maxMsgQueue)
{
    RF_ThreadX* ret;
    RF_MALLOC(ret,sizeof(RF_ThreadX));
    if( rfThreadX_Init(ret,flags,ptr2onExecution,data,lmsSize,maxMsgQueue)==false)
    {
        free(ret);
        return 0;
    }
    return ret;
}
// Initializes a ThreadX

bool rfThreadX_Init(RF_ThreadX* t, uint32_t flags,
                    void* (*ptr2onExecution)(RF_ThreadX*), void* data ,
                    uint64_t lmsSize,uint32_t maxMsgQueue)
{
    //get the data and the lms size
    t->INH_Thread.data = data;
    t->INH_Thread.lmsSize = lmsSize;
    //get the function given by the user
    if(ptr2onExecution != 0)
        t->INH_Thread.ptr2onExecution = (void*(*)(void*))ptr2onExecution;
    else
    {
        LOG_ERROR("Passed a null pointer for the thread's execution. The thread will be doing nothing, so it is meaningless",RE_THREAD_NULL_EXECUTION_FUNCTION);
        return false;
    }

    //this is an RF_ThreadX so
    t->INH_Thread.flags = flags;
    t->INH_Thread.flags|= RF_THRATT_THREADX;

    //passing "this" as a parameter to the function because we are using it in the thread's running life
    t->INH_Thread.tHandle = CreateThread(NULL,0, (LPTHREAD_START_ROUTINE) RF_THREADX_FUNCTION,(LPVOID) t,0, NULL);

    if(!t->INH_Thread.tHandle)
    {
        LOG_ERROR("CreateThread() in Windows failed. Failed to initialize Thread",RE_THREAD_CREATION);
        return false;
    }
    ///Initializing ThreadX data here
    t->msgIndex = 0;
    t->maxSignals = maxMsgQueue;
    RF_MALLOC(t->msgQueue,sizeof(RF_SignalThread)*t->maxSignals);
    InitializeCriticalSection(&t->lock);

    //success
    return true;
}

// Destroys an RF_ThreadX pointer
void rfThreadX_Destroy(RF_ThreadX* t)
{
    uint32_t i;
    //also free the handle
    CloseHandle(t->INH_Thread.tHandle);
    //destroy the Critical section
    DeleteCriticalSection(&t->lock);
    //free anything the message queue might have left
    for(i=0;i<t->msgIndex;i++)
    {
        free(t->msgQueue[i].data);
    }
    //free the queue itself
    free(t->msgQueue);

    //also free the thread's memory
    free(t);
}

// Deinitializes an RF_ThreadX
void rfThreadX_Deinit(RF_ThreadX* t)
{
    uint32_t i;
    //also free the handle
    CloseHandle(t->INH_Thread.tHandle);
    //destroy the Critical section
    DeleteCriticalSection(&t->lock);
    //free anything the message queue might have left
    for(i=0;i<t->msgIndex;i++)
    {
        free(t->msgQueue[i].data);
    }
    //free the queue itself
    free(t->msgQueue);
}

// Frees a thread but also immediately causes it to exit.
char rfThreadX_Kill(RF_ThreadX* t)
{
    //destroy the thread
    uint32_t i;
    int32_t ret = TerminateThread(t->INH_Thread.tHandle,1); //1 is the exit code, @todo make it so that I can input my own exit codes
    //also free the handle
    CloseHandle(t->INH_Thread.tHandle);
    //destroy the Critical section
    DeleteCriticalSection(&t->lock);
    //free anything the message queue might have left
    for(i=0;i<t->msgIndex;i++)
    {
        free(t->msgQueue[i].data);
    }
    //free the queue itself
    free(t->msgQueue);

    //return
    if(ret != 0)
        return true;
    return false;
}

// Attempts to send a signal from the current thread to the target RF_ThreadX. If the message queue is currently being used then RF_THREADX_QUEUE_BUSY is returned
int32_t rfThreadX_TrySendSignal(RF_ThreadX* t,void* data,uint32_t size)
{
    //attempt to access the data
    if(TryEnterCriticalSection(&t->lock) == 0)//another thread already hols the lock
        return RF_THREADX_QUEUE_BUSY;

    //check if the queue is full
    if(t->msgIndex == t->maxSignals)
    {
        LeaveCriticalSection(&t->lock);
        return RF_THREADX_QUEUE_FULL;
    }

    //allocate the data for this signal
    RF_MALLOC(t->msgQueue[t->msgIndex].data,size)
    //copy the data
    memcpy(t->msgQueue[t->msgIndex].data,data,size);
    //save the size in the signal
    t->msgQueue[t->msgIndex].size = size;
    //increase the message index
    t->msgIndex++;
    //finally unlock the queue and return success
    LeaveCriticalSection(&t->lock);
    return RF_SUCCESS;
}

// Attempts to send a signal from the current thread to the target RF_ThreadX. If the message queue is currently being used the thrad will hang and wait until it's unlocked and its turn comes
int32_t rfThreadX_SendSignal(RF_ThreadX* t,void* data,uint32_t size)
{
    //attempt to access the data
    EnterCriticalSection(&t->lock);

    //check if the queue is full
    if(t->msgIndex == t->maxSignals)
    {
        LeaveCriticalSection(&t->lock);
        return RF_THREADX_QUEUE_FULL;
    }

    //allocate the data for this signal
    RF_MALLOC(t->msgQueue[t->msgIndex].data,size)
    //copy the data
    memcpy(t->msgQueue[t->msgIndex].data,data,size);
    //save the size in the signal
    t->msgQueue[t->msgIndex].size = size;
    //increase the message index
    t->msgIndex++;
    //finally unlock the queue and return success
    LeaveCriticalSection(&t->lock);
    return RF_SUCCESS;
}

// Attempts to read the message queue of the thread for signals. If the message queue is currently being used the thread will hang and wait until it's unlocked and its turn comes.
int32_t rfThreadX_ReadSignal(RF_ThreadX* t,void* data,uint32_t* size)
{
    //attempt to access the data
    LeaveCriticalSection(&t->lock);

    //check if the queue is empty
    if(t->msgIndex == 0)
    {
        LeaveCriticalSection(&t->lock);
        return RF_THREADX_QUEUE_EMPTY;
    }

    //get the size
    *size = t->msgQueue[0].size;
    //copy the data
    memcpy(data,t->msgQueue[0].data,*size);
    //free the data from the queue
    free(t->msgQueue[0].data);
    //move the queue to make up for the lost data
    t->msgIndex--;
    memmove(t->msgQueue,t->msgQueue+1,t->msgIndex*sizeof(RF_SignalThread));
    //finally unlock message queue and return success
    LeaveCriticalSection(&t->lock);
    return RF_SUCCESS;
}

// Attempts to read the message queue of the thread for signals. If the message queue is currently being used the thread then RF_THREADX_QUEUE_BUSY is returned
int32_t rfThreadX_TryReadSignal(RF_ThreadX* t,void* data,uint32_t* size)
{
    //attempt to access the data
    if(TryEnterCriticalSection(&t->lock) == 0)//owned by another thread
    {
        return RF_THREADX_QUEUE_BUSY;
    }

    //check if the queue is empty
    if(t->msgIndex == 0)
    {
        LeaveCriticalSection(&t->lock);
        return RF_THREADX_QUEUE_EMPTY;
    }

    //get the size
    *size = t->msgQueue[0].size;
    //copy the data
    memcpy(data,t->msgQueue[0].data,*size);
    //free the data from the queue
    free(t->msgQueue[0].data);
    //move the queue to make up for the lost data
    t->msgIndex--;
    memmove(t->msgQueue,t->msgQueue+1,t->msgIndex*sizeof(RF_SignalThread));
    //finally unlock message queue and return success
    LeaveCriticalSection(&t->lock);
    return RF_SUCCESS;
}

