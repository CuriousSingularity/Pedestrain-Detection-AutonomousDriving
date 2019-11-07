/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CCameraDataProcessing.h
 * Author          : Nicolas Ojeda Leon (stnioied@stdu.h-da.de)
 * 			Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Camera Detection Algorithm is implemented here.
 *
 ****************************************************************************/



//System Include Files
#include <iostream>

//Own Include Files
#include "./App/inc/CCameraDataProcessing.h"

//Namespace
using namespace std;
using namespace global;

//Method Implementations
/**
 * @brief : Constructor
 *
 * @param sysResource : System resource 
 */
CCameraDataProcessing::CCameraDataProcessing(int threadIndex, const CSystemResource *sysResource, CThread::start_routine_t entry, void *arg) : CThread(threadIndex, sysResource, entry, arg)
{
	//nothing
}


/**
 * @brief : Destructor
 */
CCameraDataProcessing::~CCameraDataProcessing()
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
void CCameraDataProcessing::run()
{
	// The Threads runs here
	cout << "INFO\t: Thread " << this->getThreadIndex() << " started with ID : " << this->getThreadIndex() << endl;

	while (1)
	{
		cout << "INFO\t: Running Thread " << this->getThreadIndex() << " started with ID : " << this->getThreadIndex() << endl;
		sleep(1);
	}
}


/**
 * @brief : 
 *
 * @param arg
 *
 * @return 
 */
void *friend_run_camera(void *arg)
{
	CCameraDataProcessing *ptr = static_cast<CCameraDataProcessing *>(arg);

	if (ptr)
	{
		ptr->run();
	}

	return NULL;
}
