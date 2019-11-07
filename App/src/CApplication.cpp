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
#include "./App/inc/CApplication.h"

//Namespace
using namespace std;
using namespace global;

extern void *friend_com_tx_service(void *);
extern void *friend_camera_service(void *);
extern void *friend_detection(void *);

//Method Implementations
/**
 * @brief : Constructor
 */
CApplication::CApplication() :	
	m_thread_com_tx_service	(1, friend_com_tx_service, 	&this->m_thread_com_tx_service),
	m_thread_camera_service	(2, friend_camera_service, 	&this->m_thread_camera_service),
	m_thread_detection		(3, friend_detection, 		&this->m_thread_detection)
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
		cout << "ERROR\t: Failed to set up the Communication Service Thread " << this->m_thread_com_tx_service.getThreadIndex() << endl;
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
		cout << "INFO\t: Running Thread 0 started with ID : " << pthread_self() << endl;
		sleep(1);
	}
}
