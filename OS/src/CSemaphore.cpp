/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CSemaphore.cpp
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Semaphore implementation for synchronisation between 
 * 			threads or thread and resource
 *
 ****************************************************************************/

//System Include Files
#include <iostream>

//Own Include Files
#include "./OS/inc/CSemaphore.h"

//Namespaces
using namespace std;
using namespace global;

//Method Implementations
/**
 * @brief : Constructor
 *
 * @param name		: name of the semaphore
 * @param mode		: mode of the semaphore
 * @param value		: inital value of the semaphore
 */
CSemaphore::CSemaphore(std::string name, int mode, unsigned int value)
{
	this->m_name	= name;
	this->m_mode	= mode;
	this->m_value	= value;

	if (this->init() != RC_SUCCESS)
	{
		cout << "ERROR\t: Failed to create a semaphore " << this->m_name << endl;
	}
	else
	{
		cout << "INFO\t: Create semaphore " << this->m_name << endl;
	}
}


/**
 * @brief : Destructor
 */
CSemaphore::~CSemaphore()
{
	if (this->destroy() != RC_SUCCESS)
	{
		cout << "ERROR\t: Failed to destroy a semaphore " << this->m_name << endl;
	}
	else
	{
		cout << "INFO\t: Destroy semaphore " << this->m_name << endl;
	}
}


/**
 * @brief : Initialises the semaphore
 *
 * @return RC_t - status of initialisation
 */
RC_t CSemaphore::init()
{
	RC_t ret = RC_ERROR;

	if (::sem_init(&this->m_sem, this->m_mode, this->m_value) == 0)
	{
		ret = RC_SUCCESS;
	}

	return ret;
}

/**
 * @brief : Destroys the semaphore
 *
 * @return RC_t - status of destruction
 */
RC_t CSemaphore::destroy()
{
	RC_t ret = RC_ERROR;

	if (::sem_destroy(&this->m_sem) == 0)
	{
		ret = RC_SUCCESS;
	}

	return ret;
}


/**
 * @brief : Blocking wait until event
 *
 * @return RC_t - status
 */
RC_t CSemaphore::wait()
{
	RC_t ret = RC_ERROR;

	if (::sem_wait(&this->m_sem) == 0)
	{
		ret = RC_SUCCESS;
	}

	return ret;
}


/**
 * @brief : Non-Blocking wait until event
 *
 * @return RC_t - status
 */
RC_t CSemaphore::trywait()
{
	RC_t ret = RC_ERROR;

	if (::sem_trywait(&this->m_sem) == 0)
	{
		ret = RC_SUCCESS;
	}

	return ret;
}


/**
 * @brief : Timed-Blocking wait until event
 *
 * @param abs_timeout	: absolute timeperiod 
 *
 * @return RC_t - status
 */
RC_t CSemaphore::timedwait(const struct timespec &abs_timeout)
{
	RC_t ret = RC_ERROR_TIME_OUT;

	if (::sem_timedwait(&this->m_sem, &abs_timeout) == 0)
	{
		ret = RC_SUCCESS;
	}

	return ret;


}

/**
 * @brief : Set event to wake up the blocked threads
 *
 * @return RC_t - status
 */
RC_t CSemaphore::post()
{
	RC_t ret = RC_ERROR;

	if (::sem_post(&this->m_sem) == 0)
	{
		ret = RC_SUCCESS;
	}

	return ret;
}

