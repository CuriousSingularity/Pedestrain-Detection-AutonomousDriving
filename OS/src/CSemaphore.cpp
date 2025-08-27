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
 * @param mode		: mode of the semaphore (unused in C++ std implementation)
 * @param value		: inital value of the semaphore
 */
CSemaphore::CSemaphore(std::string name, int mode, unsigned int value)
{
	this->m_name	= name;
	this->m_mode	= mode;
	this->m_value	= value;
	this->m_count	= value;

	cout << "INFO\t: Create semaphore " << this->m_name << endl;
}


/**
 * @brief : Destructor
 */
CSemaphore::~CSemaphore()
{
	cout << "INFO\t: Destroy semaphore " << this->m_name << endl;
}




/**
 * @brief : Blocking wait until event
 *
 * @return RC_t - status
 */
RC_t CSemaphore::wait()
{
	try
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cv.wait(lock, [this] { return m_count > 0; });
		m_count--;
		return RC_SUCCESS;
	}
	catch (const std::exception& e)
	{
		return RC_ERROR;
	}
}


/**
 * @brief : Non-Blocking wait until event
 *
 * @return RC_t - status
 */
RC_t CSemaphore::trywait()
{
	try
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		if (m_count > 0)
		{
			m_count--;
			return RC_SUCCESS;
		}
		return RC_ERROR;
	}
	catch (const std::exception& e)
	{
		return RC_ERROR;
	}
}


/**
 * @brief : Timed-Blocking wait until event
 *
 * @param timeout_ms	: timeout in milliseconds 
 *
 * @return RC_t - status
 */
RC_t CSemaphore::timedwait(unsigned int timeout_ms)
{
	try
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		if (m_cv.wait_for(lock, std::chrono::milliseconds(timeout_ms), [this] { return m_count > 0; }))
		{
			m_count--;
			return RC_SUCCESS;
		}
		return RC_ERROR_TIME_OUT;
	}
	catch (const std::exception& e)
	{
		return RC_ERROR;
	}
}

/**
 * @brief : Set event to wake up the blocked threads
 *
 * @return RC_t - status
 */
RC_t CSemaphore::post()
{
	try
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_count++;
		m_cv.notify_one();
		return RC_SUCCESS;
	}
	catch (const std::exception& e)
	{
		return RC_ERROR;
	}
}

