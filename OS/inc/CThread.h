/***************************************************************************
*============= Copyright by Darmstadt University of Applied Sciences =======
****************************************************************************
* Filename        : CTHREAD.H
* Author          :
* Description     :
*
*
****************************************************************************/

#ifndef CTHREAD_H
#define CTHREAD_H

#include "CSystemResource.h"
#include <pthread.h>

class CThread {
public:

    typedef void*(*start_routine_t)(void *);

    CThread(int32_t threadIndex, pthread_attr_t *pAttr, CSystemResource *pSysRes = NULL);

    virtual ~CThread();

    int32_t getThreadIndex();

    virtual void * run(void * arg) =0;

    virtual CThread::start_routine_t getPointerToStartRoutine();

private:
    pthread_t m_threadId;
    pthread_attr_t m_attr;
    start_routine_t m_thread_entry;
    int32_t m_threadIndex;
    CSystemResource *m_pSysRes;

};
/********************
**  CLASS END
*********************/
#endif /* CTHREAD_H */
