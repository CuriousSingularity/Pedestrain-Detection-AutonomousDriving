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
 * @param pAttr			: Attributes
 * @param pSysRes		: Reference to the system resource which can be used by the threads
 */
CThread::CThread(int32_t threadIndex, pthread_attr_t *pAttr, start_routine_t thread_entry, CSystemResource *pSysRes)
{
	// if this is NULL, then the thread is create with default configuration
	// TODO: May explore the attributes to utilize CPU and optimizate for a better 
	// performance. Refer man pages in linux for more information :D
	// this->m_pAttr			= pAttr;
	this->m_pAttr			= 0;
	this->m_threadIndex		= threadIndex;
	this->m_threadId		= 0;
	this->m_pSysRes			= pSysRes;
	this->m_thread_entry		= thread_entry;
}


/**
 * @brief : Destructor
 */
CThread::~CThread()
{
	// Disconnect from the global resource
	this->m_pSysRes = 0;
}


/**
 * @brief : Get Pointer to the start routine for the thread
 *
 * @return : Pointer to start routine
 */
CThread::start_routine_t CThread::getPointerToStartRoutine()
{
	return this->m_thread_entry;
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
 * @return : status of the thread creation
 */
RC_t CThread::create()
{
	if (this->m_thread_entry == 0)
	{
		return RC_ERROR_NULL;
	}

	// Thread parameter arguments is made NULL since it is redandent
	// TODO: If parameter is needed for each threads independently, explore this API options
	if (pthread_create(&this->m_threadId, this->m_pAttr, this->m_thread_entry, NULL) != 0)
	{
		cout << "ERROR\t: Thread " << this->m_threadIndex << " creation failed with errno " << errno << endl;
	}

	return RC_SUCCESS;
}

