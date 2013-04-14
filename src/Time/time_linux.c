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
//*---------------------Corresponding Header inclusion---------------------------------
#include <rf_options.h>//for the module check
#include <Definitions/imex.h> //import export macro
#include <Definitions/types.h> //for the fixed size data types
#ifdef RF_MODULE_TIME_TIMER//module check
    #include <time.h> //for clockid_t used in RF_Timer
    #include <Time/timer_decl.h> //for RF_Timer
    #include <Time/timer.h>
#endif
#ifdef RF_MODULE_TIME_DATE
    #include <Time/date_decl.h> //for RF_Date
    #include <Time/date.h>
#endif
//*---------------------Module related inclusion----------------------------------------
#include <Time/sleep.h> //for the refu sleep functions
#include "common.ph"//private time macros
//*---------------------Outside module inclusion----------------------------------------
#include "../System/info.ph" //to check if we have a high-res timer
//for error logging
    #include <stdio.h>//for FILE* used inside printf.h
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Definitions/defarg.h> //since LOG_ERROR macros use argument counting
    #include <Utils/error.h>
//for memory allocation
    #include <stdlib.h> //for malloc, calloc,realloc and exit()
    #include <Definitions/retcodes.h> //for error codes, logged in allocation failure
    #include <Utils/memory.h> //for refu memory allocation
//*---------------------System specific inclusion---------------------------------------
#include <unistd.h> //for linux sleep() functions
#include <errno.h>//for linux errno
#include <stdlib.h> //for setenv()
//*----------------------------End of Includes------------------------------------------


/*-------------------------------------------------------Sleep functions-----------------------------------------------------------------------------------------*/

// Suspends the calling thread for a number of seconds
void rfSleep(uint32_t seconds)
{
    sleep(seconds);
}
//Suspends the calling thread for a number of milliseconds
void rfSleep_ms(uint32_t milliseconds)
{
    usleep(milliseconds*1000);
}

/*-------------------------------------------------------Timer functions-----------------------------------------------------------------------------------------*/
#ifdef RF_MODULE_TIME_TIMER
//initializes a timer
char rfTimer_Init(RF_Timer* t,char resolution)
{
    //check if we can have high res timer
    if(rfSysInfo.hasHighResTimer == false)
    {
        LOG_ERROR("Error initializing a timer, the system does not support a high resolution timer",RE_TIMER_HIGHRES_UNSUPPORTED);
        return false;
    }
    //query the timer
    if( clock_gettime( rfSysInfo.timerType, &t->lastQuery) == -1 ) /// Do We really need this if check here? In refu_init we do check that the timer type works
    {
      LOG_ERROR("Error initializing a timer using POSIX, clock_gettime failed",RE_TIMER_POSIX);
      return false;
    }
    //keep the resolution
    switch(resolution)
    {
        case RF_TIMER_SECONDS:
        case RF_TIMER_MILLISECONDS:
        case RF_TIMER_MICROSECONDS:
        case RF_TIMER_NANOSECONDS:
            t->resolution = resolution;
        break;
        default:
            LOG_ERROR("Attempted to initialize a timer with an illegal resolution",RE_TIMER_ILLEGAL_RESOLUTION);
            return false;
        break;
    }
    //success
    return true;
}
// Allocates and returns a timer object
RF_Timer* rfTimer_Create(char resolution)
{
    RF_Timer* ret;
    //check if we can have high res timer
    if(rfSysInfo.hasHighResTimer == false)
    {
        LOG_ERROR("Error creating a timer, the system does not support a high resolution timer",RE_TIMER_HIGHRES_UNSUPPORTED);
        return 0;
    }
    RF_MALLOC(ret,sizeof(RF_Timer))
    //query the timer
    if( clock_gettime( rfSysInfo.timerType, &ret->lastQuery) == -1 ) /// Do We really need this if check here? In refu_init we do check that the timer type works
    {
      LOG_ERROR("Error creating a timer using POSIX, clock_gettime failed",RE_TIMER_POSIX);
      free(ret);
      return 0;
    }
    //keep the resolution
    switch(resolution)
    {
        case RF_TIMER_SECONDS:
        case RF_TIMER_MILLISECONDS:
        case RF_TIMER_MICROSECONDS:
        case RF_TIMER_NANOSECONDS:
            ret->resolution = resolution;
        break;
        default:
            LOG_ERROR("Attempted to initialize a timer with an illegal resolution",RE_TIMER_ILLEGAL_RESOLUTION);
            free(ret);
            return 0;
        break;
    }
    //success
    return ret;
}

// Destroys a timer
void rfTimer_Destroy(RF_Timer* t)
{
    free(t);
}

// Queries a timer
double rfTimer_Query(RF_Timer* t,char resolution)
{
    double result=0;
    struct timespec query;

    //if a change in resolution has been requested
    if(resolution != 0)
    {
        switch(resolution)
        {
            case RF_TIMER_SECONDS:
            case RF_TIMER_MILLISECONDS:
            case RF_TIMER_MICROSECONDS:
            case RF_TIMER_NANOSECONDS:
                t->resolution = resolution;
            break;
            default:
                LOG_ERROR("Attempted to query a timer with an illegal resolution. The query will be performed with the previously saved resolution",RE_TIMER_ILLEGAL_RESOLUTION);
            break;
        }
    }

    //query the timer
    if( clock_gettime( rfSysInfo.timerType, &query) == -1 ) /// Do We really need this if check here? In refu_init we do check that the timer type works
    {
        LOG_ERROR("Error querying a timer using POSIX, clock_gettime failed",RE_TIMER_POSIX);
        return 0;
    }

    struct timespec diff;
	if ((query.tv_nsec-t->lastQuery.tv_nsec)<0)
	{
		diff.tv_sec = query.tv_sec-t->lastQuery.tv_sec-1;
		diff.tv_nsec = 1000000000+query.tv_nsec-t->lastQuery.tv_nsec;
	}
	else
	{
		diff.tv_sec = query.tv_sec-t->lastQuery.tv_sec;
		diff.tv_nsec = query.tv_nsec-t->lastQuery.tv_nsec;
	}

	switch(t->resolution)
	{
	    case RF_TIMER_SECONDS:
            result = (double)diff.tv_sec + (double)diff.tv_nsec/1E9;
	    break;
        case RF_TIMER_MILLISECONDS:
            result = (double)diff.tv_sec*1E3 + (double)diff.tv_nsec/1E6;
	    break;
	    case RF_TIMER_MICROSECONDS:
            result = (double)diff.tv_sec*1E6 + (double)diff.tv_nsec/1E3;
	    break;
	    case RF_TIMER_NANOSECONDS:
            result = (double)diff.tv_sec*1E9 + (double)diff.tv_nsec;
	    break;
	}

    t->lastQuery = query;
    return result;

}

#endif



#ifdef RF_MODULE_TIME_DATE
/**==================================================================RF_Dates Functions=========================================================================================================*/

// Initializes a Date
char rfDate_Init_Now(RF_Date* d,char local)
{
    struct tm* date;
    //get the time
    time_t t = time(NULL);
    if(local == true)
        date = localtime(&t);

    else
        date = gmtime(&t);
    //error check
    if(date == NULL)
    {
        LOG_ERROR("There was an error in initializing a date Object in Linux. The system function returned NULL",RE_DATE_SYSTEM_ERROR);
        return false;
    }
    //read in the values
    if(date->tm_sec == 60)
        d->seconds = 59;
    else
        d->seconds = date->tm_sec;
    d->minutes = date->tm_min;
    d->hours = date->tm_hour;
    d->wDay = date->tm_wday; //is the same as the #defines
    d->mDay = date->tm_mday;//because linux has it 1-31
    d->yDay = date->tm_yday;
    d->month = date->tm_mon+1;//because linux has it from 0 to 11
    d->year = 1900+date->tm_year;//since in Linux it's the number of year since 1900
    //check for leap year
    LEAP_YEAR_CHECK(d->year,d->isLeap)

    //success
    return true;
}

//Creates a date
RF_Date* rfDate_Create_Now(char local)
{
    RF_Date* ret;
    RF_MALLOC(ret,sizeof(RF_Date))
    struct tm* date;
    //get the time
    time_t t = time(NULL);
    if(local == true)
        date = localtime(&t);

    else
        date = gmtime(&t);
    //error check
    if(date == NULL)
    {
        LOG_ERROR("There was an error in Creating a date Object in Linux. The system function returned NULL",RE_DATE_SYSTEM_ERROR);
        free(ret);
        return 0;
    }
    //read in the values
    if(date->tm_sec == 60)
        ret->seconds = 59;
    else
        ret->seconds = date->tm_sec;
    ret->minutes = date->tm_min;
    ret->hours = date->tm_hour;
    ret->wDay = date->tm_wday; //is the same as the #defines
    ret->mDay = date->tm_mday;//because linux has it 1-31
    ret->yDay = date->tm_yday;
    ret->month = date->tm_mon+1;//because linux has it from 0 to 11
    ret->year = 1900+date->tm_year;//since in Linux it's the number of year since 1900

    //check for leap year
    LEAP_YEAR_CHECK(ret->year,ret->isLeap)
    //success
    return ret;
}

// Sets this date as the current system date and time
char rfDate_SetToSystem(RF_Date* d)
{
    struct tm tm;
    tm.tm_sec = d->seconds;
    tm.tm_min = d->minutes;
    tm.tm_hour = d->hours;
    tm.tm_wday = d->wDay;
    tm.tm_mday = d->mDay;
    tm.tm_yday = d->yDay;
    tm.tm_mon = d->month-1;//because linux months are 0 to 11
    if(d->year >= 1900)
        tm.tm_year = d->year-1900;
    else
        tm.tm_year = 1900;
    tm.tm_isdst = 0; //temporary .. should also add it to my dates

    //create time_t object for system (UTC) time
    time_t t = mktime(&tm);
    if(t == -1)
    {
        LOG_ERROR("Setting the system time from an RF_Date in Linux failed because mktime() returned -1",RE_DATE_SET_SYSTEMTIME)
        return false;
    }
    //now set the system time
    if(stime(&t) != 0)
    {
        switch(errno)
        {
            case EFAULT:
                LOG_ERROR("Setting the system time from an RF_Date in Linux failed in stime(). Error in getting information from user space.",RE_DATE_SET_SYSTEMTIME);
            break;
            case EPERM:
                LOG_ERROR("Setting the system time from an RF_Date in Linux failed in stime().The calling process has insufficient privilege. Under Linux the CAP_SYS_TIME privilege is required.",RE_DATE_SET_SYSTEMTIME);
            break;
            default:
                LOG_ERROR("Setting the system time from an RF_Date in Linux failed in stime(). errno has unexpected value of %d",RE_DATE_SET_SYSTEMTIME,errno);
            break;
        }
        return false;
    }
    //success
    return true;
}

// Sets this date as the current local date and time
char rfDate_SetToLocal(RF_Date* d)
{
    struct tm tm;
    tm.tm_sec = d->seconds;
    tm.tm_min = d->minutes;
    tm.tm_hour = d->hours;
    tm.tm_wday = d->wDay;
    tm.tm_mday = d->mDay;
    tm.tm_yday = d->yDay;
    tm.tm_mon = d->month-1;//because linux months are 0 to 11
    if(d->year >= 1900)
        tm.tm_year = d->year-1900;
    else
        tm.tm_year = 1900;
    tm.tm_isdst = 0; //temporary .. should also add it to my dates

    //need to use the tz environment variable to get the time zone, so that we can create a local time with mktime
    char *tz;
    tz = getenv("TZ");
    setenv("TZ", "", 1);
    tzset();
    //create time_t object for Local Time
    time_t t = mktime(&tm);
    if(t == -1)
    {
        LOG_ERROR("Setting the Local System time from an RF_Date in Linux failed because mktime() returned -1",RE_DATE_SET_SYSTEMTIME)
        return false;
    }
    if (tz)
        setenv("TZ", tz, 1);
    else
        unsetenv("TZ");
    tzset();
    //now set the local time
    if(stime(&t) != 0)
    {
        switch(errno)
        {
            case EFAULT:
                LOG_ERROR("Setting the Local System time from an RF_Date in Linux failed in stime(). Error in getting information from user space.",RE_DATE_SET_SYSTEMTIME);
            break;
            case EPERM:
                LOG_ERROR("Setting the Local System time from an RF_Date in Linux failed in stime().The calling process has insufficient privilege. Under Linux the CAP_SYS_TIME privilege is required.",RE_DATE_SET_SYSTEMTIME);
            break;
            default:
                LOG_ERROR("Setting the Local System time from an RF_Date in Linux failed in stime(). errno has unexpected value of %d",RE_DATE_SET_SYSTEMTIME,errno);
            break;
        }
        return false;
    }
    //success
    return true;
}
#endif
