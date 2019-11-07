#include "CResource.h"

/***************************************************************************
*============= Copyright by Darmstadt University of Applied Sciences =======
****************************************************************************
* Filename        : CRESOURCE.CPP
* Author          :
* Description     :
*
*
****************************************************************************/


//System Include Files


//Own Include Files

//Method Implementations
using namespace std;

int CResource::open(){}
int CResource::close(){}
ssize_t CResource::read(void * buffer, size_t nbyte){}
ssize_t CResource::write(void * buffer, size_t nbyte){}
CResource::CResource(string devPath, int oflag, int mode){}
CResource::~CResource(){}

