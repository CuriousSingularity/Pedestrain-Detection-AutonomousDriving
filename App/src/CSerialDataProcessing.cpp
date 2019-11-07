#include "CSerialDataProcessing.h"
#include "CSemaphore.h"

/***************************************************************************
*============= Copyright by Darmstadt University of Applied Sciences =======
****************************************************************************
* Filename        : CSERIALDATAPROCESSING.CPP
* Author          :
* Description     :
*
*
****************************************************************************/


//System Include Files


//Own Include Files

//Method Implementations

CSerialDataProcessing::CSerialDataProcessing() : CThread(0, NULL, NULL)
{
}

CSerialDataProcessing::~CSerialDataProcessing()
{
}

void * CSerialDataProcessing::run(void * arg){}
typename CThread::start_routine_t CSerialDataProcessing::getPointerToStartRoutine(){}
