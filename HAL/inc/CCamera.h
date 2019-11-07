/***************************************************************************
*============= Copyright by Darmstadt University of Applied Sciences =======
****************************************************************************
* Filename        : CCAMERA.H
* Author          :
* Description     :
*
*
****************************************************************************/

#ifndef CCAMERA_H
#define CCAMERA_H

//System Include Files
#include <opencv2/opencv.hpp>

//Own Include Files
#include "./OS/inc/CResource.h"
#include "./global.h"

class CCamera : public CResource {
private:
	#if (TARGET_PLATFORM == NVIDIA)
		cv::VideoCapture m_cameraStream;
	#elif (TARGET_PLATFORM == RSP)
		cout << "CCamera: Implementation pending" << endl;
	#elif (TARGET_PLATFORM == PC)
		std::string m_imageName;
	#endif

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

    global::RC_t getCapture(cv::Mat * const image);
};
/********************
**  CLASS END
*********************/
#endif /* CCAMERA_H */
