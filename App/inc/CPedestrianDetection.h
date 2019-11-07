/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CPedestrianDetection.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Pedestrian Detection Application
 *
 ****************************************************************************/


#ifndef CPEDESTRIANDETECTION_H
#define CPEDESTRIANDETECTION_H

//System Include Files

//Own Include Files
#include "CSystemResource.h"
#include "CCameraDataProcessing.h"
#include "CSerialDataProcessing.h"

class CPedestrianDetection {
private:

	/**
	 * @brief : Global System Resource
	 */
	CSystemResource m_sysRes;

	/**
	 * @brief : Camera Algorithm thread
	 */
	CCameraDataProcessing m_thread_camera;

	/**
	 * @brief : Serial Communication thread
	 */
	CSerialDataProcessing m_thread_comm;
public:

	/**
	 * @brief : Constructor
	 */
	CPedestrianDetection();

	/**
	 * @brief : Destructor
	 */
	~CPedestrianDetection();

	/**
	 * @brief : Run function for the Pedestrain Detection Aapplication
	 */
	void run();
};
/********************
 **  CLASS END
 *********************/
#endif /* CPEDESTRIANDETECTION_H */
