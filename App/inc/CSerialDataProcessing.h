/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CSerialDataProcessing.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Serial Data Processing thread - packet reception and processing
 * 			it with Service-ID, Local-ID; predefined protocol.
 *
 ****************************************************************************/


#ifndef CSERIALDATAPROCESSING_H
#define CSERIALDATAPROCESSING_H

//System Include Files

//Own Include Files
#include "./OS/inc/CThread.h"
#include "./OS/inc/CSemaphore.h"

class CSerialDataProcessing : public CThread {
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
	CSerialDataProcessing(int threadIndex, const CSystemResource *sysResource = NULL, CThread::start_routine_t entry = NULL, void *arg = NULL);

	/**
	 * @brief : Destructor
	 */
	~CSerialDataProcessing();

	/**
	 * @brief : 
	 *
	 * @param arg
	 *
	 * @return 
	 */
	friend void *friend_run_comm(void *arg);

};
/********************
 **  CLASS END
 *********************/
#endif /* CSERIALDATAPROCESSING_H */
