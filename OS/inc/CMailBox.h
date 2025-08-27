/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CMailBox.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Mutex file for protecting the resource.
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
    typedef struct {
        uint16_t sid;  // service identifier
        uint16_t lid;  // local identifier
        void* dynamicData;
    } MailBoxData;

    /**
     * @brief : Constructor
     */
    CMailBox(int owner);

    /**
     * @brief : Delete copy constructor and assignment operator
     */
    CMailBox(const CMailBox&) = delete;
    CMailBox& operator=(const CMailBox&) = delete;

    /**
     * @brief : Destructor
     */
    virtual ~CMailBox();

    global::RC_t send(int senderId, const MailBoxData& data);

    global::RC_t receive(int& senderId, MailBoxData& data);

    int getOwner();

  private:
    int m_owner;

    typedef struct {
        uint16_t src;          // source of data
        uint16_t dst;          // destination of data
        MailBoxData data;  // mail box data
    } MailBoxMessage;

    global::RC_t configure();
};

#define MAIL_BOX_DATA_SIZE (sizeof(MailBoxData))
#define MAIL_BOX_MSG_SIZE (sizeof(MailBoxMessage))
/********************
 **  CLASS END
 *********************/
#endif /* CMAILBOX_H */
