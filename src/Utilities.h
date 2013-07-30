/**
 *  \file Utilities.h
 *  \brief Various utilities functions
 *  EMNA3D Project
 *
 *  Created by Nicolas Sicard on 29/04/09.
 *  Copyright 2009 EFREI. All rights reserved.
 *
 */

#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#ifdef WIN32
    #include <windows.h>
#endif

int isBigEndian();
#ifdef WIN32
int gettimeofday(struct timeval* p, void* tz);
#endif

#endif
