/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CComRxService.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Serial Data Processing thread - packet reception and processing
 * 			it with Service-ID, Local-ID; predefined protocol.
 *
 ****************************************************************************/


#ifndef CCOMRXSERVICE_H
#define CCOMRXSERVICE_H

// System Include Files

// Own Include Files
#include "./App/inc/CSerialProtocol.h"
#include "./OS/inc/CSemaphore.h"
#include "./OS/inc/CThread.h"

class CComRxService : public CThread {
  private:
    /**
     * @brief : Protocol parser object
     */
    CSerialProtocol m_Protocol;

    /**
     * @brief : Main routine for the thread
     *
     * @return - to join the thread
     */
    void run();

  public:
    /**
     * @brief : Constructor
     *
     * @param threadIndex 	: Thread Index
     * @param entry		: Entry function for the thread
     * @param arg		: Arguments to the thread
     */
    CComRxService(int threadIndex);

    /**
     * @brief : Destructor
     */
    ~CComRxService();
};
/********************
 **  CLASS END
 *********************/
#endif /* CCOMRXSERVICE_H */
