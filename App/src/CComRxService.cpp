/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CComRxService.h
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
#include "./OS/inc/CMailBox.h"
#include "./App/inc/CComRxService.h"
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
CComRxService::CComRxService(int threadIndex, CThread::start_routine_t entry, void *arg) : 
		CThread(threadIndex, entry, arg)
{
	// nothing
}


/**
 * @brief : Destructor
 */
CComRxService::~CComRxService()
{
	// nothing
}


/**
 * @brief : Main routine for the thread
 *
 * @return - to join the thread
 */
void CComRxService::run()
{
	// The Threads runs here
	cout << "INFO\t: Communication Rx Service " << this->getThreadIndex() << " started with ID : " << pthread_self() << endl;

	cout << "INFO\t: Running Communication Rx Service " << this->getThreadIndex() << " started with ID : " << pthread_self() << endl;

	// Mailboxes
	extern CMailBox g__Mailboxes[THREAD_TOTAL_COUNT];
	int msg_src_id = 0;

	CMailBox::mail_box_data_t msg_recv = {0};

	while (1)
	{
		if (g__Mailboxes[THREAD_COM_RX_SERVICE].receive(msg_src_id, msg_recv) != RC_SUCCESS)
			continue;
	}
}



/**
 * @brief : Friend function used to create the thread 
 *
 * @param arg : arguments to the thread
 *
 * @return 
 */
void *friend_com_rx_service(void *arg)
{
	cout << "INFO\t: Thread " << __func__ << endl;

	CComRxService *ptr = static_cast<CComRxService *>(arg);

	if (ptr)
	{
		ptr->run();
	}

	return NULL;
}
