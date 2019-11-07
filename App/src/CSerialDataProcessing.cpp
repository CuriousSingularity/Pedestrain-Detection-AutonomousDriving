/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CSerialDataProcessing.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Serial Data Processing thread - packet reception and processing
 * 			it with Service-ID, Local-ID; predefined protocol.
 *
 ****************************************************************************/


//System Include Files
#include <iostream>

//Own Include Files
#include "./App/inc/CSerialDataProcessing.h"

//Namespace
using namespace std;
using namespace global;

//Method Implementations
/**
 * @brief : Constructor
 *
 * @param sysResource : System resource 
 */
CSerialDataProcessing::CSerialDataProcessing(int threadIndex, const CSystemResource *sysResource, CThread::start_routine_t entry, void *arg) : CThread(threadIndex, sysResource, entry, arg)
{
	// nothing
}


/**
 * @brief : Destructor
 */
CSerialDataProcessing::~CSerialDataProcessing()
{
	// nothing
}


/**
 * @brief : Main routine for the thread
 *
 * @param arg : Arguments for the routine
 *
 * @return - to join the thread
 */
void CSerialDataProcessing::run()
{
	// The Threads runs here
	cout << "INFO\t: Thread " << this->getThreadIndex() << " started with ID : " << this->getThreadIndex() << endl;

	while (1)
	{
		cout << "INFO\t: Running Thread " << this->getThreadIndex() << " started with ID : " << this->getThreadIndex() << endl;
		sleep(1);
	}
}



void *friend_run_comm(void *arg)
{
	CSerialDataProcessing *ptr = static_cast<CSerialDataProcessing *>(arg);

	if (ptr)
	{
		ptr->run();
	}

	return NULL;
}
