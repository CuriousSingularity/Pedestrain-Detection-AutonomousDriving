/***************************************************************************
*============= Copyright by Darmstadt University of Applied Sciences =======
****************************************************************************
* Filename        : CSERIALDATAPROCESSING.H
* Author          :
* Description     :
*
*
****************************************************************************/

#ifndef CSERIALDATAPROCESSING_H
#define CSERIALDATAPROCESSING_H
#include "CThread.h"
#include "CSemaphore.h"
class CSerialDataProcessing : public CThread {
private:
    CSemaphore* m_pSignalInputArrival;

    void * run(void * arg);
public:

    CSerialDataProcessing();

    ~CSerialDataProcessing();

    typename CThread::start_routine_t getPointerToStartRoutine();
};
/********************
**  CLASS END
*********************/
#endif /* CSERIALDATAPROCESSING_H */
