/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CCamera.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Camera class to capture frame or stream the video
 *
 ****************************************************************************/


#ifndef CCAMERA_H
#define CCAMERA_H

//System Include Files

//Own Include Files
#include "./OS/inc/CResource.h"

class CCamera : public CResource {
private:

	/**
	 * @brief : Configure the Camera interface
	 * 		- Framerate
	 * 		- Resolution
	 * 		- Orientation
	 * 		- ISO
	 * 		- Brightness
	 * 		- Contrast
	 * 		- Sharpness
	 * 		- Satureation
	 *
	 * @return RC_t : Status of the camera
	 */
	global::RC_t configure();

public:

	/**
	 * @brief : Constructor
	 */
	CCamera();

	/**
	 * @brief : Destructor
	 */
	virtual ~CCamera();

};
/********************
 **  CLASS END
 *********************/
#endif /* CCAMERA_H */
