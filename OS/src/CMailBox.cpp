/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CMailBox.cpp
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Mutex file for protecting the resource.
 *
 ****************************************************************************/

// System Include Files
#include <iostream>

#include <errno.h>
#include <string.h>
#include <unistd.h>

// Own Include Files
#include "./Common/inc/Logger.h"
#include "./OS/inc/CMailBox.h"

// Namespace
using namespace global;
using namespace std;

// Method Implementations

/**
 * @brief : Constructor
 */
CMailBox::CMailBox(int owner) {
    this->m_status = service_UNAVAILABLE;
    this->m_owner = owner;

    this->configure();
}


/**
 * @brief : Destructor
 */
CMailBox::~CMailBox() {
    this->m_owner = 0;
}


int CMailBox::getOwner() {
    return (this->m_owner);
}


RC_t CMailBox::configure() {
    // create a pipe for unidirectional message queue
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        LOG_ERROR("CMailBox", "Message Queue creation failed for the owner " + std::to_string(this->m_owner) + " with errno " + std::to_string(errno));
        return RC_ERROR_MAILBOX_FAIL;
    }

    this->m_fd_r = pipefd[0];
    this->m_fd_w = pipefd[1];
    this->m_status = service_READY;

    return RC_SUCCESS;
}


RC_t CMailBox::send(int senderId, const MailBoxData& data) {
    if (this->m_status != service_READY)
        return RC_ERROR_INVALID_STATE;

    MailBoxMessage msg = {0};

    msg.src = senderId;
    msg.dst = this->m_owner;

    if (memcpy(&msg.data, &data, MAIL_BOX_DATA_SIZE) == NULL) {
        LOG_ERROR("CMailBox", "Message Queue copy failed for the owner " + std::to_string(this->m_owner) + " with errno " + std::to_string(errno));

        return RC_ERROR_MEMORY;
    }

    ssize_t wBytes = 0;

    if ((this->write(&msg, MAIL_BOX_MSG_SIZE, wBytes) != RC_SUCCESS) ||
        (wBytes != MAIL_BOX_MSG_SIZE)) {
        LOG_ERROR("CMailBox", "Mailbox write failed for thread " + std::to_string(this->m_owner));
        return RC_ERROR_MAILBOX_FAIL;
    }

    return RC_SUCCESS;
}


RC_t CMailBox::receive(int& senderId, MailBoxData& data) {
    if (this->m_status != service_READY)
        return RC_ERROR_INVALID_STATE;

    MailBoxMessage msg = {0};
    ssize_t rBytes = 0;

    // blocking-call
    if ((this->read(&msg, MAIL_BOX_MSG_SIZE, rBytes) != RC_SUCCESS) ||
        (rBytes != MAIL_BOX_MSG_SIZE)) {
        LOG_ERROR("CMailBox", "Mailbox read failed for thread " + std::to_string(this->m_owner));
        return RC_ERROR_MAILBOX_FAIL;
    }

    senderId = msg.src;

    if (memcpy(&data, &msg.data, MAIL_BOX_DATA_SIZE) == NULL) {
        LOG_ERROR("CMailBox", "Message Queue copy failed for the owner " + std::to_string(this->m_owner) + " with errno " + std::to_string(errno));

        return RC_ERROR_MEMORY;
    }

    return RC_SUCCESS;
}
