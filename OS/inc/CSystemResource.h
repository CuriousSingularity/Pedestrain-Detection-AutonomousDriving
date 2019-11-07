/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CSystemResource.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : System resource is a collection of all the resource used
 * 			for the particular application.
 *
 ****************************************************************************/


#ifndef CSYSTEMRESOURCE_H
#define CSYSTEMRESOURCE_H

//System Include Files
#include <string>

//Own Include Files
#include "./HAL/inc/CCom.h"
#include "./HAL/inc/CCamera.h"

class CSystemResource {
private:

	/**
	 * @brief : UART Class
	 */
	CCom m_serial;

	/**
	 * @brief : Camera Class
	 */
	CCamera m_camera;

public:
	/**
	 * @brief : Constructor
	 */
	CSystemResource();

	/**
	 * @brief : Destructor
	 */
	~CSystemResource();

	/**
	 * @brief : Get the reference to the UART device
	 *
	 * @return - pointer reference to the device
	 */
	CCom *getSerialResourceReference();

	/**
	 * @brief : Get the reference to the Camera device
	 *
	 * @return - pointer reference to the device
	 */
	CCamera *getCameraResourceReference();

};
/********************
 **  CLASS END
 *********************/
#endif /* CSYSTEMRESOURCE_H */
