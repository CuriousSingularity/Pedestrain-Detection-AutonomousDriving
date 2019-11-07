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
CResource::CResource(string devPath, int flag, mode_t mode)
{
	this->m_resNodePath 	= devPath;
	this->m_flags		= flag;
	this->m_mode		= mode;
	this->m_status		= service_UNDEFINED;

	if (this->open() != RC_SUCCESS)
	{
		this->m_status	= service_UNAVAILABLE;
		cout << "ERROR\t: Failed to open " << this->m_resNodePath << " with error code " << errno << endl;
	}
	else
	{
		this->m_status	= service_READY;
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
		this->m_status	= service_UNDEFINED;
		cout << "ERROR\t: Failed to close " << this->m_resNodePath << " with error code " << errno << endl;
	}
	else
	{
		this->m_status	= service_UNDEFINED;
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
	RC_t ret = RC_ERROR_OPEN;

	if (this->m_status != service_UNDEFINED)
		return RC_ERROR_INVALID_STATE;

	if (this->m_mutex_w.lock() != RC_SUCCESS)
	{
		cout << "ERROR\t: Resource lock failed for Device " << this->m_resNodePath << " during open" << endl;
		return RC_ERROR_BUSY;
	}

	if (this->m_resNodePath.empty())
	{
		cout << "ERROR\t: Invalid Device node " << this->m_resNodePath << endl;

		return RC_ERROR_INVALID;
	}

	this->m_fd = ::open(this->m_resNodePath.c_str(), this->m_flags, this->m_mode);

	if (this->m_fd == -1)
	{
		cout << "ERROR\t: Resource open failed for Device " << this->m_resNodePath << endl;
		
		ret = RC_ERROR_OPEN;
	}

	if (this->m_mutex_w.unlock() != RC_SUCCESS)
	{
		cout << "ERROR\t: Resource unlock failed for Device " << this->m_resNodePath << " during open" << endl;
	}

	return ret;
}


/**
 * @brief : Close the device
 *
 * @return RC_t : status of close
 */
RC_t CResource::close()
{
	RC_t ret = RC_ERROR_CLOSE;

	if (this->m_status != service_READY)
		return RC_ERROR_INVALID_STATE;

	if (this->m_mutex_w.lock() != RC_SUCCESS)
	{
		cout << "ERROR\t: Resource lock failed for Device " << this->m_resNodePath << " during close" << endl;
		return RC_ERROR_BUSY;
	}

	if(::close(this->m_fd) == 0)
	{
		ret = RC_SUCCESS;
	}

	if (this->m_mutex_w.unlock() != RC_SUCCESS)
	{
		cout << "ERROR\t: Resource unlock failed for Device " << this->m_resNodePath << " during close" << endl;
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

	if (this->m_status != service_READY)
		return RC_ERROR_INVALID_STATE;

	if (buffer)
	{
		if (this->m_mutex_r.trylock() != RC_SUCCESS)
		{
			cout << "ERROR\t: Resource lock failed for Device " << this->m_resNodePath << " during read" << endl;
			return RC_ERROR_BUSY;
		}

		if ((rByte = ::read(this->m_fd, buffer, nByte)) == -1)
		{
			cout << "ERROR\t: Read failed for Device " << this->m_resNodePath << endl;

			ret = RC_ERROR_READ_FAILS;
		}
		else
		{
			ret = RC_SUCCESS;
		}

		if (this->m_mutex_r.unlock() != RC_SUCCESS)
		{
			cout << "ERROR\t: Resource unlock failed for Device " << this->m_resNodePath << " during read" << endl;
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

	if (this->m_status != service_READY)
		return RC_ERROR_INVALID_STATE;

	if (buffer)
	{
		if (this->m_mutex_w.trylock() != RC_SUCCESS)
		{
			cout << "ERROR\t: Resource lock failed for Device " << this->m_resNodePath << " during write" << endl;
			return RC_ERROR_BUSY;
		}

		if ((rByte = ::write(this->m_fd, buffer, nByte)) == -1)
		{
			cout << "ERROR\t: Write failed for Device " << this->m_resNodePath << endl;

			ret = RC_ERROR_WRITE_FAILS;
		}
		else
		{
			ret = RC_SUCCESS;
		}

		if (this->m_mutex_w.unlock() != RC_SUCCESS)
		{
			cout << "ERROR\t: Resource unlock failed for Device " << this->m_resNodePath << " during write" << endl;
		}
	}
	else
	{
		ret = RC_ERROR_NULL;
	}

	return ret;
}


/**
 * @brief : Get the device node path
 *
 * @return string
 */
string CResource::getDeviceNode()
{
	return (this->m_resNodePath);
}


