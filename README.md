# Pedestrain Detection
## Autonomous Driving

### Demo:
https://www.youtube.com/watch?v=yvu5OiZNAi8&list=PLLnkq_yigp1uETF3DD3hRX9vUfgOSlGjH&index=12&t=2s
https://www.youtube.com/watch?v=dXqr_r2ndgk

### High Level Design

#### Class Diagram
---
**CApplication is the main class of the Pedestrian Detection Application.**

**There is a single process - 5 threads in the Nvidia nano which are implemented as classes:**

    CDetection      :   Used to detect the pedestrian using the detection algorithms, filters false positives, 
                        extract the information such as Angle and Delta Angle of the subject and sends the 
                        information for transmission.
    CCameraService  :   Used to capture the image frame, using gstreamer, and store it into a ring buffer for further 
                        processing.
    CComRxService   :   Used to receive information from the external world. A single thread for the reception.
    CComTxService   :   Used to Transmit the information via different peripherals such as UART, SPI, Ethernet, Wifi. 
                        A single thread for the transmission.
    Background      :   Background thread. 

    All the threads inherit the base class CThread.

*Other important classes*

    CThread         :   Thread creation class which employes pthread library.
    CMailbox        :   Inter-thread communication - Mailbox for each thread to transmit and receive information from 
                        other threads.
    CCamera         :   Initialises and configures the Camera module for video capturing - hardware specific.
    CUart           :   Initialises the uart port (used in tty) for the Transmission and Reception at Baudrate 115200.
    CResource       :   The class which interacts with the hardware resources for reading and writing from the 
                        hardware. 
                        Eg. UART, Camera, etc,.
    CMutex          :   The class to protect the resource from concurrent access by multiple threads. 

---
**TODO insert the class diagram here**

The class design was based on the principle of **Services**. Following this project wide philosophy, the idea behind this 
principle is to design in such a way that the components (for example the camera) could be used by several modules so it 
should not be tightened to any module but instead provide an independent interface which allows extendibility aiming to 
add more modules using the same component.   


The first part of the system comprises the system related components, this is, all the hardware and OS components that 
provide the backbone of the application.

Considering the API provided by Linux to access such components, a standard class `CResource` was created to deal with this 
“file like” access to drivers as well as to the OS artefacts. This base class provides the opening, closing and I/O 
functionality of the components with the critical section’s protection implemented with the use of mutexes. The writing 
and reading capabilities were separated (implying 2 file descriptors and 2 mutexes) to support a wider variety or components, 
for instance the `CMailbox`, which was implemented using Pipes, requires 2 streams in order to enable the simultaneous operations.

In a similar way, the `CUART` and `CCamera` components (handling the hardware drivers) are also system resources and thus inherit 
from the standard `CResource` which already provides the basic functionality and just a specific configuration is to be done 
for each individual case. To handle the specific configuration, a pure virtual method is declared in `CResource` to make sure 
the derived classes implement their own configuration.

On top of the hardware and OS components, several services are defined: `CComTxService`, `CComRxService` and `CCameraService`.
These 3 services are intended to offer the hardware components as software for the application to consume. Services imply they 
are not synchronous with the application but instead run separately and provide information whenever required.

The Com Service relies on the class CSerialProtocol which defines the structure of the communications (the protocol).

The class CDetection is the actual application specific class implementing the pedestrian detection algorithm (HOG). Associated to this class, the CRingBuffer class provides a ring buffer to store frames in a sequential way and retrieves the most recent frame on every read operation.

All those classes that are intended to operate as threads, either because they provide a service or any other reason, extend the CThread class which contains the basic functionality of a Linux thread. Similar to what was done with CResource, CThread is intended to handle all the thread’s common details. As explained in the Tasks architecture, the classes that run as threads are CDetection, CCameraService, CComTxService, CComRxService and CApplicaiton.

For inter-thread communication, the class CMailbox was developed. This class provides a Mailbox based service in which a thread that needs to receive messages from other thread, instantiates a mailbox and receives messages on it.

Finally CApplication is the background thread and basic class which spawns all the application’s threads and then keeps on waiting to maintain the process active.


As explained in the class diagram section, the services and the application itself are to run in parallel, for this reason 3 threads were defined, plus the background thread.

The services offered by the application, the ones encapsulating hardware drivers and OS artefacts are running as threads as they do not depend on any other component but are always either active performing a task or awaiting the request from other thread. On the other hand, the application thread searching for detected pedestrians is to run continuously and as fast as possible to achieve the highest frame rate possible.

The design is as follows:

**TODO Task diagram here**

 The Vision algorithms (pedestrian detection in this case) are running on the Detection thread (T2). This thread is always running searching for results (detections) as fast as possible.

At the same time, the Camera Service is running on thread T3 acquiring frames from the camera hardware at the configured frame rate. Each time a new frame is available it is added to the Frame Buffer so that T2 can retrieve it.

Finally, on T1, the Com Service is running. This thread, on the contrary to the camera service, is not cyclically running or continuously running as T2 but is sleeping. The associated mailbox is pending and as soon as a message is received, it is executed and a protocol built and sent through the UART.

Thread description:

Now, after giving the big picture, a deeper description will be provided:

**TODO sequence diagram here**

 The sequence diagram shows, in a simplified way, the actions and interactions that each thread has as a function of time.

T4 (Application): This thread is the background thread which keeps the process alive and, at an initial point, spawns all the other threads.

T3 (Camera Service): The camera service is running cyclically at the same frame rate the hardware is working. Every time a new frame is available (given by the sleep timer), the thread is awakened, and a frame is read from the Camera driver (gstreamer) and stored in the global Ring Buffer that holds the most recent frames. This buffer holds several frames but it always retrieves the most recent frame as it is the only important frame. Once the buffer is full, the oldest frame is overwritten by the newest.

T2 (Detection): The main application thread which runs the pedestrian detection algorithm. This thread runs continuously (non-stop) trying to achieve the highest frame rate possible. Since the detection algorithm is long running software, this thread needs to be run all the time. The thread first retrieves the most recent frame from the global buffer, then runs the Pedestrian Detection algorithm (based on HOG) and after having the detected elements, sends a message to the ComTxService?’s mailbox.

T1 (Com Tx service): This thread is normally blocked waiting for a message to arrive at its mailbox. Using the receive method of the Mailbox, a blocking function is called, which is released when a message is available at the mailbox. When a message arrives and the thread is executed, the detected pedestrians’ information is encapsulated according to the Serial Protocol and then, after it is parsed to a raw array, sent over the UART port.

#### Angle of Detected Pedestrian
For determining the angle and the range spanned by the detected pedestrian, the 62° angle of the camera is to be considered in the following calculations:

**TODO angle pic here**

Using the previous diagram, the angle of a given point inside the frame can be calculated as:

**TODO formula pic**


## Detection Algorithm 
OpenCV - HOG

### Filtering
Finally, after getting all the possible detections from the NMS, a filtering is performed to further decide if a pedestrian is present or not.

**TODO filter pic here**

 Due to the fact that HOG gets easily tricked by straight lines, behavior observed during testing, a Straight line filter was added to discard all those areas with a certain number (6) of straight lines present. To achieve this, another image processing algorithm, available in OpenCV, was used, the Hough Lines algorithm. This method, identifies straight lines in the given image. Since it operates on gray images, a conversion is performed previously and also the Canny algorithm is applied in order to leave only the borders so that the line detection is more efficient.

Those areas with a positive detection, even after filtering out the areas containing many straight lines, an area filter is applied to rule out those detections of very small size which very likely correspond to false positives. In case of a pedestrian getting discarded by the area filtering, since it is very small area, it would be a pedestrian standing far away of the car.

Following the 2 filters, the final decision is made on the remaining hypothesis by taking the biggest area, which would be the pedestrian standing closer to the car. Having the final decision, the angle calculation (as explained before) is done. After this process, the result consists of an angle (θ) together with the spanning angle of the detection (Δθ). 

#### Protocol
Protocol information is depicted below: 
**TODO insert the protocol pic here**

The byte information is as follows:

    SOP         - 1 byte - 0xFA - Start of Protocol
    DLC         - 1 byte - 0x04 - Data length code
    Theta       - 1 byte - 0xXX - Angle at which subject is present
    Delta Theta - 1 byte - 0xXX - delta angle of the subject position
    Reserved    - 2 byte - 0xXXXX - Reserved bytes
    EOP         - 1 byte - 0xFD - End of Protocol 

### Packages

    OpenCV - 3.4
    cmake
    pthread
    man pages

&copy; All Rights Reserved for Authors

**TODO Reference : https://agea.github.io/tutorial.md/**
