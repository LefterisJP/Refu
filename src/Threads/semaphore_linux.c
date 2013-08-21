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
/*------------- Corrensponding Header inclusion -------------*/
#include <Definitions/types.h> //for fixed size data types
#include <Definitions/imex.h> //for the import export macro
#include <semaphore.h> //for pthread_semaphores
#include <Threads/semaphore_decl.h> //for RF_Semaphore
#include <Threads/semaphore.h>
/*------------- Outside Module inclusion -------------*/
//for error logging
    #include <stdio.h>//for FILE* used inside printf.h
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Threads/common.h> //for rfThread_GetID()
    #include <Definitions/defarg.h> //since LOG_ERROR macros use argument counting
    #include <Utils/error.h>
//for memory allocation
#include <stdlib.h> //for malloc, calloc,realloc and exit()
#include <Definitions/retcodes.h> //for error codes, logged in allocation failure
#include <Utils/memory.h> //for refu memory allocation
/*------------- libc inclusion -------------*/
#include <errno.h>
#include <time.h> //for CLOCK_REALTIME and clock_gettime()
/*------------- End of includes -------------*/

// Allocates and returns a semaphore object
RF_Semaphore* rfSemaphore_Create(uint32_t initialCount, uint32_t maxCount)
{
    RF_Semaphore* ret;
    RF_MALLOC(ret, sizeof(RF_Semaphore), NULL);
    if(rfSemaphore_Init(ret,initialCount,maxCount) != RF_SUCCESS)
    {
        free(ret);
        return NULL;
    }
    //success
    return ret;
}
// Initializes a semaphore object
char rfSemaphore_Init(RF_Semaphore* s, uint32_t initialCount,
                      uint32_t maxCount)
{
    if(sem_init(&s->semaphoreObj,0,initialCount)!= 0)
    {
        RF_ERROR_SEMINIT("Initializing a semaphore failed", "sem_init");
        return false;
    }
    //success
    return true;
}

// Destroys a semaphore object and frees its memory. 
char rfSemaphore_Destroy(RF_Semaphore* s)
{
    if(!rfSemaphore_Deinit(s))
    {
        return false;
    }
    free(s);
    return true;
}
// Deinitializes a semaphore
char rfSemaphore_Deinit(RF_Semaphore* s)
{
    if(sem_destroy(&s->semaphoreObj) != 0)
    {
        RF_ERROR_SEMDESTROY("Failed to deinitialize a semaphore", "sem_destroy");
        return false;
    }
    return true;
}

// Waits for the semaphore. If succesfull decreases the count of the semaphore.
char rfSemaphore_Wait(RF_Semaphore* s)
{
    int32_t error = sem_wait(&s->semaphoreObj);
    if(error != 0)
    {
        RF_ERROR_SEMWAIT("Waiting on a semaphore failed", "sem_wait", error);
        return false;
    }
    //success
    return true;
}

// Same as the rfSemaphore_Wait function except that there is a timeout to waiting given as an argument
char rfSemaphore_TimedWait(RF_Semaphore* s, uint32_t ms, char* expire)
{
    struct timespec ts;
    time_t secs;
    uint32_t nanoSecs;
    int32_t error;
    *expire = false;
    //let's create the appropriate time structure
    if(clock_gettime(CLOCK_REALTIME, &ts) == -1)
    {
        RF_ERROR(0,
                 "Getting the system time failed. Can't perform a posix "
                 "semaphore timedwait.");
        return false;
    }

    //get the seconds
    secs = ms/1000;
    //get the nano seconds
    nanoSecs = (ms%1000)*1000000;
    //increase the time
    ts.tv_sec +=  secs;
    ts.tv_nsec += nanoSecs;
    //perform the timed wait
    error = sem_timedwait(&s->semaphoreObj,&ts);
    if(error != 0)
    {
        switch(error)
        {
            //waiting timed out
            case ETIMEDOUT:
                *expire = true;
            break;
            case EINVAL:
                RF_ERROR(0,
                         "Failed to wait for a semaphore because the process or "
                         "thread would have blocked, and the abs_timeout "
                         "parameter specified a nanoseconds field value less "
                         "than zero or greater than or equal to 1000 million "
                         "or due to invalid sem argument");
            break;
            case ENOSYS:
                RF_ERROR(0,
                         "Failed to wait for a semaphore because the function "
                         "sem_timewait() is not supported by this implementation");
            break;
            case EDEADLK:
                RF_ERROR(0, "Failed to wait for a semaphore because a deadlock "
                         "condition has been detected.");
            break;
            case EINTR:
                RF_ERROR(0, "Failed to wait for a semaphore because a signal "
                         "interrupted the sem_timedwait() function.");
            break;
            default:
                RF_ERROR(0,
                         "Generic Failure during waiting for a posix semaphore"
                         " with error code: %d", error);
            break;
        }
        return false;
    }
    //success
    return true;
}

// Tries to wait for the semaphore.
char rfSemaphore_TryWait(RF_Semaphore* s, char* busy)
{
    int32_t error = sem_trywait(&s->semaphoreObj);
    *busy = false;
    if(error != 0)
    {
        switch(error)
        {
            //the semaphore is already locked by another thread
            case EAGAIN:
                *busy = true;
            break;
            i_RP_SEMWAIT_CASES("Waiting on a semaphore failed", "sem_trywait",
                               error)
            default:
            RF_ERROR(0, "Waiting on a semaphore failed due to an unknown error "
                     "code %d returned by sem_trywait()", error);
            break;
        }
        return false;
    }
    //success
    return true;
}

// Releases ownerhship of a semaphore object, increasing its count by one
char rfSemaphore_Post(RF_Semaphore* s)
{
    //attempt to post, and if fail occurs error log
    if(sem_post(&s->semaphoreObj) != 0)
    {
        switch(errno)
        {
            case EINVAL:
                RF_ERROR(0,
                         "Failure during posting to a posix semaphore due to "
                         "the argument not being a valid semaphore");
            break;
            case ENOSYS:
                RF_ERROR(0,
                         "Failure during posting to a posix semaphore because "
                         "the sem_post() function is not supported by this "
                         "implementation");
            break;
            default:
                RF_ERROR(0,
                         "Generic Failure during posting to a posix semaphore "
                         "with error code: %d", errno);
            break;
        }
        return false;
    }
    //success
    return true;
}
