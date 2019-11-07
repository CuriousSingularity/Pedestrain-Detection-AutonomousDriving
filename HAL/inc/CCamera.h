/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CCamera.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Camera class to capture frame or stream the video
 *
 * ----- Changelog -----
 *
 * Start of user code changelog
 *
 *  - 1.0 ; 22-May-2019; Leon
 *            -  Implementation of the class' functionality.
 *            		Added specific code for camera frame acquisition
 *
 * End of user code
 ****************************************************************************/

#ifndef CCAMERA_H
#define CCAMERA_H

//System Include Files
#include <opencv2/opencv.hpp>

//Own Include Files
#include "./OS/inc/CResource.h"

class CCamera : public CResource {
private:
	#if ((TARGET_PLATFORM == NVIDIA) || (TARGET_PLATFORM == RSP))
		cv::VideoCapture m_cameraStream;
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

	/**
	 * @brief : Captures a frame
	 *
	 * @param image : image / frame from the camera
	 *
	 * @return - status of capture
	 */
	global::RC_t getCapture(cv::Mat * const image);

public:
	/**
	 * @brief : Orientation for the camera
	 */
	typedef enum 
	{
		// do not change the order
		ORIENT_DEG_0 	= 0,
		ORIENT_DEG_90	= 1,
		ORIENT_DEG_180	= 2,
		ORIENT_DEG_270	= 3,
	} orientation_e;

	/**
	 * @brief : Configuration parameter for the camera
	 */
	typedef struct 
	{
		unsigned int 	capture_width;
		unsigned int 	capture_height;
		unsigned int 	resized_width;
		unsigned int 	resized_height;
		unsigned int 	framerate;
		orientation_e 	orientation;
		std::string	colour_format;
	} configuation_t;

	/**
	 * @brief : Constructor
	 */
	CCamera(std::string devPath, int flag, mode_t mode);

	/**
	 * @brief : Destructor
	 */
	virtual ~CCamera();

	/**
	 * @brief : Read a frame from the camera
	 *
	 * @param buffer	: buffer should of type cv::Mat*
	 * @param nByte		: ignore
	 * @param wByte		: bytes read
	 *
	 * @return : status of read
	 */
	global::RC_t read(const void *buffer, const size_t nByte, ssize_t &wByte);

	/**
	 * @brief : write is not used for the camera
	 *
	 * @param buffer	: ignore
	 * @param nByte		: ignore
	 * @param wByte		: ignore
	 *
	 * @return : status will be read only
	 */
	global::RC_t write(const void *buffer, const size_t nByte, ssize_t &wByte);

};
/********************
 **  CLASS END
 *********************/
#endif /* CCAMERA_H */
