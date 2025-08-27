/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CSemaphore.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Semaphore implementation for synchronisation between 
 * 			threads or thread and resource
 *
 ****************************************************************************/


#ifndef CSEMAPHORE_H
#define CSEMAPHORE_H

//System Include Files
#include <string>
#include <mutex>
#include <condition_variable>
#include <chrono>

//Own Include Files
#include "./global.h"

class CSemaphore {
private:

	/**
	 * @brief : Semaphore name
	 */
	std::string m_name;

	/**
	 * @brief : Semaphore mode - between threads of a process / between processes
	 */
	int m_mode;

	/**
	 * @brief : Initial value of the semaphore
	 */
	unsigned int m_value;

	/**
	 * @brief : mutex for condition variable
	 */
	std::mutex m_mutex;

	/**
	 * @brief : condition variable
	 */
	std::condition_variable m_cv;

	/**
	 * @brief : current count
	 */
	unsigned int m_count;

public:

	/**
	 * @brief : Constructor
	 *
	 * @param name		: name of the semaphore
	 * @param mode		: mode of the semaphore
	 * @param value		: inital value of the semaphore
	 */
	CSemaphore(std::string name, int mode, unsigned int value = 1);

	/**
	 * @brief : Delete copy constructor and assignment operator
	 */
	CSemaphore(const CSemaphore&) = delete;
	CSemaphore& operator=(const CSemaphore&) = delete;

	/**
	 * @brief : Destructor
	 */
	~CSemaphore();

	/**
	 * @brief : Blocking wait until event
	 *
	 * @return RC_t - status
	 */
	global::RC_t wait();

	/**
	 * @brief : Non-Blocking wait until event
	 *
	 * @return RC_t - status
	 */
	global::RC_t trywait();

	/**
	 * @brief : Timed-Blocking wait until event
	 *
	 * @param timeout_ms	: timeout in milliseconds 
	 *
	 * @return RC_t - status
	 */
	global::RC_t timedwait(unsigned int timeout_ms);

	/**
	 * @brief : Set event to wake up the blocked threads
	 *
	 * @return RC_t - status
	 */
	global::RC_t post();
};
/********************
 **  CLASS END
 *********************/
#endif /* CSEMAPHORE_H */
