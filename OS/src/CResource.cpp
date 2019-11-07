/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CResource.cpp
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Resource class to interact with the system hardware.
 * 			eg: UART, Camera, FileSystem, MMC, I2C, SPI etc.
 *
 ****************************************************************************/


//System Include Files
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

//Own Include Files
#include "./OS/inc/CResource.h"

//Method Implementations
using namespace std;
using namespace global;

/**
 * @brief : Constructor
 *
 * @param devPath 	: device node path
 * @param oflag		: access mode flags
 * @param mode		: permissio mode
 */
CResource::CResource(string devPath, int flag, int mode)
{
	this->m_resNodePath 	= devPath;
	this->m_flags		= flag;
	this->m_mode		= mode;

	if (this->open() != RC_SUCCESS)
	{
		cout << "ERROR\t: Failed to open " << this->m_resNodePath << " with error code " << errno << endl;
	}
	else
	{
		cout << "INFO\t: Device " << this->m_resNodePath << " opened for operation" << endl;
	}
}


/**
 * @brief : Destructor
 */
CResource::~CResource()
{
	if (this->close() != RC_SUCCESS)
	{
		cout << "ERROR\t: Failed to close " << this->m_resNodePath << " with error code " << errno << endl;
	}
	else
	{
		cout << "INFO\t: Device " << this->m_resNodePath << " closed fom operation" << endl;
	}
}


/**
 * @brief : Open the device for reading or writing
 *
 * @return RC_t : status of open
 */
RC_t CResource::open()
{
	if (this->m_resNodePath.empty())
	{
		cout << "ERROR\t: Invalid Device node " << this->m_resNodePath << endl;

		return RC_ERROR_INVALID;
	}

	this->m_fd = ::open(this->m_resNodePath.c_str(), this->m_flags, this->m_mode);

	if (this->m_fd == -1)
	{
		return RC_ERROR_OPEN;
	}

	return (this->configure());
}


/**
 * @brief : Close the device
 *
 * @return RC_t : status of close
 */
RC_t CResource::close()
{
	RC_t ret = RC_ERROR_CLOSE;

	if(::close(this->m_fd) == 0)
	{
		ret = RC_SUCCESS;
	}

	return ret;
}


/**
 * @brief : Read from the device 
 *
 * @param buffer	: buffer to read the data from
 * @param nByte		: number of bytes to be read
 * @param rByte		: number of bytes actually read
 *
 * @return RC_t - status of read
 */
RC_t CResource::read(void *buffer, const size_t nByte, ssize_t &rByte)
{
	RC_t ret = RC_ERROR_READ_FAILS;

	if (buffer != 0)
	{
		if ((rByte = ::read(this->m_fd, buffer, nByte)) != -1)
		{
			ret = RC_SUCCESS;
		}
	}
	else
	{
		ret = RC_ERROR_NULL;
	}

	return ret;
}

/**
 * @brief : Write to the device 
 *
 * @param buffer	: buffer to write the data to
 * @param nByte		: number of bytes to be written
 * @param rByte		: number of bytes actually written
 *
 * @return RC_t - status of read
 */
RC_t CResource::write(const void *buffer, const size_t nByte, ssize_t &rByte)
{
	RC_t ret = RC_ERROR_WRITE_FAILS;

	if (buffer != 0)
	{
		if ((rByte = ::write(this->m_fd, buffer, nByte)) != -1)
		{
			ret = RC_SUCCESS;
		}
	}
	else
	{
		ret = RC_ERROR_NULL;
	}

	return ret;
}
