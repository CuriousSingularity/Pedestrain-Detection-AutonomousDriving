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
#include <semaphore.h>

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
	 * @brief : semaphore
	 */
	sem_t m_sem;

	/**
	 * @brief : Initialises the semaphore
	 *
	 * @return RC_t - status of initialisation
	 */
	global::RC_t init();

	/**
	 * @brief : Destroys the semaphore
	 *
	 * @return RC_t - status of destruction
	 */
	global::RC_t destroy();

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
	 * @param abs_timeout	: absolute timeperiod 
	 *
	 * @return RC_t - status
	 */
	global::RC_t timedwait(const struct timespec &abs_timeout);

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
