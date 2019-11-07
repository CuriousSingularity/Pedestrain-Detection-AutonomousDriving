/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CApplication.cpp
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Pedestrian Detection Application
 *
 ****************************************************************************/


//System Include Files
#include <iostream>
#include <pthread.h>

//Own Include Files
#include "./OS/inc/CMailBox.h"
#include "./App/inc/CApplication.h"

//Namespace
using namespace std;
using namespace global;

extern void *friend_com_tx_service(void *);
extern void *friend_com_rx_service(void *);
extern void *friend_camera_service(void *);
extern void *friend_detection(void *);

//global Variable 
//Mailboxes for all the threads
CMailBox g__Mailboxes[THREAD_TOTAL_COUNT] = 
{
	CMailBox(THREAD_BACKGROUND),
	CMailBox(THREAD_COM_TX_SERVICE),
	CMailBox(THREAD_COM_RX_SERVICE),
	CMailBox(THREAD_CAMERA_SERVICE),
	CMailBox(THREAD_DETECTION_SERVICE),
};

//Method Implementations
/**
 * @brief : Constructor
 */
CApplication::CApplication() :	
	m_thread_com_tx_service	(THREAD_COM_TX_SERVICE, 	friend_com_tx_service, 	&this->m_thread_com_tx_service),
	m_thread_com_rx_service	(THREAD_COM_RX_SERVICE, 	friend_com_rx_service, 	&this->m_thread_com_rx_service),
	m_thread_camera_service	(THREAD_CAMERA_SERVICE, 	friend_camera_service, 	&this->m_thread_camera_service),
	m_thread_detection	(THREAD_DETECTION_SERVICE, 	friend_detection, 	&this->m_thread_detection)
{
	//nothing
}


/**
 * @brief : Destructor
 */
CApplication::~CApplication()
{
	//nothing
}


/**
 * @brief : Run function for the Pedestrain Detection Aapplication
 */
void CApplication::run()
{
	// Create the Threads
	if (this->m_thread_com_tx_service.create(0) != RC_SUCCESS)
	{
		cout << "ERROR\t: Failed to set up the Communication Tx Service Thread " << this->m_thread_com_tx_service.getThreadIndex() << endl;
	}

	if (this->m_thread_com_rx_service.create(0) != RC_SUCCESS)
	{
		cout << "ERROR\t: Failed to set up the Communication Rx Service Thread " << this->m_thread_com_rx_service.getThreadIndex() << endl;
	}

	if (this->m_thread_camera_service.create(0) != RC_SUCCESS)
	{
		cout << "ERROR\t: Failed to set up the Camera Service Thread " << this->m_thread_camera_service.getThreadIndex() << endl;
	}

	if (this->m_thread_detection.create(0) != RC_SUCCESS)
	{
		cout << "ERROR\t: Failed to set up the Detection Thread " << this->m_thread_detection.getThreadIndex() << endl;
	}

	while (1)
	{
		// background thread
		cout << "INFO\t: Running Background Thread 0 started with ID : " << pthread_self() << endl;
		sleep(1);
	}
}
