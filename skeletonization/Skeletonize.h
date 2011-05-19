#ifndef SJ_SKELETONIZE_H_22022011142139
#define SJ_SKELETONIZE_H_22022011142139

#include <list>

#include <opencv/cv.h>
#include <opencv/highgui.h>

class Skeletonize
{
public:
   Skeletonize(IplImage *src);
   void operator()(void);
   void skeletonize();
   
   void updateImage(IplImage *img);
   
   void processImage(IplImage *colorSrcImg, int threshold=230);

   
private:
   IplImage *m_src;
   static const int s_lut[256];
   
   int thin(unsigned int pass);
};


#endif
