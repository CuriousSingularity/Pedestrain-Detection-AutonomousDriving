/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CSystemResource.cpp
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : System resource is a collection of all the resource used
 * 			for the particular application.
 *
 ****************************************************************************/


//System Include Files

//Own Include Files
#include "CSystemResource.h"
#include "CCom.h"
#include "CCamera.h"

//Namespace
using namespace std;
using namespace global;

//Method Implementations
/**
 * @brief : Constructor
 */
CSystemResource::CSystemResource()
{
	//nothing
}


/**
 * @brief : Destructor
 */
CSystemResource::~CSystemResource()
{
	//nothing
}


/**
 * @brief : Get the reference to the UART device
 *
 * @return - reference to the device
 */
CCom& CSystemResource::getSerialResourceReference()
{
	return (this->m_serial);
}


/**
 * @brief : Get the reference to the Camera device
 *
 * @return - reference to the device
 */
CCamera& CSystemResource::getCameraResourceReference()
{
	return (this->m_camera);
}

