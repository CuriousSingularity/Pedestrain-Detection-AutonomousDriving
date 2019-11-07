/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CCamera.cpp
 * Author          : 	Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * 			Nicolas Ojeda Leon (stniojed@stud.h-da.de)
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

//System Include Files
#include <iostream>
#include <unistd.h>

//Own Include Files
#include "./HAL/inc/CCamera.h"

//Namespace
using namespace std;
using namespace global;

// Local variables:
/*
 * Camera Configuration
 */
static const CCamera::configuation_t camera_config_param
{
	.capture_width	= RESOLUTION_CAPTURED_WIDTH,
	.capture_height	= RESOLUTION_CAPTURED_HEIGTH,
	.resized_width	= RESOLUTION_RESIZED_WIDTH,
	.resized_height	= RESOLUTION_RESIZED_HEIGTH,
	.framerate	= FRAMERATE,
	.orientation	= CCamera::ORIENT_DEG_180,
	.colour_format	= "BGR",
};


/**
 * gstreamer configuration to be used by OpenCV methods.
 * Local function intended to get the string for configuration of the camera stream
 */
static std::string gstreamer_pipeline (const CCamera::configuation_t &config)
{
	return "nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)" + std::to_string(config.capture_width) + ", height=(int)" +
		std::to_string(config.capture_height) + ", format=(string)NV12, framerate=(fraction)" + std::to_string(config.framerate) +
		"/1 ! nvvidconv flip-method=" + std::to_string(config.orientation) + " ! video/x-raw, width=(int)" + std::to_string(config.capture_width) + ", height=(int)" +
		std::to_string(config.capture_height) + ", format=(string)" + config.colour_format + "x ! videoconvert ! video/x-raw, format=(string)" + config.colour_format + " ! appsink";
}


//Method Implementations
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
global::RC_t CCamera::configure()
{
	cout << "INFO\t: Camera port " << this->getDeviceNode() << " configuration" << endl;

	if (this->m_status != service_READY)
		return RC_ERROR_INVALID_STATE;

	// open will first release the resource in case it was previously open.
	if (!this->m_cameraStream.open(gstreamer_pipeline(camera_config_param), cv::CAP_GSTREAMER))
		return RC_ERROR_OPEN;

	if (!this->m_cameraStream.isOpened())
		return RC_ERROR_INVALID_STATE;

	return RC_SUCCESS;
}

/**
 * @brief : Constructor
 */
CCamera::CCamera(string devPath, int flags, mode_t mode) : CResource(devPath, flags, mode)
{
	if (this->configure() != RC_SUCCESS)
	{
		this->m_status	= service_UNAVAILABLE;
		cout << "ERROR\t: Resource configuration failed for Device " << this->getDeviceNode() << endl;
	}

	cout << "INFO\t: Camera port " << this->getDeviceNode() << " constructed" << endl;
}


/**
 * @brief : Destructor
 */
CCamera::~CCamera()
{
	cout << "INFO\t: Camera port " << this->getDeviceNode() << " destructed" << endl;

	if (this->m_status != service_READY)
		return;

	// release the camera resource
	this->m_cameraStream.release();
}


/**
 * @brief : Captures a frame
 *
 * @param image : image / frame from the camera
 *
 * @return - status of capture
 */
RC_t CCamera::getCapture(cv::Mat * const image)
{
	if (this->m_status != service_READY)
	{
		cout << "ERROR\t: Camera hardware not ready " << this->getDeviceNode() << endl;
		return RC_ERROR_INVALID_STATE;
	}

	cv::Mat capturedImage;

	if (!this->m_cameraStream.read(capturedImage))
	{
		cout << "ERROR\t: Camera failed to read " << this->getDeviceNode() << endl;
		return RC_ERROR_READ_FAILS;
	}

	cv::resize(capturedImage, *image, cv::Size(camera_config_param.resized_width, camera_config_param.resized_height));

	return RC_SUCCESS;
}


/**
 * @brief : Read a frame from the camera
 *
 * @param buffer	: buffer should of type cv::Mat*
 * @param nByte		: ignore
 * @param wByte		: bytes read
 *
 * @return : status of read
 */
RC_t CCamera::read(const void *buffer, const size_t nByte, ssize_t &wByte)
{
	RC_t ret = RC_ERROR_READ_FAILS;

	if (!buffer)
		return RC_ERROR_NULL;

	cv::Mat *image = (cv::Mat *) buffer;

	if ((ret = this->getCapture(image)) == RC_SUCCESS)
		wByte = image->elemSize();

	return ret;
}

/**
 * @brief : write is not used for the camera
 *
 * @param buffer	: ignore
 * @param nByte		: ignore
 * @param wByte		: ignore
 *
 * @return : status will be read only
 */

RC_t CCamera::write(const void *buffer, const size_t nByte, ssize_t &wByte)
{
	return RC_ERROR_READ_ONLY;
}
