/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CComTxService.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Communication transmission service for outbound data packets
 * 			Handles mailbox message reception and UART transmission with protocol formatting
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
     * @brief Primary UART communication interface
     * Hardware abstraction for serial communication
     */
    CUart m_primaryUart;

    /**
     * @brief Serial protocol handler
     * Manages packet formatting and protocol encoding/decoding
     */
    CSerialProtocol m_protocol;

    /**
     * @brief Main transmission thread routine
     * Waits for mailbox messages and transmits formatted data via UART
     * Runs continuously until thread termination
     */
    void run();

    /**
     * @brief Process received mailbox message
     * Routes message based on service ID and prepares for transmission
     * @param data Mailbox message data to process
     * @return RC_t Processing status code
     */
    global::RC_t processRecvdMsg(CMailBox::MailBoxData& data);

    /**
     * @brief Process data for transmission
     * Formats detection data and transmits via UART
     * @param data Mailbox data containing detection results
     * @return RC_t Transmission status code
     */
    global::RC_t processDataForTx(CMailBox::MailBoxData& data);

  public:
    /**
     * @brief Constructor
     * Initializes communication transmission service
     * @param threadIndex Unique thread identifier
     */
    CComTxService(int threadIndex);

    /**
     * @brief Destructor
     * Cleans up communication service resources
     */
    ~CComTxService();
};
/********************
 **  CLASS END
 *********************/
#endif /* CCOMTXSERVICE_H */
