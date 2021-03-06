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
	this->m_status 	= service_UNAVAILABLE;
	this->m_key 	= PTHREAD_MUTEX_INITIALIZER;

	if (this->init() != RC_SUCCESS)
	{
		cout << "ERROR\t: Mutex initialisation failed with error code " << errno << endl;
	}
	else
	{
		this->m_status = service_READY;
	}
}


/**
 * @brief : Destructor 
 */
CMutex::~CMutex()
{
	if (this->destroy() != RC_SUCCESS)
	{
		cout << "ERROR\t: Mutex destroy failed with error code " << errno << endl;
	}
	else
	{
		this->m_status = service_UNDEFINED;
	}
}


/**
 * @brief : Mutex initialisation
 *
 * @return RC_t : status
 */
RC_t CMutex::init()
{
	if (pthread_mutex_init(&this->m_key, NULL) == 0)
		return RC_SUCCESS;
	else
		return RC_ERROR_OPEN;
}

/**
 * @brief : Mutex destroy
 *
 * @return RC_t : status
 */
RC_t CMutex::destroy()
{
	if (pthread_mutex_destroy(&this->m_key) == 0)
		return RC_SUCCESS;
	else
		return RC_ERROR_CLOSE;
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

	if (pthread_mutex_lock(&this->m_key) == 0)
		return RC_SUCCESS;
	else
		return RC_ERROR_CLOSE;
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

	if (pthread_mutex_trylock(&this->m_key) == 0)
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

	if (pthread_mutex_unlock(&this->m_key) == 0)
		return RC_SUCCESS;
	else
		return RC_ERROR_CLOSE;
}

