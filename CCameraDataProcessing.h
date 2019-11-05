/***************************************************************************
*============= Copyright by Darmstadt University of Applied Sciences =======
****************************************************************************
* Filename        : CCAMERADATAPROCESSING.H
* Author          :
* Description     :
*
*
****************************************************************************/

#ifndef CCAMERADATAPROCESSING_H
#define CCAMERADATAPROCESSING_H
#include "CThread.h"
#include "CSemaphore.h"
class CCameraDataProcessing : public CThread {
private:
    CSemaphore* m_pSignalInputArrival;

    void * run(void * arg);
public:

    CCameraDataProcessing();

    ~CCameraDataProcessing();

    typename CThread::start_routine_t getPointerToStartRoutine();
};
/********************
**  CLASS END
*********************/
#endif /* CCAMERADATAPROCESSING_H */
