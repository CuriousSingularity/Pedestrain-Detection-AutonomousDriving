/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CCamera.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Camera class to capture frames and stream video from hardware devices
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

#ifndef CCAMERA_H
#define CCAMERA_H

// System Include Files
#include <opencv2/opencv.hpp>

// Own Include Files
#include "./OS/inc/CResource.h"

#define RESOLUTION_CAPTURED_WIDTH (1280)
#define RESOLUTION_CAPTURED_HEIGHT (720)
#define RESOLUTION_RESIZED_WIDTH (960)
#define RESOLUTION_RESIZED_HEIGHT (540)
#define FRAMERATE (30)

#define RSP_CAMERA_V2

#ifdef RSP_CAMERA_V2
#define HORIZONTAL_FOV (62.2)
#define VERTICAL_FOV (48.8)
#define ANGLE_RESOLUTION ((float)HORIZONTAL_FOV / RESOLUTION_RESIZED_WIDTH)
#define ZERO_PIXEL_ANGLE ((float)-HORIZONTAL_FOV / 2)
#define ANGLE_PRECISION_FACTOR (1)  // Scaling factor: change to 100 or 1000 for decimal precision
#endif


class CCamera : public CResource {
  private:
    cv::VideoCapture m_cameraStream;

    /**
     * @brief : Configure the Camera interface
     * 		- Framerate
     * 		- Resolution
     * 		- Orientation
     * 		- ISO
     * 		- Brightness
     * 		- Contrast
     * 		- Sharpness
     * 		- Satureation
     *
     * @return RC_t : Status of the camera
     */
    global::RC_t configure();

    /**
     * @brief : Captures a frame
     *
     * @param image : image / frame from the camera
     *
     * @return - status of capture
     */
    global::RC_t getCapture(cv::Mat* const image);

  public:
    /**
     * @brief Camera orientation enumeration
     * Defines rotation angles for camera mounting orientation
     */
    typedef enum {
        // do not change the order
        ORIENT_DEG_0 = 0,
        ORIENT_DEG_90 = 1,
        ORIENT_DEG_180 = 2,
        ORIENT_DEG_270 = 3,
    } orientation_e;

    /**
     * @brief Camera configuration parameters
     * Structure containing all camera setup parameters including resolution,
     * framerate, orientation, and color format
     */
    typedef struct {
        unsigned int capture_width;
        unsigned int capture_height;
        unsigned int resized_width;
        unsigned int resized_height;
        unsigned int framerate;
        orientation_e orientation;
        std::string colour_format;
    } configuration_t;

    /**
     * @brief Constructor
     * @param devPath Device path for camera (e.g., /dev/video0)
     * @param flag File access flags (O_RDWR, O_NOCTTY, etc.)
     * @param mode File permissions mode
     */
    CCamera(std::string devPath, int flag, mode_t mode);

    /**
     * @brief Destructor
     * Releases camera resources and cleans up video capture
     */
    virtual ~CCamera();

    /**
     * @brief Read a frame from the camera
     * Captures a single frame from the camera and stores it in the provided buffer
     * 
     * @param buffer Pointer to cv::Mat buffer for storing the captured frame
     * @param nByte Number of bytes to read (ignored for camera interface)
     * @param wByte Reference to store actual bytes read/processed
     * @return RC_t Status code indicating success or failure
     */
    global::RC_t read(const void* buffer, const size_t nByte, ssize_t& wByte);

    /**
     * @brief Write operation not supported for camera
     * Camera devices are read-only, this function always returns read-only error
     * 
     * @param buffer Ignored parameter
     * @param nByte Ignored parameter  
     * @param wByte Ignored parameter
     * @return RC_t Always returns RC_ERROR_READ_ONLY
     */
    global::RC_t write(const void* buffer, const size_t nByte, ssize_t& wByte);
};
/********************
 **  CLASS END
 *********************/
#endif /* CCAMERA_H */
