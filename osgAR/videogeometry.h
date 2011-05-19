/*
 * videogeometry.h
 *
 * @author Jean-Marie Normand
 * @copyright Centrale Innovation 2011
 *
 */
#ifndef NAM_VIDEO_GEOMETRY
#define NAM_VIDEO_GEOMETRY 1


// OSG includes
#include <osg/Matrix>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Material>
#include <osg/StateSet>
#include <osg/Light>
#include <osg/LightSource>
#include <osg/Texture2D>
#include <osg/TexMat>
#include <osg/PositionAttitudeTransform>
#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>
#include <osg/ImageStream>

// Std includes
#include <iostream>

// OpenCV includes
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#include "../skeletonization/Skeletonize.h"

using namespace osg;

#define THRESHOLD_INCR 5

void Convert_OpenCV_to_OSG_IMAGE(IplImage* cvImg, osg::Image *videoTex);

class VideoGeode : public Referenced {
public:

	VideoGeode(std::string videoName="", int cc=0, GLuint w=640, GLuint h=480, int thresh=230);
   ~VideoGeode();

	void clearMaterial();
	void prepareMaterial(Material *givenMaterial);
	
	// Test Jim
	PositionAttitudeTransform *createVideoSphere(float size, bool texRepeat);
	PositionAttitudeTransform *createVideoPlane(float sizeX, float sizeY, bool texRepeat);
   Texture2D *createVideoTexture(bool texRepeat);
   void updateVideoTexture();
   
   // accessors
   void setThreshold(int t) { _threshold = t; }
   int  getThreshold() { return _threshold; }
   // modifiers
   void increaseThreshold(int incr=THRESHOLD_INCR);
   void decreaseThreshold(int incr=THRESHOLD_INCR);

   
private:
	Material             *_material;
   
   // Skeletonization
   Skeletonize          *_skel;
   // thresholding level for image processing & skeletonization
   int                  _threshold;
   
   // other features
   int                  _camNumber; // camera number, default=0

   // OpenCV & Open Scene Graph stuff
   CvCapture            *_capture;
   IplImage             *_camImage;
   GLuint               _width;
   GLuint               _height;
   ref_ptr<Image>       _videoImage;
   ref_ptr<Texture2D>   _videoTexture;
   std::string          _videoName;
};

#endif
