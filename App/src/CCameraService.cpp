/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CCameraService.cpp
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Camera Service which fills up the request with an image
 *
 ****************************************************************************/


//System Include Files
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

//Own Include Files
#include "./Lib/inc/CRingBuffer.h"
#include "./App/inc/CCameraService.h"

//Namespace
using namespace cv;
using namespace std;
using namespace global;

//Macros
//Global variables
CRingBuffer<cv::Mat, FRAMERATE> g_framesBuffer;

static void cloneMat(cv::Mat &lhs, const cv::Mat &rhs)
{
	lhs = rhs.clone();
}


//Method Implementations
/**
 * @brief : Constructor
 *
 * @param threadIndex 	: Thread Index
 * @param sysResource	: Global resource pointer
 * @param entry		: Entry function for the thread
 * @param arg		: Arguments to the thread
 */
CCameraService::CCameraService(int threadIndex, CThread::start_routine_t entry, void *arg) : 
		CThread(threadIndex, entry, arg),
		m_camera_0("/dev/video0"	, O_RDWR | O_NOCTTY | O_SYNC, S_IRWXU)
{
	// nothing
}


/**
 * @brief : Destructor
 */
CCameraService::~CCameraService()
{
	// nothing
}

int CCameraService::signal_type = 0;

void CCameraService::__camera_cyclic__signal_handler(int sig)
{
	CCameraService::signal_type = sig;
}

void CCameraService::wait_for_newFrame()
{
	struct timespec delta = 
	{
		.tv_sec 	= 0,// seconds
		.tv_nsec	= (999999999 / FRAMERATE),// nano seconds
	};

	while (nanosleep(&delta, &delta));
}
/**
 * @brief : Main routine for the thread
 *
 * @return - to join the thread
 */
void CCameraService::run()
{
	// The Threads runs here
	cout << "INFO\t: Camera Thread " << this->getThreadIndex() << " started with ID : " << pthread_self() << endl;

	cv::Mat image;
	ssize_t wBytes;

	while (1)
	{
		// background thread
		cout << "INFO\t: Running Camera Service " << this->getThreadIndex() " : " << pthread_self() << endl;

		this->wait_for_newFrame();

		// read new image to ring buffer
		if (this->m_camera_0.read(&image, 0, wBytes) != RC_SUCCESS)
			continue;

		// store the frame to ringbuffer for consumers
		g_framesBuffer.writeData(image, cloneMat);
	}
}



/**
 * @brief : Friend function used to create the thread 
 *
 * @param arg : arguments to the thread
 *
 * @return 
 */
void *friend_camera_service(void *arg)
{
	cout << "INFO\t: Thread " << __func__ << endl;

	CCameraService *ptr = static_cast<CCameraService *>(arg);

	if (ptr)
	{
		ptr->run();
	}

	return NULL;
}
