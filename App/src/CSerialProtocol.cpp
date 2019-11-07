/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CSerialProtocol.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Protocol parser from the serial port
 *
 ****************************************************************************/


//System Include Files
#include <string.h>

//Own Include Files
#include "./App/inc/CSerialProtocol.h"

using namespace std;
using namespace global;

//Method Implementations

/**
 * @brief : Constructor
 */
CSerialProtocol::CSerialProtocol()
{
	// nothing
}

/**
 * @brief : Destructor
 */
CSerialProtocol::~CSerialProtocol()
{
	// nothing
}


/**
 * @brief : Read the lidar detected objects
 *
 * @param detectedObjects : Objects from the lidar are filled into the vector
 *
 * @return : status
 */
RC_t CSerialProtocol::readRequest(std::vector<object_detection_lidar_t> &detectedObjects, CUart *pComResource)
{
	RC_t		ret = RC_ERROR_NOT_MATCH;
#if 0
	uint8_t 	protocolBuf[PROTOCOL_BUF_MAX_SIZE];
	uint16_t	nBytes = 0, blocks = 0;
	ssize_t 	rBytes = 0;

	// protocol read
	nBytes = SOP_SIZE;
	if ((ret = pComResource->read(&protocolBuf[SOP_INDEX], nBytes, rBytes)) == RC_SUCCESS && (nBytes == rBytes))
	{
		// check if the 1st byte is delimiter
		if (protocolBuf[SOP_INDEX] == SOP)
		{
			// read the number of blocks
			nBytes = LENGTH_SIZE;
			if ((ret = pComResource->read(&protocolBuf[NO_OF_BLOCKS_INDEX], nBytes, rBytes)) == RC_SUCCESS && (nBytes == rBytes))
			{
				blocks = protocolBuf[NO_OF_BLOCKS_INDEX];

				if (blocks >= PAYLOAD_BLOCKS)
					return RC_ERROR_OVERRUN;

				// read each blocks into the vector
				for (uint16_t index = 0; index < blocks; index++)
				{
					nBytes = BLOCK_SIZE;

					if ((ret = pComResource->read(&protocolBuf[PAYLOAD_INDEX], nBytes, rBytes)) == RC_SUCCESS && (nBytes == rBytes))
					{
						object_detection_lidar_t 	each;

						memcpy(&each, &protocolBuf[PAYLOAD_INDEX], BLOCK_SIZE);
						detectedObjects.push_back(each);
					}
					else
					{
						break;
					}
				}
			}
		}
	}

#endif
	return ret;
}
