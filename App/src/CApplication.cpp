/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CApplication.cpp
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Pedestrian Detection Application
 *
 ****************************************************************************/


// System Include Files
#include <iostream>
#include <thread>

// Own Include Files
#include "./App/inc/CApplication.h"
#include "./Common/inc/Logger.h"
#include "./OS/inc/CMailBox.h"

// Namespace
using namespace std;
using namespace global;


// global Variable
// Mailboxes for all the threads
CMailBox g__Mailboxes[THREAD_TOTAL_COUNT] = {
    {THREAD_BACKGROUND},     {THREAD_COM_TX_SERVICE},    {THREAD_COM_RX_SERVICE},
    {THREAD_CAMERA_SERVICE}, {THREAD_DETECTION_SERVICE},
};

// Method Implementations
/**
 * @brief : Constructor
 */
CApplication::CApplication()
    : m_thread_com_tx_service(THREAD_COM_TX_SERVICE),
      m_thread_com_rx_service(THREAD_COM_RX_SERVICE),
      m_thread_camera_service(THREAD_CAMERA_SERVICE), m_thread_detection(THREAD_DETECTION_SERVICE) {
    // nothing
}


/**
 * @brief : Destructor
 */
CApplication::~CApplication() {
    // nothing
}


/**
 * @brief : Run function for the Pedestrain Detection Aapplication
 */
void CApplication::run() {
    // Create the Threads
    if (this->m_thread_com_tx_service.create() != RC_SUCCESS) {
        LOG_ERROR("CApplication", "Failed to set up the Communication Tx Service Thread " + std::to_string(this->m_thread_com_tx_service.getThreadIndex()));
    }

    if (this->m_thread_com_rx_service.create() != RC_SUCCESS) {
        LOG_ERROR("CApplication", "Failed to set up the Communication Rx Service Thread " + std::to_string(this->m_thread_com_rx_service.getThreadIndex()));
    }

    if (this->m_thread_camera_service.create() != RC_SUCCESS) {
        LOG_ERROR("CApplication", "Failed to set up the Camera Service Thread " + std::to_string(this->m_thread_camera_service.getThreadIndex()));
    }

    if (this->m_thread_detection.create() != RC_SUCCESS) {
        LOG_ERROR("CApplication", "Failed to set up the Detection Thread " + std::to_string(this->m_thread_detection.getThreadIndex()));
    }

    LOG_INFO("CApplication", "Running Background Thread 0 started with ID : " + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())));
    while (1) {
        // background thread
        sleep(1);
    }
}
