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

//System Include Files

//Own Include Files
#include "./global.h"
#include "./OS/inc/CResource.h"

class CMailBox : protected CResource{
public:

	typedef struct 
	{
		uint16_t 	sid;	// service identifier
		uint16_t 	lid;	// local identifier
		void		*pDynamicData;
	} mail_box_data_t;

	/**
	 * @brief : Constructor
	 */
	CMailBox(int owner);

	/**
	 * @brief : Destructor 
	 */
	virtual ~CMailBox();

	global::RC_t send(int senderId, const mail_box_data_t &data);
	
	global::RC_t receive(int &senderId, mail_box_data_t &data);

private:

	int m_owner;

	typedef struct 
	{
		uint16_t 			src;	// source of data
		uint16_t 			dst;	// destination of data
		mail_box_data_t		data;	// mail box data
	} mail_box_msg_t;

};

#define MAIL_BOX_DATA_SIZE		(sizeof(mail_box_data_t))
#define MAIL_BOX_MSG_SIZE		(sizeof(mail_box_msg_t))
/********************
 **  CLASS END
 *********************/
#endif /* CMAILBOX_H */
