all:exe

# Paths to sources
PATH_PROJECT=.

PATH_APP_SOURCE=$(PATH_PROJECT)/App/src
PATH_APP_INCLUDE=$(PATH_PROJECT)/App/inc

PATH_OS_SOURCE=$(PATH_PROJECT)/OS/src
PATH_OS_INCLUDE=$(PATH_PROJECT)/OS/inc

PATH_HAL_SOURCE=$(PATH_PROJECT)/HAL/src
PATH_HAL_INCLUDE=$(PATH_PROJECT)/HAL/inc

# New modular components
PATH_COMMON_SOURCE=$(PATH_PROJECT)/Common/src
PATH_COMMON_INCLUDE=$(PATH_PROJECT)/Common/inc

PATH_DETECTION_SOURCE=$(PATH_PROJECT)/Detection/src
PATH_DETECTION_INCLUDE=$(PATH_PROJECT)/Detection/inc

PATH_COMMUNICATION_SOURCE=$(PATH_PROJECT)/Communication/src
PATH_COMMUNICATION_INCLUDE=$(PATH_PROJECT)/Communication/inc

PATH_BUILD_DIR=$(PATH_PROJECT)/build

GEN_DESIGN_SCRIPT=generate_design.sh

# Cross-Compiler specific flags
# Use CXX environment variable if set, otherwise default to g++
CXX ?= g++
CC = $(CXX)

# Check GCC version and set appropriate C++ standard
GCC_VERSION := $(shell $(CXX) -dumpversion)
GCC_VERSION_MAJOR := $(shell echo $(GCC_VERSION) | cut -d. -f1)

# Use C++20 if GCC >= 11, otherwise fall back to C++17
ifeq ($(shell expr $(GCC_VERSION_MAJOR) \>= 11), 1)
    STD_FLAG = -std=c++20
    COROUTINE_FLAG = -fcoroutines
    MODULE_FLAG = 
else ifeq ($(shell expr $(GCC_VERSION_MAJOR) \>= 8), 1)
    STD_FLAG = -std=c++17
    COROUTINE_FLAG = 
    MODULE_FLAG = 
else
    STD_FLAG = -std=c++14
    COROUTINE_FLAG = 
    MODULE_FLAG = 
endif

CFLAGS=-c -Wall -Wextra $(STD_FLAG) $(COROUTINE_FLAG) $(MODULE_FLAG) -Wno-deprecated-enum-enum-conversion
LFLAGS=-pthread -lstdc++ -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui -lopencv_calib3d -lopencv_objdetect -lopencv_flann -lopencv_videoio -lopencv_dnn
EFLAGS=-o
DFLAGS=-D
IFLAGS=-I

PATH_OPENCV_INCLUDE=$(IFLAGS)/usr/include/opencv2/ $(IFLAGS)/usr/include/opencv/
PATH_ALL_INCLUDE=$(IFLAGS)$(PATH_APP_INCLUDE) $(IFLAGS)$(PATH_OS_INCLUDE) $(IFLAGS)$(PATH_HAL_INCLUDE) $(IFLAGS)$(PATH_COMMON_INCLUDE) $(IFLAGS)$(PATH_DETECTION_INCLUDE) $(IFLAGS)$(PATH_COMMUNICATION_INCLUDE) $(IFLAGS)$(PATH_PROJECT) $(PATH_OPENCV_INCLUDE) $(EXTRA_CFLAGS)

exe:obj
	$(CC) -L/usr/lib/ $(PATH_BUILD_DIR)/*.o $(EFLAGS) exe_$(PLATFORM) $(LFLAGS) $(EXTRA_LIBS) && ./$(GEN_DESIGN_SCRIPT)

obj:*.cpp
	# create directories
	mkdir -p $(PATH_BUILD_DIR)
	# Application source
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/main.o 			$(PATH_PROJECT)/main.cpp
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/CDetection.o 		$(PATH_APP_SOURCE)/CDetection.cpp
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/CApplication.o 		$(PATH_APP_SOURCE)/CApplication.cpp
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/CComTxService.o 		$(PATH_APP_SOURCE)/CComTxService.cpp
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/CComRxService.o 		$(PATH_APP_SOURCE)/CComRxService.cpp
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/CCameraService.o 		$(PATH_APP_SOURCE)/CCameraService.cpp
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/CSerialProtocol.o 	$(PATH_APP_SOURCE)/CSerialProtocol.cpp
	# OS source
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/CMutex.o 			$(PATH_OS_SOURCE)/CMutex.cpp
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/CResource.o 		$(PATH_OS_SOURCE)/CResource.cpp
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/CSemaphore.o 		$(PATH_OS_SOURCE)/CSemaphore.cpp
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/CThread.o 		$(PATH_OS_SOURCE)/CThread.cpp
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/CMailBox.o 		$(PATH_OS_SOURCE)/CMailBox.cpp
	# HAL source
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/CCamera.o 		$(PATH_HAL_SOURCE)/CCamera.cpp
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/CUart.o 			$(PATH_HAL_SOURCE)/CUart.cpp
	# Common framework source
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/ConfigurationManager.o 	$(PATH_COMMON_SOURCE)/ConfigurationManager.cpp
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/ObservableSubject.o 	$(PATH_COMMON_SOURCE)/ObservableSubject.cpp
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/ServiceFactoryManager.o 	$(PATH_COMMON_SOURCE)/ServiceFactoryManager.cpp
	# Detection algorithms - temporarily commented out for debugging
	# Always compile legacy algorithm for compatibility
	#$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/LegacyHOGAlgorithm.o 	$(PATH_DETECTION_SOURCE)/LegacyHOGAlgorithm.cpp
	# Compile modern algorithm only if C++20 is available
#ifeq ($(shell expr $(GCC_VERSION_MAJOR) \>= 11), 1)
	#$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/HOGDetectionAlgorithm.o 	$(PATH_DETECTION_SOURCE)/HOGDetectionAlgorithm.cpp
#endif

clean:
	rm $(PATH_BUILD_DIR)/*.o -r -v
	rm exe*
