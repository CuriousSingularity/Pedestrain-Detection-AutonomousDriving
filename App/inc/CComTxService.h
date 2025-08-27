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

// System Include Files

// Own Include Files
#include "./App/inc/CSerialProtocol.h"
#include "./OS/inc/CMailBox.h"
#include "./OS/inc/CSemaphore.h"
#include "./OS/inc/CThread.h"

class CComTxService : public CThread {
  private:
    /**
     * @brief : UART channel 1
     */
    CUart m_uart_1;

    /**
     * @brief : Protocol parser object
     */
    CSerialProtocol m_Protocol;

    /**
     * @brief : Main routine for the thread
     *
     * @return - to join the thread
     */
    void run();

    global::RC_t processRecvdMsg(CMailBox::mail_box_data_t& data);

    global::RC_t processDataForTx(CMailBox::mail_box_data_t& data);

  public:
    /**
     * @brief : Constructor
     *
     * @param threadIndex 	: Thread Index
     */
    CComTxService(int threadIndex);

    /**
     * @brief : Destructor
     */
    ~CComTxService();
};
/********************
 **  CLASS END
 *********************/
#endif /* CCOMTXSERVICE_H */
