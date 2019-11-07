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
	 * @return - to join the thread
	 */
	void run();

public:

	/**
	 * @brief : Constructor
	 *
	 * @param threadIndex 	: Thread Index
	 * @param sysResource	: Global resource pointer
	 * @param entry		: Entry function for the thread
	 * @param arg		: Arguments to the thread
	 */
	CSerialDataProcessing(int threadIndex, const CSystemResource *sysResource = NULL, CThread::start_routine_t entry = NULL, void *arg = NULL);

	/**
	 * @brief : Destructor
	 */
	~CSerialDataProcessing();

	/**
	 * @brief : Friend function used to create the thread 
	 *
	 * @param arg : arguments to the thread
	 *
	 * @return 
	 */
	friend void *friend_run_comm(void *arg);

};
/********************
 **  CLASS END
 *********************/
#endif /* CSERIALDATAPROCESSING_H */
