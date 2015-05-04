/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef REFU_THREADING_LINUX_H
#define REFU_THREADING_LINUX_H

/*------------- Outside Module inclusion -------------*/
#include <Definitions/inline.h>
#include <Definitions/retcodes.h>
/*------------- System specific inclusion -------------*/
#include <pthread.h>
/*------------- End of includes -------------*/

struct RFmutex {
    pthread_mutex_t m;
};

i_INLINE_DECL bool rf_mutex_init(struct RFmutex *mutex)
{
    return pthread_mutex_init(&mutex->m, NULL) == 0;
}

i_INLINE_DECL bool rf_mutex_lock(struct RFmutex *mutex)
{
    return pthread_mutex_lock(&mutex->m) == 0;
}

i_INLINE_DECL bool rf_mutex_unlock(struct RFmutex *mutex)
{
    return pthread_mutex_unlock(&mutex->m) == 0;
}

i_INLINE_DECL bool rf_mutex_deinit(struct RFmutex *mutex)
{
    return pthread_mutex_destroy(&mutex->m) == 0;
}

#endif
