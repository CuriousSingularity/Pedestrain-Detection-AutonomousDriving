/*
 * Copyright (c) [15-Apr-2019] 

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 *      Authors:	Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 *					Nicolas Leon Ojeda (nicolas.leon@stud.h-da.de)
 *
 * Modifying a section of the code does not make you the author of the code. The 
 * credit must always go to the original authors who did an intensive research to
 * realise the solution. 
 *
 */

// Header Files
#include <iostream>
#include <sys/types.h>
#include <unistd.h>

// User-defined Header Files
#include "./global.h"
#include "./App/inc/CApplication.h"

#define VERSION		("1.0")

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

	CApplication	pedestrian;

	pedestrian.run();

	while(1);

	return 0;
}
