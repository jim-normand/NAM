
#ifndef HEADER_GUARD_NAMFFMPEG_FFMPEG_IMAGE_STREAM_H
#define HEADER_GUARD_NAMFFMPEG_FFMPEG_IMAGE_STREAM_H

#include "../FFmpeg/FFmpegImageStream.hpp"
#include "../FFmpeg/FFmpegDecoder.hpp"

#include "../FFmpeg/MessageQueue.hpp"

#include <osg/ImageStream>

#include <OpenThreads/Condition>
#include <OpenThreads/Thread>

// OpenCV
#include <opencv/cv.h>



using namespace osgFFmpeg;

template <class T>
class MessageQueue;

class NAMFFmpegImageStream : public osgFFmpeg::FFmpegImageStream
{
public:
   
   NAMFFmpegImageStream();
   ~NAMFFmpegImageStream();
   
   bool open(const std::string & filename);
   
   //virtual void play();
   
   void updateImage();
   
private:
   
   //virtual void run();
   //virtual void applyLoopingMode();
   
   static void publishNewFrame(const osgFFmpeg::FFmpegDecoderVideo &, void * user_data);
   
   //OpenCV image
   IplImage       *_opencvImage;
   unsigned int   _nbChannels;
   
   // Skeletonization
   osg::Image     *_movieImage;
   
   void createIplImageFromGLimage(unsigned char* glImage);
   void OpenCV_to_OSG(IplImage* cvImg, osg::Image *videoTex);

};


#endif // HEADER_GUARD_NAMFFMPEG_FFMPEG_IMAGE_STREAM_H
