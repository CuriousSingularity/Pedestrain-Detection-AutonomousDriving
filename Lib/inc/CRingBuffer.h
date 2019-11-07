/**
* \file CRingBuffer.h
* \author Guna, Fromm
* \date 02-May-2016
*
* \brief CRingBuffer
*
* Template file to provide a ringbuffer for any data type.
*
* \note <notes>
* \todo <todos>
* \warning  Assumption of use: a protocol is read out before the next full protocol is added. Otherwise the eopFlag is cleared in a wrong manner
*
*  Changelog:\n
*  - 1.3; 17.12.18 Leon
*  			- Removed protocol handling, changed type of template size to uint16_t and made methods virtual to extend class
*  - 1.2; 27.02.17 Barth
*  			- Added Constructor without EOP
*  - 1.1; 16.8.16; Fromm
*            - Corrected length parameter in readProtocol
*  - 1.0; 2.5.16; Guna
*            - Initial release
*
* \copyright Copyright ©2016
* Department of electrical engineering and information technology, Hochschule Darmstadt - University of applied sciences (h_da). All Rights Reserved.
* Permission to use, copy, modify, and distribute this software and its documentation for educational, and research purposes in the context of non-commercial
* (unless permitted by h_da) and official h_da projects, is hereby granted for enrolled students of h_da, provided that the above copyright notice,
* this paragraph and the following paragraph appear in all copies, modifications, and distributions.
* Contact Prof.Dr.-Ing. Peter Fromm, peter.fromm@h-da.de, Birkenweg 8 64295 Darmstadt - GERMANY for commercial requests.
*
* \warning This software is a PROTOTYPE version and is not designed or intended for use in production, especially not for safety-critical applications!
* The user represents and warrants that it will NOT use or redistribute the Software for such purposes.
* This prototype is for research purposes only. This software is provided "AS IS," without a warranty of any kind.
*/



#ifndef _CRingBuffer_H_
#define _CRingBuffer_H_

#include "global.h"



template <typename DATA, uint16_t SIZE>
class CRingBuffer
{
protected:
	DATA					m_pBuffer[SIZE];
	uint16_t				m_writeindex;
	volatile	uint16_t	m_readindex;
	volatile 	uint16_t	m_fillLevel;		/*\Brief Fill level of the buffer. Number of positions holding content at any given point in time. Volatile because one write operation can be interrupted by a read operation (for example in the uart, the read operation is performed in an ISR) and that could lead to inconsistency in the fillLevel */

public:

	typedef void (*fp_customCopy_t)(DATA &lhs, const DATA &rhs);

	/********************************************[ Init Functions ]*****************************************/

	CRingBuffer();

	/**
	 * clears and initialises the ringbuffer
	 */
	void 		init();

	/*****************************************[ Read / Write Functions ]*************************************/

	/**
	 * Read the next data element from the ringbuffer
	 * Return false in case no data is available
	 */
	global::RC_t	readData(DATA* const data, fp_customCopy_t customFunction = NULL);

	/**
	 * Write data to the ringbuffer.
	 * Basic atomic write operation. After one element write, if write was possible, it is assumed as completed.
	 * Returns error in case ringbuffer is full
	 */
	global::RC_t 		writeData(DATA const& data, fp_customCopy_t customFunction = NULL);


	/********************************************[ Helper Functions ]***************************************/

	/**
	 * Return the number of filled elements.
	 * Indicates the number of elements than can be read.
	 * Intended to be used when reading elements.
	 */
	uint16_t 	getFilledCount();

	/**
	 * Return the number of available empty elements.
	 * Intended to use when writing to the buffer.
	 */
	uint16_t 	getEmptyCount();

};


/********************************************[ Init Functions ]*****************************************/

template <typename DATA, uint16_t SIZE>
CRingBuffer<DATA, SIZE>::CRingBuffer()
{
	m_readindex = 0;
	m_writeindex = 0;
	m_fillLevel = 0;
}

template<typename DATA, uint16_t SIZE>
void CRingBuffer<DATA, SIZE>::init()
{
	m_readindex = 0;
	m_writeindex = 0;
	m_fillLevel = 0;
}


/********************************************[ Read / Write Functions ]*****************************************/

template <typename DATA, uint16_t SIZE>
global::RC_t CRingBuffer<DATA, SIZE>::readData(DATA* const data, fp_customCopy_t customFunction)
{
	if(getFilledCount() > 0)
	{
		if (data == NULL){
			return global::RC_ERROR_NULL;
		}

		if (customFunction)
		{
			customFunction(*data, m_pBuffer[m_writeindex]);
		}
		else
		{
			*data = m_pBuffer[m_readindex];
		}

		m_readindex = (m_readindex+1) % SIZE;
		m_fillLevel--;
		return global::RC_SUCCESS;
	}
	return global::RC_ERROR_BUFFER_EMTPY;
}

template <typename DATA, uint16_t SIZE>
global::RC_t CRingBuffer<DATA, SIZE>::writeData(DATA const& data, fp_customCopy_t customFunction)
{
	if(getEmptyCount() > 0)
	{
		if (customFunction)
		{
			customFunction(m_pBuffer[m_writeindex], data);
		}
		else
		{
			(m_pBuffer[m_writeindex]) = data;
		}

		m_writeindex = (m_writeindex+1) % SIZE;
		m_fillLevel++;
		return global::RC_SUCCESS;
	}

	return global::RC_ERROR_BUFFER_FULL;
}



/********************************************[ Helper Functions ]*****************************************/


template <typename DATA, uint16_t SIZE>
uint16_t CRingBuffer<DATA, SIZE>::getFilledCount()
{
	return m_fillLevel;
}

template <typename DATA, uint16_t SIZE>
uint16_t CRingBuffer<DATA, SIZE>::getEmptyCount()
{
	return (SIZE-m_fillLevel);
}




#endif /* _CRingBuffer_H_ */
