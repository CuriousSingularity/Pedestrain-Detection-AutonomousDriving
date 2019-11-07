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
	uint16_t				m_readindex;

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

};


/********************************************[ Init Functions ]*****************************************/

template <typename DATA, uint16_t SIZE>
CRingBuffer<DATA, SIZE>::CRingBuffer()
{
	m_readindex = 0;
	m_writeindex = 0;
}

template<typename DATA, uint16_t SIZE>
void CRingBuffer<DATA, SIZE>::init()
{
	m_readindex = 0;
	m_writeindex = 0;
}


/********************************************[ Read / Write Functions ]*****************************************/

template <typename DATA, uint16_t SIZE>
global::RC_t CRingBuffer<DATA, SIZE>::readData(DATA* const data, fp_customCopy_t customFunction)
{
	// First check if buffer is empty:
	if (this->m_readindex == this->m_writeindex){
		return global::RC_ERROR_BUFFER_EMTPY;
	}

	// Check if the provided pointer is NULL:
	if (NULL == data){
		return global::RC_ERROR_NULL;
	}

	// If everything os correct, then write data:
	if (customFunction)
	{
		customFunction(*data, this->m_pBuffer[this->m_readindex]);
	}
	else
	{
		*data = this->m_pBuffer[this->m_readindex];
	}
	
	return global::RC_SUCCESS;
}

template <typename DATA, uint16_t SIZE>
global::RC_t CRingBuffer<DATA, SIZE>::writeData(DATA const& data, fp_customCopy_t customFunction)
{
	// Write the new data in the next position
	if (customFunction)
	{
		customFunction(this->m_pBuffer[this->m_writeindex], data);
	}
	else
	{
		this->m_pBuffer[this->m_writeindex] = data;
	}
	
	this->m_readindex = this->m_writeindex;					// Move read index to old write position
	this->m_writeindex = (this->m_writeindex+1) % SIZE;		// Move write index to next buffer's position

	return global::RC_SUCCESS;
}


#endif /* _CRingBuffer_H_ */
