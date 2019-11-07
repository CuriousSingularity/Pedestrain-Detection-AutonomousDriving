/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CThread.cpp
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Thread creation and handling for the process.
 *
 ****************************************************************************/


//System Include Files
#include <iostream>
#include <cstring>

//Own Include Files
#include "./OS/inc/CThread.h"

//Namespace
using namespace std;
using namespace global;

//Method Implementations
/**
 * @brief : Constructor
 *
 * @param threadIndex		: Index for a thread
 * @param pSysRes		: Reference to the system resource which can be used by the threads
 * @param entry			: Entry Function for the thread
 */
CThread::CThread(int32_t threadIndex, const CSystemResource *pSysRes, start_routine_t entry, void *arg)
{
	// if this is NULL, then the thread is create with default configuration
	// TODO: May explore the attributes to utilize CPU and optimizate for a better 
	// performance. Refer man pages in linux for more information :D
	this->m_threadId		= 0;
	this->m_thread_entry		= entry;
	this->m_pArg			= arg;
	this->m_threadIndex		= threadIndex;
	this->m_pSysRes			= (CSystemResource *)pSysRes;
}


/**
 * @brief : Destructor
 */
CThread::~CThread()
{
	// Disconnect from the global resource
	this->m_pSysRes = 0;

	if (pthread_attr_destroy(&this->m_attr) != 0)
	{
		cout << "ERROR\t: Thread " << this->m_threadIndex << " setup failed due to bad attribute" << endl;
	}

	// pthread_exit - Thread exit can be done here, but we are not going to terminate our application, hence
	// not implemented here. If needed, this is the place to implement.
}


/**
 * @brief : Get the Thread Index
 *
 * @return - Thread Index
 */
int32_t CThread::getThreadIndex()
{
	return this->m_threadIndex;
}


/**
 * @brief : Function which creates a thread based on the entry function provided
 *
 * @param pAttr			: Thread attributes
 *
 * @return 			: status of setup
 */
RC_t CThread::create(pthread_attr_t *pAttr)
{
	// check if the thread is already created
	if (this->m_threadId == 0)
	{
		if (this->m_thread_entry == 0)
		{
			return RC_ERROR_NULL;
		}

		if (pAttr)
		{
			memcpy(&this->m_attr, pAttr, sizeof(pthread_attr_t));
		}

		if (pthread_attr_init(&this->m_attr) != 0)
		{
			cout << "ERROR\t: Thread " << this->m_threadIndex << " setup failed due to bad attribute" << endl;
			return RC_ERROR_BAD_PARAM;
		}

		// Thread parameter arguments is made NULL since it is redandent
		// TODO: If parameter is needed for each threads independently, explore this API options
		if (pthread_create(&this->m_threadId, &this->m_attr, this->m_thread_entry, NULL) != 0)
		{
			cout << "ERROR\t: Thread " << this->m_threadIndex << " creation failed with errno " << errno << endl;
			return RC_ERROR;
		}

		return RC_SUCCESS;
	}
	else
	{
		return RC_ERROR_BUSY;
	}
}
