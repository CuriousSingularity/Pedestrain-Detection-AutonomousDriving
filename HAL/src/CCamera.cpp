#include "CCamera.h"
#include <unistd.h>
/***************************************************************************
*============= Copyright by Darmstadt University of Applied Sciences =======
****************************************************************************
* Filename        : CCAMERA.CPP
* Author          :
* Description     :
*
*
****************************************************************************/


//System Include Files
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//Own Include Files

//Method Implementations

int CCamera::configure()
{
	return -1;
}

CCamera::CCamera() : CResource("/dev/camera", O_RDWR | O_NOCTTY | O_SYNC, S_IRWXU) 
{
}

CCamera::~CCamera()
{
}
