/*
GPL. blah, blah, blah.
*/

#ifndef __VIDEODUMP_H__
#define __VIDEODUMP_H__


#include "AL/alc.h"
#include "AL/al.h"
//#include "AL/alext.h"


void videoDoDumpToDiskIfRequired(void);
void videoDumpToDisk(const char* path);


#endif // __VIDEODUMP_H__
