#include "CCom.h"

/***************************************************************************
*============= Copyright by Darmstadt University of Applied Sciences =======
****************************************************************************
* Filename        : CCOM.CPP
* Author          :
* Description     :
*
*
****************************************************************************/


//System Include Files
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//Own Include Files

//Method Implementations

int CCom::configure()
{
	return -1;
}

CCom::CCom() : CResource("/dev/tty0", O_RDWR | O_NOCTTY | O_SYNC, S_IRWXU) 
{

}

CCom::~CCom()
{
}
