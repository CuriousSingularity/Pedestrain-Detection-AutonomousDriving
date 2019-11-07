/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CCamera.cpp
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

//System Include Files
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

//Own Include Files
#include "./HAL/inc/CCamera.h"

//Namespace
using namespace std;
using namespace global;

// Local variables:
#if (TARGET_PLATFORM != PC)
/*
 *		Camera parameters
 *	(to check where to place - could be an enum with valid configuration to be set)
 *   Could even be attributes of the class to make it reconfigurable.
 *   Could be part of a config file
*/
// TODO: Check what to do with this camera parameters
static int capture_width = 800 ;
static int capture_height = 600 ;
static int display_width = 800 ;
static int display_height = 600 ;
static int framerate = 15 ;
static int flip_method = 0 ;

//Local helper functions:


	/**
	 * gstreamer configuration to be used by OpenCV methods.
	 * Local function intended to get the string for configuration of the camera stream
	 */
	static std::string gstreamer_pipeline ()
	{
		return "nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)" + std::to_string(capture_width) + ", height=(int)" +
			   std::to_string(capture_height) + ", format=(string)NV12, framerate=(fraction)" + std::to_string(framerate) +
			   "/1 ! nvvidconv flip-method=" + std::to_string(flip_method) + " ! video/x-raw, width=(int)" + std::to_string(display_width) + ", height=(int)" +
			   std::to_string(display_height) + ", format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
	}
#endif

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
	#if (TARGET_PLATFORM == NVIDIA)
		if (! this->m_cameraStream.open(gstreamer_pipeline(), cv::CAP_GSTREAMER))		// open will first release the resource in case it was previously open.
			return RC_ERROR;		// Error if not possible to open the camera stream.
	#elif (TARGET_PLATFORM == RSP)
		cout << "CCamera::configure: Implementation pending" << endl;
	#elif (TARGET_PLATFORM == PC)
		this->m_imageName = "img1.jpeg";
	#endif

	return RC_SUCCESS;
}

/**
 * @brief : Constructor
 */
CCamera::CCamera() : CResource("/dev/video0", O_RDWR | O_NOCTTY | O_SYNC, S_IRWXU)
{
	// Nothing
//	this->configure();
}


/**
 * @brief : Destructor
 */
CCamera::~CCamera()
{
	#if (TARGET_PLATFORM == NVIDIA)
		this->m_cameraStream.release();
	#elif (TARGET_PLATFORM == RSP)
		cout << "CCamera::configure: Implementation pending" << endl;
	#elif (TARGET_PLATFORM == PC)

	#endif

}

global::RC_t CCamera::getCapture(cv::Mat * const image){
	#if (TARGET_PLATFORM == NVIDIA)
		if (!this->m_cameraStream.isOpened())
			return RC_ERROR_INVALID_STATE;

		if (!this->m_cameraStream.read(*image))
			return RC_ERROR_READ_FAILS;
	#elif (TARGET_PLATFORM == RSP)
		cout << "CCamera::configure: Implementation pending" << endl;
	#elif (TARGET_PLATFORM == PC)
		*image = cv::imread(this->m_imageName);

		if(! image->data )
		{
			cout << "ERROR\t: Failed to open image " << this->m_imageName << endl;
			return RC_ERROR_READ_FAILS;
		}
	#endif

	return RC_SUCCESS;
}
