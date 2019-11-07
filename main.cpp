/*
 * main.cpp
 *
 *  Created on: 15-Apr-2019
 *      Author: Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 */

// Header Files
#include <iostream>
#include <sys/types.h>
#include <unistd.h>

// User-defined Header Files
#include "CPedestrianDetection.h"
#include "./global.h"

#define VERSION		("0.1")

using namespace std;

int main(int argc, char **argv)
{

#if (TARGET_PLATFORM == NVIDIA)
	cout << "NVidia Platform" << endl;
#elif (TARGET_PLATFORM == RSP)
	cout << "Raspberry Pi Platform" << endl;
#elif (TARGET_PLATFORM == PC)
	cout << "PC platform" << endl;
#else
	cout << "Invalid Platform" << endl;
	while(1);
#endif
	cout << VERSION << "v : Program Started with PPID : " << getppid() << "; PID : " << getpid() << endl;

	CPedestrianDetection	pedestrianDetection;

	pedestrianDetection.run();

	while(1);

	return 0;
}
