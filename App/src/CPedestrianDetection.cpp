/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CPedestrianDetection.cpp
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Pedestrian Detection Application
 *
 ****************************************************************************/


//System Include Files
#include <iostream>

//Own Include Files
#include "./App/inc/CPedestrianDetection.h"

//Namespace
using namespace std;
using namespace global;

extern void *friend_run_comm(void *);
extern void *friend_run_camera(void *);

//Method Implementations
/**
 * @brief : Constructor
 */
CPedestrianDetection::CPedestrianDetection() :	
	m_thread_camera	(0, &this->m_sysRes, friend_run_camera, &this->m_thread_camera),
	m_thread_comm	(1, &this->m_sysRes, friend_run_comm,   &this->m_thread_comm)
{
	//nothing
}


/**
 * @brief : Destructor
 */
CPedestrianDetection::~CPedestrianDetection()
{
	//nothing
}


/**
 * @brief : Run function for the Pedestrain Detection Aapplication
 */
void CPedestrianDetection::run()
{
	// Create the Threads
	if (this->m_thread_comm.create(0) != RC_SUCCESS)
	{
		cout << "ERROR\t: Failed to set up the Communication Thread " << this->m_thread_comm.getThreadIndex() << endl;
	}

	if (this->m_thread_camera.create(0) != RC_SUCCESS)
	{
		cout << "ERROR\t: Failed to set up the Camera Algorithm Thread " << this->m_thread_camera.getThreadIndex() << endl;
	}
}
