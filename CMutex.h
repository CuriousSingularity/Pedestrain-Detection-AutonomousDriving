/***************************************************************************
*============= Copyright by Darmstadt University of Applied Sciences =======
****************************************************************************
* Filename        : CMUTEX.H
* Author          :
* Description     :
*
*
****************************************************************************/

#ifndef CMUTEX_H
#define CMUTEX_H

#include <pthread.h>

class CMutex {
private:
    int destroy();
    int init();
    pthread_mutex_t m_key;
    pthread_mutexattr_t m_attr;
public:

    CMutex();

    virtual ~CMutex();

    int lock();

    int trylock();

    void unlock();

};
/********************
**  CLASS END
*********************/
#endif /* CMUTEX_H */
