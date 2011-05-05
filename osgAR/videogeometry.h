/*
 * videogeometry.h
 *
 * @author Jean-Marie Normand
 * @copyright Centrale Innovation 2011
 *
 */

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

// Big test
//#include "NAM-FFmpegImageStream.hpp"

using namespace osg;


void Convert_OpenCV_to_OSG_IMAGE(IplImage* cvImg, osg::Image *videoTex);

class VideoGeode : public Referenced {
public:
	VideoGeode(GLuint w=640, GLuint h=480);
	void clearMaterial();
	void prepareMaterial(Material *givenMaterial);
	
	// Test Jim
	PositionAttitudeTransform *createVideoSphere(float size, bool texRepeat);
	PositionAttitudeTransform *createVideoPlane(float size, bool texRepeat);
   Texture2D *createVideoTexture(bool texRepeat);
   void updateVideoTexture();
   
private:
	Material    *_material;
   
   // Test OpenCV
   CvCapture   *_capture;
   IplImage    *_camImage;
   GLuint      _width;
   GLuint      _height;
   Image       *_videoImage;
   Texture2D   *_videoTexture;
};


