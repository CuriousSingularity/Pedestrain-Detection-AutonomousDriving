/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CMutex.cpp
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Mutex file for protecting the resource.
 *
 ****************************************************************************/

//System Include Files
#include <iostream>
#include <errno.h>

//Own Include Files
#include "CMutex.h"

//Namespace
using namespace global;
using namespace std;

//Method Implementations

/**
 * @brief : Constructor
 */
CMutex::CMutex()
{
	this->m_status = service_READY;
}


/**
 * @brief : Destructor 
 */
CMutex::~CMutex()
{
	this->m_status = service_UNDEFINED;
}




/**
 * @brief : acquire blocking mutex lock
 *
 * @return RC_t : status
 */
RC_t CMutex::lock()
{
	if (this->m_status != service_READY)
		return RC_ERROR_INVALID_STATE;

	try
	{
		m_mutex.lock();
		return RC_SUCCESS;
	}
	catch (const std::exception& e)
	{
		return RC_ERROR_CLOSE;
	}
}

/**
 * @brief : acquire non-blocking mutex lock
 *
 * @return RC_t : status
 */
RC_t CMutex::trylock()
{
	if (this->m_status != service_READY)
		return RC_ERROR_INVALID_STATE;

	if (m_mutex.try_lock())
		return RC_SUCCESS;
	else
		return RC_ERROR_CLOSE;
}


/**
 * @brief : release the mutex lock
 *
 * @return RC_t : status
 */
RC_t CMutex::unlock()
{
	if (this->m_status != service_READY)
		return RC_ERROR_INVALID_STATE;

	try
	{
		m_mutex.unlock();
		return RC_SUCCESS;
	}
	catch (const std::exception& e)
	{
		return RC_ERROR_CLOSE;
	}
}

