/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CMutex.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Mutex file for protecting the resource.
 *
 ****************************************************************************/

#ifndef CMUTEX_H
#define CMUTEX_H

//System Include Files
#include <mutex>

//Own Include Files
#include "./global.h"

class CMutex {
private:

	/**
	 * @brief : Mutex object 
	 */
	std::mutex m_mutex;

	/**
	 * @brief : Status of the Mutex
	 */
	global::service_state_t m_status;

public:

	/**
	 * @brief : Constructor
	 */
	CMutex();

	/**
	 * @brief : Delete copy constructor and assignment operator
	 */
	CMutex(const CMutex&) = delete;
	CMutex& operator=(const CMutex&) = delete;

	/**
	 * @brief : Destructor 
	 */
	virtual ~CMutex();

	/**
	 * @brief : acquire blocking mutex lock
	 *
	 * @return RC_t : status
	 */
	global::RC_t lock();

	/**
	 * @brief : acquire non-blocking mutex lock
	 *
	 * @return RC_t : status
	 */
	global::RC_t trylock();

	/**
	 * @brief : release the mutex lock
	 *
	 * @return RC_t : status
	 */
	global::RC_t unlock();

};
/********************
 **  CLASS END
 *********************/
#endif /* CMUTEX_H */
