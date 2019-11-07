/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CComTxService.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Serial Data Processing thread - packet reception and processing
 * 			it with Service-ID, Local-ID; predefined protocol.
 *
 ****************************************************************************/


//System Include Files
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//Own Include Files
#include "./App/inc/CComTxService.h"
#include "./App/inc/CSerialProtocol.h"

//Namespace
using namespace std;
using namespace global;

//Method Implementations
/**
 * @brief : Constructor
 *
 * @param threadIndex 	: Thread Index
 * @param sysResource	: Global resource pointer
 * @param entry		: Entry function for the thread
 * @param arg		: Arguments to the thread
 */
CComTxService::CComTxService(int threadIndex, CThread::start_routine_t entry, void *arg) : 
		CThread(threadIndex, entry, arg), 
		m_uart_0("/dev/tty0"	, O_RDWR | O_NOCTTY | O_SYNC, S_IRWXU)
{
	// nothing
}


/**
 * @brief : Destructor
 */
CComTxService::~CComTxService()
{
	// nothing
}


/**
 * @brief : Main routine for the thread
 *
 * @return - to join the thread
 */
void CComTxService::run()
{
	// The Threads runs here
	cout << "INFO\t: Serial Thread " << this->getThreadIndex() << " started with ID : " << pthread_self() << endl;

	ssize_t wBytes = 0;

	while (1)
	{
		cout << "INFO\t: Serial Running Thread " << this->getThreadIndex() << " started with ID : " << pthread_self() << endl;

		for (uint8_t counter = 0; counter < 10; counter++)
			this->m_uart_0.write(&counter, counter, wBytes);

		sleep(1);
	}
}



/**
 * @brief : Friend function used to create the thread 
 *
 * @param arg : arguments to the thread
 *
 * @return 
 */
void *friend_com_tx_service(void *arg)
{
	cout << "INFO\t: Thread " << __func__ << endl;

	CComTxService *ptr = static_cast<CComTxService *>(arg);

	if (ptr)
	{
		ptr->run();
	}

	return NULL;
}
