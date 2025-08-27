/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CThread.cpp
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Thread creation and handling for the process.
 *
 ****************************************************************************/


// System Include Files
#include <cstring>
#include <iostream>

// Own Include Files
#include "./OS/inc/CThread.h"

// Namespace
using namespace std;
using namespace global;

// Method Implementations
/**
 * @brief : Constructor
 *
 * @param threadIndex		: Index for a thread
 * @param entry			: Entry Function for the thread
 */
CThread::CThread(int32_t threadIndex, start_routine_t entry) {
    this->m_thread_entry = entry;
    this->m_threadIndex = threadIndex;
    this->m_created = false;
}


/**
 * @brief : Destructor
 */
CThread::~CThread() {
    if (m_created && m_thread.joinable()) {
        m_thread.join();
    }
}


/**
 * @brief : Get the Thread Index
 *
 * @return - Thread Index
 */
int32_t CThread::getThreadIndex() {
    return this->m_threadIndex;
}


/**
 * @brief : Get the Thread ID
 *
 * @return - Thread ID
 */
std::thread::id CThread::getThreadID() {
    return this->m_thread.get_id();
}


/**
 * @brief : Function which creates a thread based on the entry function provided
 *
 * @return 			: status of setup
 */
RC_t CThread::create() {
    // check if the thread is already created
    if (!this->m_created) {
        if (!this->m_thread_entry) {
            return RC_ERROR_NULL;
        }

        try {
            m_thread = std::thread(m_thread_entry);
            m_created = true;
            return RC_SUCCESS;
        } catch (const std::exception& e) {
            cout << "ERROR\t: Thread " << this->m_threadIndex << " creation failed: " << e.what()
                 << endl;
            return RC_ERROR;
        }
    } else {
        return RC_ERROR_BUSY;
    }
}

/**
 * @brief : Join the thread
 *
 * @return 			: status of join
 */
RC_t CThread::join() {
    try {
        if (m_created && m_thread.joinable()) {
            m_thread.join();
            return RC_SUCCESS;
        }
        return RC_ERROR_INVALID_STATE;
    } catch (const std::exception& e) {
        cout << "ERROR\t: Thread " << this->m_threadIndex << " join failed: " << e.what() << endl;
        return RC_ERROR;
    }
}

/**
 * @brief : Detach the thread
 *
 * @return 			: status of detach
 */
RC_t CThread::detach() {
    try {
        if (m_created && m_thread.joinable()) {
            m_thread.detach();
            return RC_SUCCESS;
        }
        return RC_ERROR_INVALID_STATE;
    } catch (const std::exception& e) {
        cout << "ERROR\t: Thread " << this->m_threadIndex << " detach failed: " << e.what() << endl;
        return RC_ERROR;
    }
}
