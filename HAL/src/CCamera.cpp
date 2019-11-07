/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CCamera.cpp
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Camera class to capture frame or stream the video
 *
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

//Method Implementations
/**
 * @brief : Constructor
 */
CCamera::CCamera() : CResource("/dev/video0", O_RDWR | O_NOCTTY | O_SYNC, S_IRWXU) 
{
	if (this->configure() != RC_SUCCESS)
	{
		cout << "ERROR\t: Failed to configure the camera " << this->getDeviceNode() << endl;
	}
	else
	{
		cout << "INFO\t: Configured the camera " << this->getDeviceNode() << endl;
	}
}


/**
 * @brief : Destructor
 */
CCamera::~CCamera()
{
	// nothing
}

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
RC_t CCamera::configure()
{
	return RC_ERROR_INVALID_STATE;
}
