#ifndef _VIDEO_H_
#define _VIDEO_H_

#include "i8042.h"

#define VBE_FUNCTION 		0x4F

#define RETURN_VBE_MODE_INFORMATION	0x01

#define RETURN_VBE_CONTROLLER_INFORMATION	0x00

#define SET_VBE_MODE		0x02

#define BIOS_VIDEO_SERVICE	0x10

#define VIDEO_MODE 			0x105


#define END_VBE_MODES_CODE  0xFFFF

#endif
