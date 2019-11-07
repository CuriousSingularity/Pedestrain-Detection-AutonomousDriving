/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CCameraService.cpp
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Camera Service which fills up the request with an image
 *
 ****************************************************************************/


//System Include Files
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//Own Include Files
#include "./App/inc/CCameraService.h"

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
CCameraService::CCameraService(int threadIndex, CThread::start_routine_t entry, void *arg) : 
		CThread(threadIndex, entry, arg),
		m_camera_0("/dev/video0"	, O_RDWR | O_NOCTTY | O_SYNC, S_IRWXU)
{
	// nothing
}


/**
 * @brief : Destructor
 */
CCameraService::~CCameraService()
{
	// nothing
}


/**
 * @brief : Main routine for the thread
 *
 * @return - to join the thread
 */
void CCameraService::run()
{
	// The Threads runs here
	cout << "INFO\t: Camera Thread " << this->getThreadIndex() << " started with ID : " << pthread_self() << endl;

	while (1)
	{
		cout << "INFO\t: Camera Running Thread " << this->getThreadIndex() << " started with ID : " << pthread_self() << endl;
	}
}



/**
 * @brief : Friend function used to create the thread 
 *
 * @param arg : arguments to the thread
 *
 * @return 
 */
void *friend_camera_service(void *arg)
{
	cout << "INFO\t: Thread " << __func__ << endl;

	CCameraService *ptr = static_cast<CCameraService *>(arg);

	if (ptr)
	{
		ptr->run();
	}

	return NULL;
}
