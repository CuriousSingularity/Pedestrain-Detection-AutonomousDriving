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
#include <thread>

//Own Include Files
#include "./OS/inc/CMailBox.h"
#include "./App/inc/CApplication.h"

//Namespace
using namespace std;
using namespace global;


//global Variable 
//Mailboxes for all the threads
CMailBox g__Mailboxes[THREAD_TOTAL_COUNT] = 
{
	{THREAD_BACKGROUND},
	{THREAD_COM_TX_SERVICE},
	{THREAD_COM_RX_SERVICE},
	{THREAD_CAMERA_SERVICE},
	{THREAD_DETECTION_SERVICE},
};

//Method Implementations
/**
 * @brief : Constructor
 */
CApplication::CApplication() :	
	m_thread_com_tx_service	(THREAD_COM_TX_SERVICE),
	m_thread_com_rx_service	(THREAD_COM_RX_SERVICE),
	m_thread_camera_service	(THREAD_CAMERA_SERVICE),
	m_thread_detection	(THREAD_DETECTION_SERVICE)
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
	if (this->m_thread_com_tx_service.create() != RC_SUCCESS)
	{
		cout << "ERROR\t: Failed to set up the Communication Tx Service Thread " << this->m_thread_com_tx_service.getThreadIndex() << endl;
	}

	if (this->m_thread_com_rx_service.create() != RC_SUCCESS)
	{
		cout << "ERROR\t: Failed to set up the Communication Rx Service Thread " << this->m_thread_com_rx_service.getThreadIndex() << endl;
	}

	if (this->m_thread_camera_service.create() != RC_SUCCESS)
	{
		cout << "ERROR\t: Failed to set up the Camera Service Thread " << this->m_thread_camera_service.getThreadIndex() << endl;
	}

	if (this->m_thread_detection.create() != RC_SUCCESS)
	{
		cout << "ERROR\t: Failed to set up the Detection Thread " << this->m_thread_detection.getThreadIndex() << endl;
	}

	cout << "INFO\t: Running Background Thread 0 started with ID : " << std::this_thread::get_id() << endl;
	while (1)
	{
		// background thread
		sleep(1);
	}
}
