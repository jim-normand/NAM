
#ifndef HEADER_GUARD_NAMFFMPEG_HEADERS_H
#define HEADER_GUARD_NAMFFMPEG_HEADERS_H

#define USE_SWSCALE 1
#define FF_API_OLD_SAMPLE_FMT 0

extern "C"
{
#define __STDC_CONSTANT_MACROS
#include <errno.h>    // for error codes defined in avformat.h
#include <stdint.h>
#include <avcodec.h>
#include <avformat.h>
#include <avdevice.h>

#ifdef USE_SWSCALE    
    #include <swscale.h>
#endif

}



#endif // HEADER_GUARD_NAMFFMPEG_HEADERS_H
