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
#include <pthread.h>

//Own Include Files
#include "./global.h"

class CMutex {
private:

	/**
	 * @brief : Mutex key 
	 */
	pthread_mutex_t m_key;

	/**
	 * @brief : Mutex attributes
	 */
	pthread_mutexattr_t m_attr;

	/**
	 * @brief : Status of the Mutex
	 */
	global::service_state_t m_status;

	/**
	 * @brief : Mutex initialisation
	 *
	 * @return RC_t : status
	 */
	global::RC_t init();

	/**
	 * @brief : Mutex destroy
	 *
	 * @return RC_t : status
	 */
	global::RC_t destroy();
public:

	/**
	 * @brief : Constructor
	 */
	CMutex();

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
