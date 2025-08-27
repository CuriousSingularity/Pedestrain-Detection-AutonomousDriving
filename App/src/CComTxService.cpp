/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CComTxService.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Serial Data Processing thread - packet reception and processing
 * 			it with Service-ID, Local-ID; predefined protocol.
 *
 ****************************************************************************/


// System Include Files
#include <iostream>

#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

// Own Include Files
#include "./App/inc/CComTxService.h"
#include "./App/inc/CSerialProtocol.h"
#include "./Common/inc/Logger.h"

// Namespace
using namespace std;
using namespace global;

// Method Implementations
/**
 * @brief : Constructor
 *
 * @param threadIndex 	: Thread Index
 * @param sysResource	: Global resource pointer
 * @param entry		: Entry function for the thread
 * @param arg		: Arguments to the thread
 */
CComTxService::CComTxService(int threadIndex)
    : CThread(threadIndex, [this]() { this->run(); }),
      m_primaryUart("/dev/ttyTHS1", O_RDWR | O_NOCTTY | O_SYNC, S_IRWXU) {
    // nothing
}


/**
 * @brief : Destructor
 */
CComTxService::~CComTxService() {
    // nothing
}


/**
 * @brief : Main routine for the thread
 *
 * @return - to join the thread
 */
void CComTxService::run() {
    // The Threads runs here
    LOG_INFO("CComTxService", "Communication Tx Service " + std::to_string(this->getThreadIndex()) + " started with ID : " + std::to_string(pthread_self()));

    LOG_INFO("CComTxService", "Running Communication Tx Service " + std::to_string(this->getThreadIndex()) + " started with ID : " + std::to_string(pthread_self()));
    // Mailboxes
    extern CMailBox g__Mailboxes[THREAD_TOTAL_COUNT];

    int msg_src_id = 0;
    CMailBox::MailBoxData msg_recv = {0};

    while (1) {
        if (g__Mailboxes[THREAD_COM_TX_SERVICE].receive(msg_src_id, msg_recv) != RC_SUCCESS)
            continue;

        // check who sent the message
        switch (msg_src_id) {
        case THREAD_DETECTION_SERVICE:
            // process the message
            this->processRecvdMsg(msg_recv);
            break;

        default:
            break;
        }
    }
}


RC_t CComTxService::processDataForTx(CMailBox::MailBoxData& data) {
    CSerialProtocol::object_detection_frame_t* ptr =
        static_cast<CSerialProtocol::object_detection_frame_t*>(data.dynamicData);

    if (!ptr) {
        LOG_ERROR("CComTxService", "Invalid Memory used for Tx");
        return RC_ERROR_MEMORY;
    }

    char __tx_buf[PROTOCOL_BUF_MAX_SIZE];

    memset(__tx_buf, 0, sizeof(__tx_buf));

    uint8_t __blk_cnt = 0;
    uint16_t __tx_length = 0;

    __blk_cnt = ptr->blks.size();
    __blk_cnt = __blk_cnt > PAYLOAD_BLOCKS ? PAYLOAD_BLOCKS : __blk_cnt;
    __tx_buf[SOP_INDEX] = SOP;
    __tx_buf[DLC_INDEX] = DLC;

    if (__blk_cnt)
        memcpy(&__tx_buf[PAYLOAD_INDEX], ptr->blks.data(), __blk_cnt * BLOCK_SIZE);

    __tx_buf[EOP_INDEX] = EOP;

    __tx_length = SOP_SIZE + DLC_SIZE + DLC + EOP_SIZE;

    RC_t ret = RC_ERROR_INVALID;
    ssize_t wBytes = 0;

    switch (data.lid) {
    case CUart::UART_CHANNEL_1:
        ret = this->m_primaryUart.write(__tx_buf, __tx_length, wBytes);
        break;

    default:
        LOG_ERROR("CComTxService", "Invalid UART Channel Tx Request");
        break;
    }

    return ret;
}

RC_t CComTxService::processRecvdMsg(CMailBox::MailBoxData& data) {
    RC_t ret = RC_ERROR_INVALID;

    switch (data.sid) {
    case SID_TX_DATA:
        ret = this->processDataForTx(data);

        // release the resource
        delete ((CSerialProtocol::object_detection_frame_t*)data.dynamicData);

        break;

    default:
        break;
    }

    return ret;
}
