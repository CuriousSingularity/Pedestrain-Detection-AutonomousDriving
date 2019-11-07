/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CCameraService.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Serial Data Processing thread - packet reception and processing
 * 			it with Service-ID, Local-ID; predefined protocol.
 *
 ****************************************************************************/


#ifndef CCAMERASERVICE_H
#define CCAMERASERVICE_H

//System Include Files

//Own Include Files
#include "./OS/inc/CThread.h"
#include "./OS/inc/CSemaphore.h"
#include "./HAL/inc/CCamera.h"

class CCameraService : public CThread {
private:

	/**
	 * @brief : Camera channel 0
	 */
	CCamera		m_camera_0;

	/**
	 * @brief : Main routine for the thread
	 *
	 * @return - to join the thread
	 */
	void run();

	static int signal_type;

	static void __camera_cyclic__signal_handler(int sig);

public:

	/**
	 * @brief : Constructor
	 *
	 * @param threadIndex 	: Thread Index
	 * @param entry		: Entry function for the thread
	 * @param arg		: Arguments to the thread
	 */
	CCameraService(int threadIndex, CThread::start_routine_t entry = NULL, void *arg = NULL);

	/**
	 * @brief : Destructor
	 */
	~CCameraService();

	/**
	 * @brief : Friend function used to create the thread 
	 *
	 * @param arg : arguments to the thread
	 *
	 * @return 
	 */
	friend void *friend_camera_service(void *arg);

};
/********************
 **  CLASS END
 *********************/
#endif /* CCAMERASERVICE_H */
