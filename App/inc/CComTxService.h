/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CComTxService.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Serial Data Processing thread - packet reception and processing
 * 			it with Service-ID, Local-ID; predefined protocol.
 *
 ****************************************************************************/


#ifndef CCOMTXSERVICE_H
#define CCOMTXSERVICE_H

//System Include Files

//Own Include Files
#include "./OS/inc/CThread.h"
#include "./OS/inc/CSemaphore.h"
#include "./App/inc/CSerialProtocol.h"

class CComTxService : public CThread {
private:

	/**
	 * @brief : UART channel 0
	 */
	CUart		m_uart_0;

	/**
	 * @brief : Protocol parser object
	 */
	CSerialProtocol		m_Protocol;

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
	 * @param entry		: Entry function for the thread
	 * @param arg		: Arguments to the thread
	 */
	CComTxService(int threadIndex, CThread::start_routine_t entry = NULL, void *arg = NULL);

	/**
	 * @brief : Destructor
	 */
	~CComTxService();

	/**
	 * @brief : Friend function used to create the thread 
	 *
	 * @param arg : arguments to the thread
	 *
	 * @return 
	 */
	friend void *friend_com_tx_service(void *arg);

};
/********************
 **  CLASS END
 *********************/
#endif /* CCOMTXSERVICE_H */
