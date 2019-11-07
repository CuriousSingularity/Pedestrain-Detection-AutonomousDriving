/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CApplication.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Pedestrian Detection Application
 *
 ****************************************************************************/


#ifndef CAPPLICATION_H
#define CAPPLICATION_H

//System Include Files

//Own Include Files
#include "./App/inc/CDetection.h"
#include "./App/inc/CComTxService.h"
#include "./App/inc/CComRxService.h"
#include "./App/inc/CCameraService.h"

class CApplication {
private:

	/**
	 * @brief : Communication Tx Service thread
	 */
	CComTxService m_thread_com_tx_service;

	/**
	 * @brief : Communication Rx Service thread
	 */
	CComRxService m_thread_com_rx_service;

	/**
	 * @brief : Camera Service thread
	 */
	CCameraService m_thread_camera_service;

	/**
	 * @brief : Detection Algorithm thread
	 */
	CDetection m_thread_detection;

public:

	/**
	 * @brief : Constructor
	 */
	CApplication();

	/**
	 * @brief : Destructor
	 */
	~CApplication();

	/**
	 * @brief : Run function for the Pedestrain Detection Aapplication
	 */
	void run();
};
/********************
 **  CLASS END
 *********************/
#endif /* CAPPLICATION_H */
