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

# 🚀 Configurable Build Flags
# Debug and development flags (can be overridden via command line)
ENABLE_ALGO_TIME_MEASUREMENT ?= 0
ENABLE_DISPLAY_CONNECTED ?= 0
ENABLE_DEBUG_LOGGING ?= 0
ENABLE_PERFORMANCE_PROFILING ?= 0
ENABLE_VERBOSE_OUTPUT ?= 0

# Build type configuration
BUILD_TYPE ?= RELEASE
DEBUG_LEVEL ?= 0

# Feature flags
ENABLE_MODERN_CPP20 ?= 1
ENABLE_COROUTINES ?= 1
ENABLE_CONCEPTS ?= 1

# Enforce C++20 standard - no fallbacks
GCC_VERSION := $(shell $(CXX) -dumpversion)
GCC_VERSION_MAJOR := $(shell echo $(GCC_VERSION) | cut -d. -f1)

# Require GCC 11+ for C++20 support
ifeq ($(shell expr $(GCC_VERSION_MAJOR) \>= 11), 1)
    STD_FLAG = -std=c++20
    COROUTINE_FLAG = -fcoroutines
    MODULE_FLAG = -fmodules-ts
else
    $(error Error: This project requires GCC 11+ for C++20 support. Current version: $(GCC_VERSION))
endif

# 🔧 Build Flag Generation
BUILD_FLAGS = 
ifeq ($(ENABLE_ALGO_TIME_MEASUREMENT), 1)
    BUILD_FLAGS += -DALGO_TIME_MEASUREMENT
endif
ifeq ($(ENABLE_DISPLAY_CONNECTED), 1)
    BUILD_FLAGS += -DDISPLAY_CONNECTED=1
else
    BUILD_FLAGS += -DDISPLAY_CONNECTED=0
endif
ifeq ($(ENABLE_DEBUG_LOGGING), 1)
    BUILD_FLAGS += -DDEBUG_LOGGING -DDEBUG
endif
ifeq ($(ENABLE_PERFORMANCE_PROFILING), 1)
    BUILD_FLAGS += -DPERFORMANCE_PROFILING -pg
endif
ifeq ($(ENABLE_VERBOSE_OUTPUT), 1)
    BUILD_FLAGS += -DVERBOSE_OUTPUT
endif
ifeq ($(BUILD_TYPE), DEBUG)
    BUILD_FLAGS += -g -O0 -DDEBUG
else ifeq ($(BUILD_TYPE), RELEASE)
    BUILD_FLAGS += -O3 -DNDEBUG
endif

CFLAGS=-c -Wall -Wextra $(STD_FLAG) $(COROUTINE_FLAG) $(MODULE_FLAG) $(BUILD_FLAGS) -Wno-deprecated-enum-enum-conversion
LFLAGS=-pthread -lstdc++ -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui -lopencv_calib3d -lopencv_objdetect -lopencv_flann -lopencv_videoio -lopencv_dnn
EFLAGS=-o
DFLAGS=-D
IFLAGS=-I

PATH_OPENCV_INCLUDE=$(IFLAGS)/usr/include/opencv4/ $(IFLAGS)/usr/include/opencv2/ $(IFLAGS)/usr/include/opencv/
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
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/Logger.o 			$(PATH_COMMON_SOURCE)/Logger.cpp
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/ObservableSubject.o 	$(PATH_COMMON_SOURCE)/ObservableSubject.cpp
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/ServiceFactoryManager.o 	$(PATH_COMMON_SOURCE)/ServiceFactoryManager.cpp
	# Detection algorithms - C++20 modern implementation only
	$(CC) $(CFLAGS) $(PATH_ALL_INCLUDE) $(DFLAGS) TARGET_PLATFORM=$(PLATFORM) $(EFLAGS) $(PATH_BUILD_DIR)/HOGDetectionAlgorithm.o 	$(PATH_DETECTION_SOURCE)/HOGDetectionAlgorithm.cpp

clean:
	rm $(PATH_BUILD_DIR)/*.o -r -v
	rm exe*

# 📋 Build configuration help
help:
	@echo "🚀 Pedestrian Detection Build System"
	@echo "=================================="
	@echo ""
	@echo "📦 Available Platforms:"
	@echo "  PLATFORM=NVIDIA    - NVIDIA Jetson Nano"
	@echo "  PLATFORM=RSP       - Raspberry Pi"
	@echo "  PLATFORM=PC        - PC Platform"
	@echo ""
	@echo "🔧 Configurable Flags:"
	@echo "  ENABLE_ALGO_TIME_MEASUREMENT=1  - Enable algorithm timing"
	@echo "  ENABLE_DISPLAY_CONNECTED=1      - Enable display output"
	@echo "  ENABLE_DEBUG_LOGGING=1          - Enable debug logging"
	@echo "  ENABLE_PERFORMANCE_PROFILING=1  - Enable performance profiling"
	@echo "  ENABLE_VERBOSE_OUTPUT=1         - Enable verbose output"
	@echo ""
	@echo "🏗️ Build Types:"
	@echo "  BUILD_TYPE=DEBUG    - Debug build (-g -O0)"
	@echo "  BUILD_TYPE=RELEASE  - Release build (-O3 -DNDEBUG)"
	@echo ""
	@echo "🎯 Examples:"
	@echo "  make PLATFORM=PC"
	@echo "  make PLATFORM=NVIDIA ENABLE_ALGO_TIME_MEASUREMENT=1"
	@echo "  make PLATFORM=PC BUILD_TYPE=DEBUG ENABLE_DEBUG_LOGGING=1"
	@echo "  make clean"
	@echo ""

# 🎯 Quick build targets
debug:
	$(MAKE) exe BUILD_TYPE=DEBUG ENABLE_DEBUG_LOGGING=1

profile:
	$(MAKE) exe ENABLE_ALGO_TIME_MEASUREMENT=1 ENABLE_PERFORMANCE_PROFILING=1

timing:
	$(MAKE) exe ENABLE_ALGO_TIME_MEASUREMENT=1

display:
	$(MAKE) exe ENABLE_DISPLAY_CONNECTED=1
