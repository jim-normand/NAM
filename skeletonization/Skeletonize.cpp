#include "Skeletonize.h"
#include "Intersections.h"

using namespace std;

const int Skeletonize::s_lut[256] =
{0,0,0,1,0,0,1,3,0,0,3,1,1,0,1,3,0,0,0,0,0,0,0,0,2,0,2,0,3,0,3,3,
   0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,3,0,2,2,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   2,0,0,0,0,0,0,0,2,0,0,0,2,0,0,0,3,0,0,0,0,0,0,0,3,0,0,0,3,0,2,0,
   0,0,3,1,0,0,1,3,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
   3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   2,3,1,3,0,0,1,3,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   2,3,0,1,0,0,0,1,0,0,0,0,0,0,0,0,3,3,0,1,0,0,0,0,2,2,0,0,2,0,0,0};

Skeletonize::Skeletonize(IplImage *src) : m_src(src)
{
}

/* ImageJ skeletonize algorithm */
void Skeletonize::operator()(void)
{
   // Border blackout
   cvRectangle(m_src,
               cvPoint(0, 0), cvPoint(m_src->width - 1, m_src->height - 1),
               cvScalarAll(0));
   
   int pixelsRemoved;
   int pass = 0;
   
   do
   {
      pixelsRemoved = thin(pass++);
      pixelsRemoved += thin(pass++);
   } while (pixelsRemoved > 0);
   
}

int Skeletonize::thin(unsigned int pass)
{
   const int cMin = 1, cMax = m_src->width - 2;
   const int lMin = 1, lMax = m_src->height - 2;
   const int rowOffset = m_src->widthStep;
   int pixelsRemoved = 0;
   
   unsigned char *data = new unsigned char[m_src->imageSize];
   copy(m_src->imageData, m_src->imageData + m_src->imageSize, data);
   
   for (int l = lMin; l <= lMax; ++l)
   {
      for (int c = cMin; c <= cMax; ++c)
      {
         int offset = l * rowOffset + c;
         if (data[offset] == 255) // White
         {
            // Get neighbours
            unsigned char index = 0;
            if (data[offset-rowOffset-1] == 255) index |= 1; // p1
            if (data[offset-rowOffset  ] == 255) index |= 2; // p2
            if (data[offset-rowOffset+1] == 255) index |= 4; // p3
            if (data[offset          +1] == 255) index |= 8; // p6
            if (data[offset+rowOffset+1] == 255) index |= 16; // p9
            if (data[offset+rowOffset  ] == 255) index |= 32; // p8
            if (data[offset+rowOffset-1] == 255) index |= 64; // p7
            if (data[offset          -1] == 255) index |= 128; // p4
            
            int code = s_lut[index];
            if ((pass & 1) == 1) // odd pass
            {
               if (code == 2 || code == 3)
               {
                  m_src->imageData[offset] = 0;
                  ++pixelsRemoved;
               }
            }
            else // even pass
            {
               if (code == 1 || code == 3)
               {
                  m_src->imageData[offset] = 0;
                  ++pixelsRemoved;
               }
            }
         }
      }
   }
   
   delete[] data;
   
   return pixelsRemoved;
}

void process(IplImage *colorSrcImg, int threshold)
{
   IplImage *graySrcImg = cvCreateImage(cvGetSize(colorSrcImg), IPL_DEPTH_8U, 1);
   
   /***** Source *****/
   //cvShowImage("Source", colorSrcImg);
   cvConvertImage(colorSrcImg, graySrcImg);
   
   /***** Threshold *****/
   cvThreshold(graySrcImg, graySrcImg, threshold, 255, CV_THRESH_BINARY);
   cvMorphologyEx(graySrcImg, graySrcImg, NULL,
                  cvCreateStructuringElementEx(5, 5, 2, 2, CV_SHAPE_RECT),
                  CV_MOP_CLOSE);
   //cvShowImage("Binary", graySrcImg);
   
   /***** Skeleton *****/
   Skeletonize skel(graySrcImg);
   skel();
  // cvShowImage("Skeleton", graySrcImg);
   
   /***** Intersections *****/
   vector<CvPoint> crossroads = Intersections(graySrcImg)();
   for (unsigned int i = 0; i < crossroads.size(); ++i)
   {
      cvCircle(colorSrcImg, crossroads[i], 2, CV_RGB(255, 0, 0), -1);
   }
   //cvShowImage("Intersections", colorSrcImg);
   
   cvReleaseImage(&graySrcImg);
}
