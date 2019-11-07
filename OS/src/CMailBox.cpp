/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CMailBox.cpp
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Mutex file for protecting the resource.
 *
 ****************************************************************************/

//System Include Files
#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <string.h>

//Own Include Files
#include "./OS/inc/CMailBox.h"

//Namespace
using namespace global;
using namespace std;

//Method Implementations

/**
 * @brief : Constructor
 */
CMailBox::CMailBox(int owner)
{
	this->m_status 	= service_UNAVAILABLE;
	this->m_owner	= owner;

	this->configure();
}


/**
 * @brief : Destructor 
 */
CMailBox::~CMailBox()
{
	this->m_owner	= 0;
}


int CMailBox::getOwner()
{
	return (this->m_owner);
}


RC_t CMailBox::configure()
{
	// create a pipe for unidirectional message queue
	int pipefd[2];

	if (pipe(pipefd) == -1)
	{
		cout << "ERROR\t: Message Queue creation failed for the owner " << this->m_owner << " with errno " << errno << endl;
		return RC_ERROR_MAILBOX_FAIL;
	}

	this->m_fd_r = pipefd[0];
	this->m_fd_w = pipefd[1];
	this->m_status 	= service_READY;

	return RC_SUCCESS;
}


RC_t CMailBox::send(int senderId, const mail_box_data_t &data)
{
	if (this->m_status != service_READY)
		return RC_ERROR_INVALID_STATE;

	mail_box_msg_t msg = {0};

	msg.src 	= senderId;
	msg.dst 	= this->m_owner;

	if (memcpy(&msg.data, &data, MAIL_BOX_DATA_SIZE) == NULL)
	{
		cout << "ERROR\t: Message Queue copy failed for the owner " << this->m_owner << " with errno " << errno << endl;

		return RC_ERROR_MEMORY;
	}

	ssize_t wBytes = 0;

	if ((this->write(&msg, MAIL_BOX_MSG_SIZE, wBytes) != RC_SUCCESS)
			|| (wBytes != MAIL_BOX_MSG_SIZE))
	{
		cout << "ERROR\t: Mailbox write failed for thread " << this->m_owner << endl;
		return RC_ERROR_MAILBOX_FAIL;
	}

	return RC_SUCCESS;
}


RC_t CMailBox::receive(int &senderId, mail_box_data_t &data)
{
	if (this->m_status != service_READY)
		return RC_ERROR_INVALID_STATE;

	mail_box_msg_t msg = {0};
	ssize_t rBytes = 0;

	// blocking-call
	if ((this->read(&msg, MAIL_BOX_MSG_SIZE, rBytes) != RC_SUCCESS)
			|| (rBytes != MAIL_BOX_MSG_SIZE))
	{
		cout << "ERROR\t: Mailbox read failed for thread " << this->m_owner << endl;
		return RC_ERROR_MAILBOX_FAIL;
	}

	senderId = msg.src;

	if (memcpy(&data, &msg.data, MAIL_BOX_DATA_SIZE) == NULL)
	{
		cout << "ERROR\t: Message Queue copy failed for the owner " << this->m_owner << " with errno " << errno << endl;

		return RC_ERROR_MEMORY;
	}

	return RC_SUCCESS;
}
