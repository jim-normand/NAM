
#include "NAM-FFmpegImageStream.hpp"
#include "../FFmpeg/FFmpegAudioStream.hpp"

#include <OpenThreads/ScopedLock>
#include <osg/Notify>

#include <memory>
#include <iostream>

// OpenCV test
#include <opencv/highgui.h>
// Skeletonization test
#include "../skeletonization/Skeletonize.h"


///////////////////////////////////////////////////////////////////////////////
//////////////////////  NAM FFmpeg Image Stream   /////////////////////////////
///////////////////////////////////////////////////////////////////////////////


NAMFFmpegImageStream::NAMFFmpegImageStream()
{
   _movieImage = new osg::Image();
}



NAMFFmpegImageStream::~NAMFFmpegImageStream()
{
   OSG_INFO<<"Destructing NAMFFmpegImageStream..."<<std::endl;
   
   cvReleaseImage(&_opencvImage);
   _opencvImage = NULL;
   //cvDestroyWindow("Test");
   OSG_INFO<<"Destructed NAMFFMpegImageStream."<<std::endl;
}



bool NAMFFmpegImageStream::open(const std::string & filename)
{
    setFileName(filename);

    if (! m_decoder->open(filename))
        return false;

    //setImage(
    //    m_decoder->video_decoder().width(), m_decoder->video_decoder().height(), 1, GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE,
    //    const_cast<unsigned char *>(m_decoder->video_decoder().image()), NO_DELETE
    //);

   // OpenCV Image creation
   //cvNamedWindow( "Test", 0 );
   //cvResizeWindow("Test", m_decoder->video_decoder().width(), m_decoder->video_decoder().height());

   // According to previous line the number of channels is 4: RGBA
   _nbChannels = 4;
   _opencvImage = cvCreateImage(cvSize(m_decoder->video_decoder().width(), m_decoder->video_decoder().height()), IPL_DEPTH_8U, _nbChannels);
   
   
   //TEST
   createIplImageFromGLimage(const_cast<unsigned char *>(m_decoder->video_decoder().image()));
   
   process(_opencvImage);
   //OpenCV_to_OSG(_opencvImage,this);
   setImage(
            m_decoder->video_decoder().width(), m_decoder->video_decoder().height(), 1, GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE,
            (unsigned char*)(_opencvImage->imageData), NO_DELETE
            );
   /*setImage(
            m_decoder->video_decoder().width(), m_decoder->video_decoder().height(), 1, GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE,
            //(unsigned char*)(_opencvImage->imageData), NO_DELETE
            _movieImage->data(),NO_DELETE
            );*/
   
   // END TEST
   
   
   
    setPixelAspectRatio(m_decoder->video_decoder().pixelAspectRatio());

    OSG_NOTICE<<"ffmpeg::open("<<filename<<") size("<<s()<<", "<<t()<<") aspect ratio "<<m_decoder->video_decoder().pixelAspectRatio()<<std::endl;

#if 1
    // swscale is reported errors and then crashing when rescaling video of size less than 10 by 10.
    if (s()<=10 || t()<=10) return false;
#endif

    m_decoder->video_decoder().setUserData(this);
    m_decoder->video_decoder().setPublishCallback(publishNewFrame);

    /*if (m_decoder->audio_decoder().validContext())
    {
        OSG_NOTICE<<"Attaching FFmpegAudioStream"<<std::endl;

       getAudioStreams().push_back(new osgFFmpeg::FFmpegAudioStream(m_decoder.get()));
    }*/

    _status = PAUSED;
    applyLoopingMode();

    start(); // start thread

    return true;
}

/*

void NAMFFmpegImageStream::play()
{
    m_commands->push(CMD_PLAY);

#if 0
    // Wait for at least one frame to be published before returning the call
    OpenThreads::ScopedLock<Mutex> lock(m_mutex);

    while (duration() > 0 && ! m_frame_published_flag)
        m_frame_published_cond.wait(&m_mutex);

#endif
}
*/


/*
void NAMFFmpegImageStream::run()
{
   try
   {
      bool done = false;
      
      while (! done)
      {
         if (_status == PLAYING)
         {
            bool no_cmd;
            const Command cmd = m_commands->timedPop(no_cmd, 1);
            
            if (no_cmd)
            {
               m_decoder->readNextPacket();
            }
            else
               done = ! handleCommand(cmd);
         }
         else
         {
            done = ! handleCommand(m_commands->pop());
         }
      }
   }
   
   catch (const std::exception & error)
   {
      OSG_WARN << "FFmpegImageStream::run : " << error.what() << std::endl;
   }
   
   catch (...)
   {
      OSG_WARN << "FFmpegImageStream::run : unhandled exception" << std::endl;
   }
   
   OSG_NOTICE<<"Finished FFmpegImageStream::run()"<<std::endl;
}
*/

/*
void NAMFFmpegImageStream::applyLoopingMode()
{
    m_decoder->loop(getLoopingMode() == LOOPING);
}
*/



void NAMFFmpegImageStream::publishNewFrame(const FFmpegDecoderVideo &, void * user_data)
{
   //std::cout<<"Publishing new frame"<<std::endl;
   
   NAMFFmpegImageStream * const this_ = reinterpret_cast<NAMFFmpegImageStream*>(user_data);

#if 1
    /*this_->setImage(
        this_->m_decoder->video_decoder().width(), this_->m_decoder->video_decoder().height(), 1, GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE,
        const_cast<unsigned char *>(this_->m_decoder->video_decoder().image()), NO_DELETE
    );*/
   
   
   // Applying Skeletonization
   if(this_->_opencvImage){
      this_->createIplImageFromGLimage(const_cast<unsigned char *>(this_->m_decoder->video_decoder().image()));
      
      process(this_->_opencvImage);
      //this_->setData((unsigned char*)(this_->_opencvImage->imageData),osg::Image::NO_DELETE);
      //this_->OpenCV_to_OSG(this_->_opencvImage,this_);
     
      this_->setImage(
                      this_->m_decoder->video_decoder().width(), this_->m_decoder->video_decoder().height(), 1, GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE,
                      (unsigned char*)(this_->_opencvImage->imageData), NO_DELETE
                      );
   
      //std::cout<<"gné?"<<std::endl;
      //cvShowImage("Test", this_->_opencvImage);
      //cvWaitKey(1);
   }
   
#else
    /** \bug If viewer.realize() hasn't been already called, this doesn't work? */
    this_->dirty();
   
   std::cout<<"Nope I'm going through here"<<std::endl;
   
#endif

    OpenThreads::ScopedLock<Mutex> lock(this_->m_mutex);


   
    if (! this_->m_frame_published_flag)
    {
        this_->m_frame_published_flag = true;
        this_->m_frame_published_cond.signal();
    }
}


void NAMFFmpegImageStream::updateImage()
{
   if(_opencvImage){
      createIplImageFromGLimage(data());
      
      process(_opencvImage);
      
      setImage(
               m_decoder->video_decoder().width(), m_decoder->video_decoder().height(), 1, GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE,
               (unsigned char*)(_opencvImage->imageData), NO_DELETE
               );
   }
   
}


void NAMFFmpegImageStream::createIplImageFromGLimage (unsigned char* glImage)
{
	int hIndex = 0;
	int wIndex = 0;
   int nbComponentInTexture = _nbChannels;
   int nbComponentInOpenCV = _opencvImage->nChannels;
   
   unsigned char *glAddress = &(glImage[0]);
   for(int i = 0; i < _opencvImage->width * _opencvImage->height * nbComponentInOpenCV; i += nbComponentInOpenCV, ++wIndex) {
		if(wIndex >= _opencvImage->width) {
			wIndex = 0;
			hIndex++;
		}
		((uchar *)(_opencvImage->imageData + hIndex*_opencvImage->widthStep))[wIndex*nbComponentInOpenCV]=*glAddress;
		((uchar *)(_opencvImage->imageData + hIndex*_opencvImage->widthStep))[wIndex*nbComponentInOpenCV + 1]=*(glAddress+1);
		((uchar *)(_opencvImage->imageData + hIndex*_opencvImage->widthStep))[wIndex*nbComponentInOpenCV + 2]=*(glAddress+2);
      glAddress += nbComponentInTexture;
	}
}

void NAMFFmpegImageStream::OpenCV_to_OSG(IplImage* cvImg, osg::Image* videoTex) 
{ 
   
   //cvFlip(cvImg, cvImg, 0);
   
   videoTex->setImage( 
                      cvImg->width, //s 
                      cvImg->height, //t 
                      1, //r 
                      GL_RGBA, 
                      GL_BGRA, 
                      GL_UNSIGNED_BYTE, 
                      (unsigned char*)(cvImg->imageData), 
                      osg::Image::NO_DELETE 
                      ); 
   
   //videoTex->setOrigin( (cvImg->origin == IPL_ORIGIN_BL) ? 
   //                   osg::Image::BOTTOM_LEFT : osg::Image::TOP_LEFT); 
   
   //std::cout<<"Conversion finihed"<<std::endl;
   
   //Mutex.unlock(); 
}
