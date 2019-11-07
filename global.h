/**
 * \file	global.h
 * \author	Linde Material Handling
 * \author	Prof. P. Fromm	Hochschule Darmstadt
 * \author	Thomas Barth 	Hochschule Darmstadt - thomas.barth@h-da.de
 * \date 	04.09.2017
 * \version	0.7
 *
 * \brief Global definitions for datatypes and definitions
 *
 *	Changelog:\n
 *	- 0.7 04.09.17 Barth
 *              - Added ERIKA OS
 *              - Made OS Selection available for all architectures
 *              -
 *	- 0.6 12.01.17 Barth
 *              - Added #BIT Macro
 *	- 0.5 13.12.16 Barth
 *				- Advanced documentation
 *				- removed STDLIB and STRING from global headers
 *				- cleaned up backward compatibility symbols. U16 etc.
 *
 *	- 0.4 05.07.16 Barth
 *				- Added CEIL Macros\n\n
 *
 *	- 0.3 02.06.16 Barth
 * 				- Removed symbol TC29XB\n
 *		        - Added uint32_t_MAX\n
 *		        - Removed inline functions: _min, _max, _ldmst, _getA11, _swap. Moved _minu to driver.\n
 *		        - Adapted file to work on both, ARUIX and PSoC\n
 *		        - Added RC_t enums: RC_ERROR_BAD_PARAM, RC_ERROR_BAD_DATA, RC_ERROR_CANCELED, RC_ERROR_INVALID_STATE, RC_UNKNOWN\n\n
 *
 * \copyright Copyright �2016
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


#ifndef GLOBAL_H_
#define GLOBAL_H_

//-------------------------------------------------------------------- [global includes]
//#include "derivate.h"

//-------------------------------------------------------------------- [OS selection]

namespace global {
#define OS_NONE         0                           	/**< \brief target runs without operationg system*/
#define OS_PXROS        1                           	/**< \brief target runs PxROS*/
#define OS_FREERTOS     2                           	/**< \brief target runs FreeRTOS*/
#define OS_ERIKA        3                           	/**< \brief target runs ERIKA OS*/
#define OS_LINUX	4				/**< \brief target runs Linux OS*/
#define OS              OS_LINUX                    	/**< \brief OS selection*/

//-------------------------------------------------------------------- [Target specific includes]
#define ARCHITECTURE	ARCH_ARM

//-------------------------------------------------------------------- [global enumerations]

/**
 * \brief Return types for RTE and non RTE functions, to indicate the result of a operation
 */
enum eRC
{
	RC_SUCCESS              =  0,                   /**< \brief Function was successfully completed.                            	*/
	RC_ERROR                = -1,                   /**< \brief Non specific error.                                             	*/
	RC_ERROR_NULL           = -2,                   /**< \brief A pointer was NULL when a non-NULL pointer was expected.        	*/
	RC_ERROR_ZERO           = -3,                   /**< \brief A value was zero when no zero can be accepted.                  	*/
	RC_ERROR_MEMORY         = -4,                   /**< \brief Out of memory.                                                  	*/
	RC_ERROR_RANGE          = -5,                   /**< \brief A value was out of Range.                                       	*/
	RC_ERROR_OVERRUN        = -6,                   /**< \brief A value was too big.                                            	*/
	RC_ERROR_UNDERRUN       = -7,                   /**< \brief A value was too small                                           	*/
	RC_ERROR_BUFFER_FULL    = -8,                   /**< \brief A buffer was full.                                              	*/
	RC_ERROR_BUFFER_EMTPY   = -9,                   /**< \brief A buffer was empty.                                             	*/
	RC_ERROR_BUSY           = -10,                  /**< \brief A resource was busy.                                            	*/
	RC_ERROR_TIME_OUT       = -11,                  /**< \brief Something take too much time.                                   	*/
	RC_ERROR_OPEN           = -12,                  /**< \brief A device can't be opened.                                       	*/
	RC_ERROR_CHECKSUM       = -13,                  /**< \brief Wrong checksum.                                                 	*/
	RC_ERROR_READ_ONLY      = -14,                  /**< \brief Object is read only.                                            	*/
	RC_ERROR_WRITE_ONLY     = -15,                  /**< \brief Object is write only.                                           	*/
	RC_ERROR_INVALID        = -16,                  /**< \brief Object is invalid.                                              	*/
	RC_ERROR_READ_FAILS     = -17,                  /**< \brief Could not read from object.                                     	*/
	RC_ERROR_WRITE_FAILS    = -18,                  /**< \brief Could not write to the object.                                  	*/
	RC_ERROR_NOT_MATCH      = -19,                  /**< \brief Could not write to the object.								 	    */
	RC_ERROR_BAD_PARAM      = -20,                  /**< \brief Parameters passed to the function are invalid or illegal.           */
	RC_ERROR_BAD_DATA       = -21,                  /**< \brief Invalid global data which is required by the function.              */
	RC_ERROR_CANCELED       = -22,                  /**< \brief operation was canceled.                                             */
	RC_ERROR_INVALID_STATE  = -23,                  /**< \brief The operation can not be performed because of a FSM-state           */
	RC_ERROR_CLOSE  	= -24,                  /**< \brief The device could not be closed           */
    RC_ERROR_UNKNOWN        = -25,                  /**< \brief Unknown error.                                                      */
};
typedef enum eRC RC_t;


/** \brief CPU Core number */
typedef enum{
	cpu0=0,	                                        /**< \brief Core/CPU 0*/
	cpu1=1,	                                        /**< \brief Core/CPU 1*/
	cpu2=2	                                        /**< \brief Core/CPU 2*/
}CpuId_t;

/** \brief Transmission states for communication services */
typedef enum{
    transmission_UNDEFINED,                         /**< \brief Undefined state.*/
    transmission_PENDING,                           /**< \brief Request is pending.*/
    transmission_ACTIVE,                            /**< \brief Request is currently processed.*/
    transmission_COMPLETED,                         /**< \brief Request is completed.*/
    transmission_ERROR,                             /**< \brief Request failed due to communication error */
    transmission_ABORTED                            /**< \brief Request was aborted.*/
}transmission_state_t;

/** \brief Services states */
typedef enum{
    service_UNDEFINED,                              /**< \brief Undefined state.*/
    service_READY,                                  /**< \brief Service is ready.*/
    service_BUSY,                                   /**< \brief Service is busy.*/
    service_UNAVAILABLE,                            /**< \brief Service is unavailable.*/
}service_state_t;
};

/* Platforms */
#define NVIDIA			1
#define RSP			2
#define PC 			3

#endif /* GLOBAL_H_ */
