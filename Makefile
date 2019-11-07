all:exe

# Paths to sources
PATH_PROJECT=.

PATH_APP_SOURCE=$(PATH_PROJECT)/App/src
PATH_APP_INCLUDE=$(PATH_PROJECT)/App/inc

PATH_OS_SOURCE=$(PATH_PROJECT)/OS/src
PATH_OS_INCLUDE=$(PATH_PROJECT)/OS/inc

PATH_HAL_SOURCE=$(PATH_PROJECT)/HAL/src
PATH_HAL_INCLUDE=$(PATH_PROJECT)/HAL/inc

PATH_BUILD_DIR=$(PATH_PROJECT)/build

GEN_DESIGN_SCRIPT=generate_design.sh

# Cross-Compiler specific flags
CC=g++
CFLAGS=-c -Wall
LFLAGS=-pthread -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui -lopencv_calib3d -lopencv_objdetect -lopencv_flann -lopencv_videoio
EFLAGS=-o
DFLAGS=-D
IFLAGS=-I

PATH_OPENCV_INCLUDE=$(IFLAGS)/usr/include/opencv2/ $(IFLAGS)/usr/include/opencv/
PATH_ALL_INCLUDE=$(IFLAGS)$(PATH_APP_INCLUDE) $(IFLAGS)$(PATH_OS_INCLUDE) $(IFLAGS)$(PATH_HAL_INCLUDE) $(IFLAGS)$(PATH_PROJECT)

exe:obj
	$(CC) -L/usr/lib/ $(PATH_BUILD_DIR)/*.o $(EFLAGS) exe_$(PLATFORM) $(LFLAGS) && ./$(GEN_DESIGN_SCRIPT)

obj:*.cpp
	# create directories
	mkdir -p $(PATH_BUILD_DIR)
	# Application source
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/main.o 			$(PATH_PROJECT)/main.cpp
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/CCameraDataProcessing.o 	$(PATH_APP_SOURCE)/CCameraDataProcessing.cpp
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/CPedestrianDetection.o 	$(PATH_APP_SOURCE)/CPedestrianDetection.cpp
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/CSerialDataProcessing.o 	$(PATH_APP_SOURCE)/CSerialDataProcessing.cpp
	# OS source
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/CMutex.o 			$(PATH_OS_SOURCE)/CMutex.cpp
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/CResource.o 		$(PATH_OS_SOURCE)/CResource.cpp
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/CSemaphore.o 		$(PATH_OS_SOURCE)/CSemaphore.cpp
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/CSystemResource.o 	$(PATH_OS_SOURCE)/CSystemResource.cpp
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/CThread.o 				$(PATH_OS_SOURCE)/CThread.cpp
	# HAL source
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/CCamera.o 		$(PATH_HAL_SOURCE)/CCamera.cpp
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/CCom.o 			$(PATH_HAL_SOURCE)/CCom.cpp

clean:
	rm $(PATH_BUILD_DIR)/*.o -r -v
	rm exe*
