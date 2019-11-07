#include "CCameraDataProcessing.h"
#include "CSemaphore.h"

/***************************************************************************
*============= Copyright by Darmstadt University of Applied Sciences =======
****************************************************************************
* Filename        : CCAMERADATAPROCESSING.CPP
* Author          :
* Description     :
*
*
****************************************************************************/


//System Include Files


//Own Include Files

//Method Implementations

CCameraDataProcessing::CCameraDataProcessing() : CThread(1, NULL, NULL)
{

}

CCameraDataProcessing::~CCameraDataProcessing()
{

}
void * CCameraDataProcessing::run(void * arg){}
typename CThread::start_routine_t CCameraDataProcessing::getPointerToStartRoutine(){}
