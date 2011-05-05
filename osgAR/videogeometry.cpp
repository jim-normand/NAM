/*
 * videogeometry.cpp
 *
 * @author Jean-Marie Normand
 * @copyright Centrale Innovation 2011
 *
 */

#include "videogeometry.h"
#include "../skeletonization/Skeletonize.h"
#include "NAM-FFmpegImageStream.hpp"

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

VideoGeode::VideoGeode(GLuint w, GLuint h):_width(w),_height(h)
{
	clearMaterial();

   // OpenCV
   _capture = cvCreateCameraCapture(0);
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


PositionAttitudeTransform *VideoGeode::createVideoPlane(float size, bool texRepeat)
{
	// vertex array
	Vec3Array *vertexArray = new Vec3Array();
	
   vertexArray->push_back(Vec3(-size, 0, -size));
	vertexArray->push_back(Vec3(size, 0, -size));
	vertexArray->push_back(Vec3(size, 0, size));
	vertexArray->push_back(Vec3(-size, 0, size));
	
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


/////////////////////////////////////////////////////////////////////////////// 
///////////////////////    CLASS   MOVIEGEODE   /////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////// 

MovieGeode::MovieGeode(std::string movieName)
{
	clearMaterial();
   
   // ImageStream
   //_movieImage    = osgDB::readImageFile(movieName);
   cout<<"Image from Movie: "<<_movieImage<<endl;
   //_movieStream   = dynamic_cast<osg::ImageStream*>(_movieImage);
   _movieStream   = new NAMFFmpegImageStream();
   _movieStream->open(movieName);
   if(!_movieStream)
      cout << "Error! Could not load movie file"<<endl;
   else {
      _movieStream->play();
   }

   // Creating texturead Quad
}

void MovieGeode::clearMaterial()
{
	_mat = new Material();
}

void MovieGeode::prepareMaterial(Material *givenMaterial)
{
	_mat = givenMaterial;
}

// Getters
Image* MovieGeode::getMovieImage()
{
   return _movieImage;
}

ImageStream* MovieGeode::getMovieStream()
{
   return _movieStream;
}

PositionAttitudeTransform *MovieGeode::createMoviePlane(const osg::Vec3& pos, float width,float height, bool texRepeat)
//osg::Geometry* myCreateTexturedQuadGeometry(const osg::Vec3& pos,float width,float height, osg::Image* image, bool useTextureRectangle, bool xyPlane, bool option_flip)
{
   bool flip = _movieStream->getOrigin()==osg::Image::TOP_LEFT;
   //bool flip = _movieImage->getOrigin()==osg::Image::TOP_LEFT; OK
   //if (option_flip) flip = !flip;
   
   osg::Geometry* pictureQuad = osg::createTexturedQuadGeometry(pos,
                                                                osg::Vec3(width,0.0f,0.0f),
                                                                osg::Vec3(0.0f,0.0f,height),
                                                                0.0f, flip ? 1.0f : 0.0f , 1.0f, flip ? 0.0f : 1.0f);
   
   //_movieTexture = new osg::Texture2D(_movieImage); OK
   _movieTexture = new osg::Texture2D(dynamic_cast<osg::Image*>(_movieStream));
   _movieTexture->setResizeNonPowerOfTwoHint(false);
   _movieTexture->setFilter(Texture::MIN_FILTER, Texture::LINEAR_MIPMAP_LINEAR);
   _movieTexture->setFilter(Texture::MAG_FILTER, Texture::LINEAR);
   //_movieTexture->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
   //_movieTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
   //_movieTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
   
   // handle repeat
   if (texRepeat) {
      _movieTexture->setWrap(Texture::WRAP_S, Texture::REPEAT);
      _movieTexture->setWrap(Texture::WRAP_T, Texture::REPEAT);
   } else {
      _movieTexture->setWrap(Texture::WRAP_S, Texture::CLAMP);
      _movieTexture->setWrap(Texture::WRAP_T, Texture::CLAMP);
   }
   
   pictureQuad->getOrCreateStateSet()->setTextureAttributeAndModes(0,_movieTexture,osg::StateAttribute::ON);
   
   osg::ref_ptr<osg::Geode> geode = new osg::Geode;
   geode->addDrawable(pictureQuad);
   
   PositionAttitudeTransform *planeTransform = new PositionAttitudeTransform();
	planeTransform->addChild(geode);
	return planeTransform;
}


PositionAttitudeTransform *MovieGeode::createMovieSphere(float size, bool texRepeat)
{
   osg::ref_ptr<Geode> sphere = new Geode();
	sphere->addDrawable(new ShapeDrawable(new Sphere(Vec3(0, 0, 4), size)));
	
	// assign the material to the sphere
	StateSet *sphereStateSet = sphere->getOrCreateStateSet();
	sphereStateSet->ref();
	sphereStateSet->setAttribute(_mat);
	
   _movieTexture = new osg::Texture2D(_movieImage);
   _movieTexture->setResizeNonPowerOfTwoHint(false);
   _movieTexture->setFilter(Texture::MIN_FILTER, Texture::LINEAR_MIPMAP_LINEAR);
   _movieTexture->setFilter(Texture::MAG_FILTER, Texture::LINEAR);
  
   // handle repeat
   if (texRepeat) {
      _movieTexture->setWrap(Texture::WRAP_S, Texture::REPEAT);
      _movieTexture->setWrap(Texture::WRAP_T, Texture::REPEAT);
   } else {
      _movieTexture->setWrap(Texture::WRAP_S, Texture::CLAMP);
      _movieTexture->setWrap(Texture::WRAP_T, Texture::CLAMP);
   }
   
   
   try {
      sphereStateSet->setTextureAttributeAndModes(0, _movieTexture, StateAttribute::ON);
   } catch (char *e) {
      throw e;
   }
	
	PositionAttitudeTransform *sphereTransform = new PositionAttitudeTransform();
	sphereTransform->addChild(sphere);
	return sphereTransform;
}


void MovieGeode::updateMovieTexture()
{
   _movieStream->updateImage();
}