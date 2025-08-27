/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CCamera.cpp
 * Author          : 	Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * 			Nicolas Ojeda Leon (stniojed@stud.h-da.de)
 * Description     : Camera class implementation for capturing frames and streaming video
 *
 * ----- Changelog -----
 *
 * Start of user code changelog
 *
 *  - 1.0 ; 22-May-2019; Leon
 *            -  Implementation of the class' functionality.
 *            		Added specific code for camera frame acquisition
 *
 * End of user code
 ****************************************************************************/

// System Include Files
#include <iostream>

#include <unistd.h>

// Own Include Files
#include "./Common/inc/Logger.h"
#include "./HAL/inc/CCamera.h"

// Namespace
using namespace std;
using namespace global;

// Local variables:
/**
 * @brief Default camera configuration parameters
 * Static configuration used for initializing camera with standard settings
 */
static const CCamera::configuration_t camera_config_param{
    .capture_width = RESOLUTION_CAPTURED_WIDTH,
    .capture_height = RESOLUTION_CAPTURED_HEIGHT,
    .resized_width = RESOLUTION_RESIZED_WIDTH,
    .resized_height = RESOLUTION_RESIZED_HEIGHT,
    .framerate = FRAMERATE,
    .orientation = CCamera::ORIENT_DEG_180,
    .colour_format = "BGR",
};


/**
 * @brief Generate GStreamer pipeline configuration string
 * Creates a GStreamer pipeline string based on camera configuration parameters
 * for use with OpenCV VideoCapture on NVIDIA platforms
 * 
 * @param config Camera configuration parameters
 * @return std::string GStreamer pipeline configuration string
 */
static std::string gstreamer_pipeline(const CCamera::configuration_t& config) {
    return "nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)" +
           std::to_string(config.capture_width) + ", height=(int)" +
           std::to_string(config.capture_height) + ", format=(string)NV12, framerate=(fraction)" +
           std::to_string(config.framerate) +
           "/1 ! nvvidconv flip-method=" + std::to_string(config.orientation) +
           " ! video/x-raw, width=(int)" + std::to_string(config.capture_width) + ", height=(int)" +
           std::to_string(config.capture_height) + ", format=(string)" + config.colour_format +
           "x ! videoconvert ! video/x-raw, format=(string)" + config.colour_format + " ! appsink";
}


// Method Implementations
/**
 * @brief Configure the camera interface
 * Initializes camera with specified parameters including:
 * - Framerate: Target frames per second
 * - Resolution: Capture and output resolution
 * - Orientation: Camera mounting orientation
 * - Color format: Output pixel format
 * 
 * Uses GStreamer pipeline for NVIDIA hardware acceleration
 * 
 * @return RC_t Status code indicating configuration success or failure
 */
global::RC_t CCamera::configure() {
    LOG_INFO("CCamera", "Camera port " + this->getDeviceNode() + " configuration");

    if (this->m_status != service_READY)
        return RC_ERROR_INVALID_STATE;

    // open will first release the resource in case it was previously open.
    if (!this->m_cameraStream.open(gstreamer_pipeline(camera_config_param), cv::CAP_GSTREAMER))
        return RC_ERROR_OPEN;

    if (!this->m_cameraStream.isOpened())
        return RC_ERROR_INVALID_STATE;

    return RC_SUCCESS;
}

/**
 * @brief Constructor
 * Initializes camera device and attempts configuration
 * Sets service status based on configuration success
 * 
 * @param devPath Device path for camera access
 * @param flags File access flags
 * @param mode File permission mode
 */
CCamera::CCamera(string devPath, int flags, mode_t mode) : CResource(devPath, flags, mode) {
    if (this->configure() != RC_SUCCESS) {
        this->m_status = service_UNAVAILABLE;
        LOG_ERROR("CCamera", "Resource configuration failed for Device " + this->getDeviceNode());
    }

    LOG_INFO("CCamera", "Camera port " + this->getDeviceNode() + " constructed");
}


/**
 * @brief Destructor
 * Safely releases camera resources and closes video stream
 */
CCamera::~CCamera() {
    LOG_INFO("CCamera", "Camera port " + this->getDeviceNode() + " destructed");

    if (this->m_status != service_READY)
        return;

    // release the camera resource
    this->m_cameraStream.release();
}


/**
 * @brief Capture a single frame from camera
 * Reads frame from camera stream and resizes to configured output resolution
 * 
 * @param image Pointer to cv::Mat to store captured frame
 * @return RC_t Status code indicating capture success or failure
 */
RC_t CCamera::getCapture(cv::Mat* const image) {
    if (this->m_status != service_READY) {
        LOG_ERROR("CCamera", "Camera hardware not ready " + this->getDeviceNode());
        return RC_ERROR_INVALID_STATE;
    }

    cv::Mat capturedImage;

    if (!this->m_cameraStream.read(capturedImage)) {
        LOG_ERROR("CCamera", "Camera failed to read " + this->getDeviceNode());
        return RC_ERROR_READ_FAILS;
    }

    cv::resize(capturedImage, *image,
               cv::Size(camera_config_param.resized_width, camera_config_param.resized_height));

    return RC_SUCCESS;
}


/**
 * @brief Read interface implementation for camera
 * Implements the CResource read interface for camera frame capture
 * 
 * @param buffer Pointer to cv::Mat buffer for frame storage
 * @param nByte Number of bytes (ignored for camera interface)
 * @param wByte Reference to store element size of captured frame
 * @return RC_t Status code indicating read operation result
 */
RC_t CCamera::read(const void* buffer, const size_t nByte, ssize_t& wByte) {
    RC_t ret = RC_ERROR_READ_FAILS;

    if (!buffer)
        return RC_ERROR_NULL;

    cv::Mat* image = (cv::Mat*)buffer;

    if ((ret = this->getCapture(image)) == RC_SUCCESS)
        wByte = image->elemSize();

    return ret;
}

/**
 * @brief Write operation not supported
 * Camera devices are read-only, write operations are not applicable
 * 
 * @param buffer Ignored parameter
 * @param nByte Ignored parameter
 * @param wByte Ignored parameter
 * @return RC_t Always returns RC_ERROR_READ_ONLY
 */

RC_t CCamera::write(const void* buffer, const size_t nByte, ssize_t& wByte) {
    return RC_ERROR_READ_ONLY;
}
