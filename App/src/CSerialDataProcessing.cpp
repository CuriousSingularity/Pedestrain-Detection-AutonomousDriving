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
 * @param threadIndex 	: Thread Index
 * @param sysResource	: Global resource pointer
 * @param entry		: Entry function for the thread
 * @param arg		: Arguments to the thread
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
 * @return - to join the thread
 */
void CSerialDataProcessing::run()
{
	// The Threads runs here
	cout << "INFO\t: Thread " << this->getThreadIndex() << " started with ID : " << pthread_self() << endl;

	while (1)
	{
		cout << "INFO\t: Running Thread " << this->getThreadIndex() << " started with ID : " << pthread_self() << endl;
		sleep(1);
	}
}



/**
 * @brief : Friend function used to create the thread 
 *
 * @param arg : arguments to the thread
 *
 * @return 
 */
void *friend_run_comm(void *arg)
{
	cout << "INFO\t: Thread " << __func__ << endl;

	CSerialDataProcessing *ptr = static_cast<CSerialDataProcessing *>(arg);

	if (ptr)
	{
		ptr->run();
	}

	return NULL;
}
