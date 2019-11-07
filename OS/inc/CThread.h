/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CThread.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Thread creation and handling for the process.
 *
 ****************************************************************************/


#ifndef CTHREAD_H
#define CTHREAD_H

//System Include Files
#include <pthread.h>

//Own Include Files
#include "./OS/inc/CSystemResource.h"

class CThread {
public:

	/**
	 * @brief : Typedef for the start routine of a thread 
	 *
	 * @param : void * arguments - POSIX specific
	 *
	 * @return : void * - thread join
	 */
	typedef void*(*start_routine_t)(void *);

	/**
	 * @brief : Constructor
	 *
	 * @param threadIndex		: Index for a thread
	 * @param pAttr			: Attributes
	 * @param pSysRes		: Reference to the system resource which can be used by the threads
	 * @param entry			: Entry Function for the thread
	 */
	CThread(int32_t threadIndex, const CSystemResource *pSysRes = NULL, CThread::start_routine_t entry = NULL, void *arg = NULL);

	/**
	 * @brief : Destructor
	 */
	virtual ~CThread();

	/**
	 * @brief : Get the Thread Index
	 *
	 * @return - Thread Index
	 */
	int32_t getThreadIndex();

	/**
	 * @brief : Pure virtual entry function for each thread
	 *
	 * @param arg			: POSIX specific
	 *
	 * @return 			: POSIX specific (check man page for more information :D )
	 */
	virtual void run() =0;

	/**
	 * @brief : Function which creates a thread based on the entry function provided
	 *
	 * @param pAttr			: Thread attributes
	 *
	 * @return 			: status of setup
	 */
	global::RC_t create(pthread_attr_t *pAttr);

private:

	/**
	 * @brief : Thread ID - unique for a thread in a process
	 */
	pthread_t m_threadId;

	/**
	 * @brief : Thread attributes such as priority, detachable, stack, etc
	 */
	pthread_attr_t m_attr;

	/**
	 * @brief : Thread index to monitor the threads
	 */
	int32_t m_threadIndex;

	void *m_pArg;
	/**
	 * @brief : entry function for a thread
	 */
	start_routine_t m_thread_entry;

	/**
	 * @brief : Pointer reference for the System resource
	 */
	CSystemResource *m_pSysRes;

};
/********************
 **  CLASS END
 *********************/
#endif /* CTHREAD_H */
