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

//Macros
#define DELIMITER			(0x7E)

// size
// change the paylock blocks according to the requirement
#define PAYLOAD_BLOCKS			10
#define BLOCK_SIZE			(sizeof(CSerialProtocol::object_detection_lidar_t))

#define DELIMITER_SIZE			1
#define NO_OF_BLOCKS			1
#define PAYLOAD_SIZE			(PAYLOAD_BLOCKS * BLOCK_SIZE)

#define DELIMITER_INDEX			0
#define NO_OF_BLOCKS_INDEX		(DELIMITER_INDEX + DELIMITER_SIZE)
#define PAYLOAD_INDEX			(NO_OF_BLOCKS_INDEX + NO_OF_BLOCKS)

#define PROTOCOL_BUF_SIZE		(DELIMITER_SIZE + NO_OF_BLOCKS + BLOCK_SIZE)

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
	uint8_t 	protocolBuf[PROTOCOL_BUF_SIZE];
	uint16_t	nBytes = 0, blocks = 0;
	ssize_t 	rBytes = 0;

	// protocol read
	nBytes = DELIMITER_SIZE;
	if ((ret = pComResource->read(&protocolBuf[DELIMITER_INDEX], nBytes, rBytes)) == RC_SUCCESS && (nBytes == rBytes))
	{
		// check if the 1st byte is delimiter
		if (protocolBuf[DELIMITER_INDEX] == DELIMITER)
		{
			// read the number of blocks
			nBytes = NO_OF_BLOCKS;
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

	return ret;
}
