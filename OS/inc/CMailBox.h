/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CMailBox.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Inter-process communication mailbox system for thread messaging
 *
 ****************************************************************************/

#ifndef CMAILBOX_H
#define CMAILBOX_H

// System Include Files

// Own Include Files
#include "./OS/inc/CResource.h"
#include "./global.h"


class CMailBox : protected CResource {
  public:
    /**
     * @brief Mailbox message data structure
     * Contains service and local identifiers along with optional dynamic data
     */
    typedef struct {
        uint16_t sid;           ///< Service identifier for message routing
        uint16_t lid;           ///< Local identifier for sub-service routing  
        void* dynamicData;      ///< Pointer to dynamic payload data (optional)
    } MailBoxData;

    /**
     * @brief Constructor
     * @param owner Thread ID that owns this mailbox instance
     */
    CMailBox(int owner);

    /**
     * @brief Deleted copy operations
     * Mailboxes cannot be copied to prevent resource conflicts
     */
    CMailBox(const CMailBox&) = delete;
    CMailBox& operator=(const CMailBox&) = delete;

    /**
     * @brief Destructor  
     * Cleans up mailbox resources and resets owner
     */
    virtual ~CMailBox();

    /**
     * @brief Send message to mailbox
     * @param senderId ID of the sending thread
     * @param data Message data to send
     * @return RC_t Status code indicating send operation result
     */
    global::RC_t send(int senderId, const MailBoxData& data);

    /**
     * @brief Receive message from mailbox (blocking)
     * @param senderId Reference to store sender's thread ID
     * @param data Reference to store received message data
     * @return RC_t Status code indicating receive operation result
     */
    global::RC_t receive(int& senderId, MailBoxData& data);

    /**
     * @brief Get mailbox owner thread ID
     * @return int Thread ID of the mailbox owner
     */
    int getOwner();

  private:
    int m_owner;                ///< Thread ID of mailbox owner

    /**
     * @brief Internal message structure for mailbox communication
     * Contains routing information and payload data
     */
    typedef struct {
        uint16_t src;           ///< Source thread ID
        uint16_t dst;           ///< Destination thread ID  
        MailBoxData data;       ///< Message payload data
    } MailBoxMessage;

    /**
     * @brief Configure mailbox resources
     * @return RC_t Configuration status
     */
    global::RC_t configure();
};

/// Size of mailbox data structure in bytes
#define MAIL_BOX_DATA_SIZE (sizeof(MailBoxData))
/// Size of complete mailbox message structure in bytes
#define MAIL_BOX_MSG_SIZE (sizeof(MailBoxMessage))
/********************
 **  CLASS END
 *********************/
#endif /* CMAILBOX_H */
