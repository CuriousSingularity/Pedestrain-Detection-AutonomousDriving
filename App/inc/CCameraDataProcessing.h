/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CCameraDataProcessing.h
 * Author          : Nicolas Ojeda Leon (stnioied@stdu.h-da.de)
 * 			Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Camera Detection Algorithm is implemented here.
 *
 ****************************************************************************/

#ifndef CCAMERADATAPROCESSING_H
#define CCAMERADATAPROCESSING_H

//System Include Files

//Own Include Files
#include "./OS/inc/CThread.h"
#include "./OS/inc/CSemaphore.h"

class CCameraDataProcessing : public CThread {
private:

	/**
	 * @brief : Main routine for the thread
	 *
	 * @param arg : Arguments for the routine
	 *
	 * @return - to join the thread
	 */
	void run();

public:

	/**
	 * @brief : Constructor
	 *
	 * @param sysResource : System resource 
	 */
	CCameraDataProcessing(int threadIndex, const CSystemResource *sysResource = NULL, CThread::start_routine_t entry = NULL, void *arg = NULL);

	/**
	 * @brief : Destructor
	 */
	~CCameraDataProcessing();

	/**
	 * @brief : 
	 *
	 * @param arg
	 *
	 * @return 
	 */
	friend void *friend_run_camera(void *arg);

};
/********************
 **  CLASS END
 *********************/
#endif /* CCAMERADATAPROCESSING_H */
