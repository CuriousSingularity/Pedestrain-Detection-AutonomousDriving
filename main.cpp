/*
 * main.cpp
 *
 *  Created on: 15-Apr-2019
 *      Author: Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 */

// Header Files
#include <iostream>
#include <unistd.h>

// User-defined Header Files
#include "CPedestrianDetection.h"

#define NVIDIA		1
#define RSP		2

using namespace std;

int main(int argc, char **argv)
{
#if (TARGET_PLATFORM == NVIDIA)
	cout << "NVidia Platform" << endl;
#elif (TARGET_PLATFORM == RSP)
	cout << "Raspberry Pi Platform" << endl;
#else
	cout << "Invalid Platform" << endl;
	while(1);
#endif
	cout << "Program Started..." << endl;

	CPedestrianDetection	pedestrianDetection;

	pedestrianDetection.run();

	while(1);

	return 0;
}
