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
#include <string.h>

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
		m_uart_1("/dev/ttyTHS1"	, O_RDWR | O_NOCTTY | O_SYNC, S_IRWXU)
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
	cout << "INFO\t: Communication Tx Service " << this->getThreadIndex() << " started with ID : " << pthread_self() << endl;

	cout << "INFO\t: Running Communication Tx Service " << this->getThreadIndex() << " started with ID : " << pthread_self() << endl;
	// Mailboxes
	extern CMailBox g__Mailboxes[THREAD_TOTAL_COUNT];

	int msg_src_id = 0;
	CMailBox::mail_box_data_t msg_recv = {0};

	while (1)
	{
		if (g__Mailboxes[THREAD_COM_TX_SERVICE].receive(msg_src_id, msg_recv) != RC_SUCCESS)
			continue;

		// check who sent the message
		switch (msg_src_id)
		{
			case THREAD_DETECTION_SERVICE:
				// process the message
				this->processRecvdMsg(msg_recv);
				break;

			default:
				break;
		}
	}
}


RC_t CComTxService::processDataForTx(CMailBox::mail_box_data_t &data)
{
	CSerialProtocol::object_detection_frame_t *ptr = static_cast<CSerialProtocol::object_detection_frame_t *>(data.pDynamicData);

	if (!ptr)
	{
		cout << "ERROR\t: Invalid Memory used for Tx " << endl;
		return RC_ERROR_MEMORY;
	}

	char __tx_buf[PROTOCOL_BUF_MAX_SIZE];
	uint8_t __blk_cnt = 0;
	uint16_t __tx_length = 0;

	__blk_cnt = ptr->blks.size();
	__blk_cnt			= __blk_cnt > PAYLOAD_BLOCKS ? PAYLOAD_BLOCKS : __blk_cnt;
	__tx_buf[DELIMITER_INDEX] 	= DELIMITER;
	__tx_buf[NO_OF_BLOCKS_INDEX]	= __blk_cnt;

	if (__blk_cnt)
		memcpy(&__tx_buf[PAYLOAD_INDEX], ptr->blks.data(), __blk_cnt * BLOCK_SIZE);

	__tx_length = DELIMITER_SIZE + LENGTH_SIZE + __blk_cnt * BLOCK_SIZE;

	RC_t ret = RC_ERROR_INVALID;
	ssize_t wBytes = 0;

	switch (data.lid)
	{
		case CUart::UART_CHANNEL_1:
			ret = this->m_uart_1.write(__tx_buf, __tx_length, wBytes);
			break;

		default:
			cout << "ERROR\t: Invalid UART Channel Tx Request " << endl;
			break;
	}

	return ret;
}

RC_t CComTxService::processRecvdMsg(CMailBox::mail_box_data_t &data)
{
	RC_t ret = RC_ERROR_INVALID;

	switch (data.sid)
	{
		case SID_TX_DATA:
			ret = this->processDataForTx(data);
			break;

		default:
			break;
	}

	return ret;
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
