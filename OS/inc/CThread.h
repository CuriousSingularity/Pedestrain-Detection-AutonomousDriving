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
#include <thread>
#include <functional>

//Own Include Files
#include "./global.h"

class CThread {
public:

	/**
	 * @brief : Typedef for the start routine of a thread 
	 *
	 * @param : void * arguments - C++ standard
	 *
	 * @return : void
	 */
	typedef std::function<void()> start_routine_t;

	/**
	 * @brief : Constructor
	 *
	 * @param threadIndex		: Index for a thread
	 * @param entry			: Entry Function for the thread
	 */
	CThread(int32_t threadIndex, CThread::start_routine_t entry = nullptr);

	/**
	 * @brief : Delete copy constructor and assignment operator
	 */
	CThread(const CThread&) = delete;
	CThread& operator=(const CThread&) = delete;

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
	 * @brief : Get the Thread ID
	 *
	 * @return - Thread ID
	 */
	std::thread::id getThreadID();

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
	 * @return 			: status of setup
	 */
	global::RC_t create();

	/**
	 * @brief : Join the thread
	 *
	 * @return 			: status of join
	 */
	global::RC_t join();

	/**
	 * @brief : Detach the thread
	 *
	 * @return 			: status of detach
	 */
	global::RC_t detach();

private:

	/**
	 * @brief : Thread object
	 */
	std::thread m_thread;

	/**
	 * @brief : Thread index to monitor the threads
	 */
	int32_t m_threadIndex;

	/**
	 * @brief : entry function for a thread
	 */
	start_routine_t m_thread_entry;

	/**
	 * @brief : flag to track if thread is created
	 */
	bool m_created;

};
/********************
 **  CLASS END
 *********************/
#endif /* CTHREAD_H */
