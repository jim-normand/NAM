/*
 * videogeometry.cpp
 *
 * @author Jean-Marie Normand
 * @copyright Centrale Innovation 2011
 *
 */

#include "videogeometry.h"
#include "../skeletonization/Skeletonize.h"
//#include "NAM-FFmpegImageStream.hpp"

using namespace osg;
using namespace std;

void Convert_OpenCV_to_OSG_IMAGE(IplImage* cvImg, osg::Image* videoTex) 
{ 
   
   /*
    std::cout<<"Trying to convert IplImage to osg::Image"<<std::endl;
   std::cout<<"Information on the image: "<<cvImg<<std::endl;
   std::cout<<"Width: "<<cvImg->width<<" Height: "<<cvImg->height;
   std::cout<<" Channels: "<<cvImg->nChannels<<std::endl;
   */
   
   cvFlip(cvImg, cvImg, 0);
   
   videoTex->setImage( 
                     cvImg->width, //s 
                     cvImg->height, //t 
                     1, //r 
                     GL_RGB8,//GL_RGBA,// 
                     GL_BGR_EXT, //GL_BGRA,//
                     GL_UNSIGNED_BYTE, 
                     (unsigned char*)(cvImg->imageData), 
                      osg::Image::NO_DELETE 
                     ); 
   
   //videoTex->setOrigin( (cvImg->origin == IPL_ORIGIN_BL) ? 
   //                   osg::Image::BOTTOM_LEFT : osg::Image::TOP_LEFT); 
   
   //std::cout<<"Conversion finihed"<<std::endl;
   
   //Mutex.unlock(); 
}


/////////////////////////////////////////////////////////////////////////////// 
///////////////////////    CLASS   VIDEOGEODE   /////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////// 

VideoGeode::VideoGeode(GLuint w, GLuint h, int cc):_width(w),_height(h),camNumber(cc)
{
	clearMaterial();

   // OpenCV
   _capture = cvCreateCameraCapture(camNumber);
   cvSetCaptureProperty(_capture, CV_CAP_PROP_FRAME_WIDTH, _width);
   cvSetCaptureProperty(_capture, CV_CAP_PROP_FRAME_HEIGHT, _height);
   _camImage = cvCreateImage(cvSize(_width, _height), IPL_DEPTH_8U, 3);
   cvCopy(cvQueryFrame(_capture),_camImage,0);
   
   _videoImage = new osg::Image();
}

void VideoGeode::clearMaterial()
{
	_material = new Material();
}

void VideoGeode::prepareMaterial(Material *givenMaterial)
{
	_material = givenMaterial;
}

/*
Texture2D *VideoGeode::createTexture(std::string texName, bool texRepeat)
{
	// load texture file
	Image *image = osgDB::readImageFile(texName);
	if (!image) {
		throw "Couldn't load texture.";
	}
	
	// create texture
	Texture2D *texture = new Texture2D;
	texture->setDataVariance(Object::DYNAMIC);
	texture->setFilter(Texture::MIN_FILTER, Texture::LINEAR_MIPMAP_LINEAR);
	texture->setFilter(Texture::MAG_FILTER, Texture::LINEAR);
	
	// handle repeat
	if (texRepeat) {
		texture->setWrap(Texture::WRAP_S, Texture::REPEAT);
		texture->setWrap(Texture::WRAP_T, Texture::REPEAT);
	} else {
		texture->setWrap(Texture::WRAP_S, Texture::CLAMP);
		texture->setWrap(Texture::WRAP_T, Texture::CLAMP);
	}
	texture->setImage(image);
	
	return texture;
}
*/

// Test Jim
Texture2D *VideoGeode::createVideoTexture(bool texRepeat)
{
	// load texture file 
  /* _videoImage->allocateImage(_width, 
                              _height, 
                              1, 
                              GL_RGBA_INTEGER_EXT, 
                              GL_UNSIGNED_BYTE); */
	Convert_OpenCV_to_OSG_IMAGE(_camImage,_videoImage);
	
	// create texture
	_videoTexture = new Texture2D;
	_videoTexture->setDataVariance(Object::DYNAMIC);
	_videoTexture->setFilter(Texture::MIN_FILTER, Texture::LINEAR_MIPMAP_LINEAR);
	_videoTexture->setFilter(Texture::MAG_FILTER, Texture::LINEAR);
   
   // Test
   _videoTexture->setResizeNonPowerOfTwoHint(false);
	
	// handle repeat
	if (texRepeat) {
		_videoTexture->setWrap(Texture::WRAP_S, Texture::REPEAT);
		_videoTexture->setWrap(Texture::WRAP_T, Texture::REPEAT);
	} else {
		_videoTexture->setWrap(Texture::WRAP_S, Texture::CLAMP);
		_videoTexture->setWrap(Texture::WRAP_T, Texture::CLAMP);
	}
	_videoTexture->setImage(_videoImage);
	
	return _videoTexture;
}

void VideoGeode::updateVideoTexture()
{
   cvCopy(cvQueryFrame(_capture),_camImage,0);
   process(_camImage);
   Convert_OpenCV_to_OSG_IMAGE(_camImage,_videoImage);
   _videoImage->dirty();
   _videoTexture->dirtyTextureObject();
}

PositionAttitudeTransform *VideoGeode::createVideoSphere(float size, bool texRepeat)
{
	Geode *sphere = new Geode();
	sphere->addDrawable(new ShapeDrawable(new Sphere(Vec3(0, 0, 4), size)));
	
	// assign the material to the sphere
	StateSet *sphereStateSet = sphere->getOrCreateStateSet();
	sphereStateSet->ref();
	sphereStateSet->setAttribute(_material);
	
   try {
      sphereStateSet->setTextureAttributeAndModes(0, createVideoTexture(texRepeat), StateAttribute::ON);
   } catch (char *e) {
      throw e;
   }
	
	PositionAttitudeTransform *sphereTransform = new PositionAttitudeTransform();
	sphereTransform->addChild(sphere);
	return sphereTransform;
}


PositionAttitudeTransform *VideoGeode::createVideoPlane(float sizeX, float sizeY, bool texRepeat)
{
	// vertex array
	Vec3Array *vertexArray = new Vec3Array();
   
   sizeX /= 2.0;
   sizeY /= 2.0;
	
   vertexArray->push_back(Vec3(-sizeX, 0, -sizeY));
	vertexArray->push_back(Vec3(sizeX, 0, -sizeY));
	vertexArray->push_back(Vec3(sizeX, 0, sizeY));
	vertexArray->push_back(Vec3(-sizeX, 0, sizeY));

	
   /*vertexArray->push_back(Vec3(-sizeX, -sizeY, 0));
	vertexArray->push_back(Vec3(sizeX, -sizeY, 0));
	vertexArray->push_back(Vec3(sizeX, sizeY, 0));
	vertexArray->push_back(Vec3(-sizeX, sizeY, 0));*/
   
	// face array
	DrawElementsUInt *faceArray = new DrawElementsUInt(PrimitiveSet::TRIANGLES, 0);
	
	faceArray->push_back(0); // face 1
	faceArray->push_back(1);
	faceArray->push_back(2);
	faceArray->push_back(2); // face 2
	faceArray->push_back(3);
	faceArray->push_back(0);
	
	// normal array
	Vec3Array *normalArray = new Vec3Array();
	normalArray->push_back(Vec3(0, 0, 1));
	
	// normal index
	TemplateIndexArray<unsigned int, Array::UIntArrayType, 24, 4> *normalIndexArray;
	normalIndexArray = new TemplateIndexArray<unsigned int, Array::UIntArrayType, 24, 4>();
	
	normalIndexArray->push_back(0);
	normalIndexArray->push_back(0);
	normalIndexArray->push_back(0);
	normalIndexArray->push_back(0);
	
	// texture coordinates
	Vec2Array *texCoords = new Vec2Array();
	texCoords->push_back(Vec2(0.0f, 0.0f));
	texCoords->push_back(Vec2(1.0f, 0.0f));
	texCoords->push_back(Vec2(1.0f, 1.0f));
	texCoords->push_back(Vec2(0.0f, 1.0f));
	
	Geometry *geometry = new Geometry();
	geometry->setVertexArray(vertexArray);
	geometry->setNormalArray(normalArray);
	geometry->setNormalIndices(normalIndexArray);
	geometry->setNormalBinding(Geometry::BIND_PER_VERTEX);
	geometry->setTexCoordArray(0, texCoords);
	geometry->addPrimitiveSet(faceArray);
	
	Geode *plane = new Geode();
	plane->addDrawable(geometry);
	
	// assign the material to the sphere
	StateSet *planeStateSet = plane->getOrCreateStateSet();
	planeStateSet->ref();
	planeStateSet->setAttribute(_material);
	
   try {
		planeStateSet->setTextureAttributeAndModes(0, createVideoTexture(texRepeat), StateAttribute::ON);
   } catch (char *e) {
      throw e;
   }
	
	PositionAttitudeTransform *planeTransform = new PositionAttitudeTransform();
	planeTransform->addChild(plane);
	return planeTransform;
}
// \End Test Jim


