//
//  main.cpp
//  osgAR
//
//  Created by Guillaume Moreau on 26/04/11.
//  Copyright 2011 Ecole Centrale de Nantes. All rights reserved.
//

#include <osg/GLExtensions>
#include <osg/Node>
#include <osg/Geometry>
#include <osg/Notify>
#include <osg/MatrixTransform>
#include <osg/Texture2D>
#include <osg/TextureRectangle>
#include <osg/Stencil>
#include <osg/ColorMask>
#include <osg/Depth>
#include <osg/Billboard>
#include <osg/Material>
#include <osg/AnimationPath>
#include <osg/ImageStream>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>

#include <osgUtil/SmoothingVisitor>

#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui.hpp>


#include <iostream>
using namespace std;

CvCapture *captureMethod;

bool liveVideo = false;

osg::Image* Convert_OpenCV_to_OSG_IMAGE(IplImage* cvImg) 
{ 
   osg::Image* osgImg1 = new osg::Image(); 
   osgImg1->allocateImage(cvImg->width, cvImg->height, 1, 
                          GL_RGBA_INTEGER_EXT, GL_UNSIGNED_BYTE); 
   osgImg1->setImage( 
                    cvImg->width, //s 
                    cvImg->height, //t 
                    1, //r 
                    3, 
                    GL_BGR, 
                    GL_UNSIGNED_BYTE, 
                    (unsigned char*)(cvImg->imageData), 
                    osg::Image::NO_DELETE 
                    ); 
   
   osgImg1->setOrigin( (cvImg->origin == IPL_ORIGIN_BL) ? 
                     osg::Image::TOP_LEFT : osg::Image::BOTTOM_LEFT); 
   
   
   //Mutex.unlock(); 
   return osgImg1; 
}

// call back which creates a deformation field to oscillate the model.
class MyGeometryCallback : 
public osg::Drawable::UpdateCallback, 
public osg::Drawable::AttributeFunctor
{
public:
   
   MyGeometryCallback(const osg::Vec3& o,
                      const osg::Vec3& x,const osg::Vec3& y,const osg::Vec3& z,
                      double period,double xphase,double amplitude):
   _firstCall(true),
   _startTime(0.0),
   _time(0.0),
   _period(period),
   _xphase(xphase),
   _amplitude(amplitude),
   _origin(o),
   _xAxis(x),
   _yAxis(y),
   _zAxis(z) {}
   
   virtual void update(osg::NodeVisitor* nv,osg::Drawable* drawable)
   {
      // OpenThreads::Thread::microSleep( 1000 );
      
      const osg::FrameStamp* fs = nv->getFrameStamp();
      double simulationTime = fs->getSimulationTime();
      if (_firstCall)
      {
         _firstCall = false;
         _startTime = simulationTime;
      }
      
      _time = simulationTime-_startTime;
      
      drawable->accept(*this);
      drawable->dirtyBound();
      
      osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(drawable);
      if (geometry)
      {
         osgUtil::SmoothingVisitor::smooth(*geometry);
      }
      
   }
   
   virtual void apply(osg::Drawable::AttributeType type,unsigned int count,osg::Vec3* begin) 
   {
      if (type == osg::Drawable::VERTICES)
      {
         const float TwoPI=2.0f*osg::PI;
         const float phase = -_time/_period;
         
         osg::Vec3* end = begin+count;
         for (osg::Vec3* itr=begin;itr<end;++itr)
         {
            osg::Vec3 dv(*itr-_origin);
            osg::Vec3 local(dv*_xAxis,dv*_yAxis,dv*_zAxis);
            
            local.z() = local.x()*_amplitude*
            sinf(TwoPI*(phase+local.x()*_xphase)); 
            
            (*itr) = _origin + 
            _xAxis*local.x()+
            _yAxis*local.y()+
            _zAxis*local.z();
         }
      }
   }
   
   bool    _firstCall;
   
   double  _startTime;
   double  _time;
   
   double  _period;
   double  _xphase;
   float   _amplitude;
   
   osg::Vec3   _origin;
   osg::Vec3   _xAxis;
   osg::Vec3   _yAxis;
   osg::Vec3   _zAxis;
   
};

struct MyCameraPostDrawCallback : public osg::Camera::DrawCallback
{
   MyCameraPostDrawCallback(osg::Image* image):
   _image(image)
   {
   }
   
   virtual void operator () (const osg::Camera& /*camera*/) const
   {
      if (_image && _image->getPixelFormat()==GL_RGBA && _image->getDataType()==GL_UNSIGNED_BYTE)
      {
         // we'll pick out the center 1/2 of the whole image,
         int column_start = _image->s()/4;
         int column_end = 3*column_start;
         
         int row_start = _image->t()/4;
         int row_end = 3*row_start;
         
         
         // and then invert these pixels
         for(int r=row_start; r<row_end; ++r)
         {
            unsigned char* data = _image->data(column_start, r);
            for(int c=column_start; c<column_end; ++c)
            {
               (*data) = 255-(*data); ++data;
               (*data) = 255-(*data); ++data;
               (*data) = 255-(*data); ++data;
               (*data) = 255; ++data;
            }
         }
         
         
         // dirty the image (increments the modified count) so that any textures
         // using the image can be informed that they need to update.
         _image->dirty();
      }
      else if (_image && _image->getPixelFormat()==GL_RGBA && _image->getDataType()==GL_FLOAT)
      {
         // we'll pick out the center 1/2 of the whole image,
         int column_start = _image->s()/4;
         int column_end = 3*column_start;
         
         int row_start = _image->t()/4;
         int row_end = 3*row_start;
         
         // and then invert these pixels
         for(int r=row_start; r<row_end; ++r)
         {
            float* data = (float*)_image->data(column_start, r);
            for(int c=column_start; c<column_end; ++c)
            {
               (*data) = 1.0f-(*data); ++data;
               (*data) = 1.0f-(*data); ++data;
               (*data) = 1.0f-(*data); ++data;
               (*data) = 1.0f; ++data;
            }
         }
         
         // dirty the image (increments the modified count) so that any textures
         // using the image can be informed that they need to update.
         _image->dirty();
      }
      
   }
   
   osg::Image* _image;
};


osg::Geometry* myCreateTexturedQuadGeometry(const osg::Vec3& pos,float width,float height, osg::Image* image, bool useTextureRectangle, bool xyPlane, bool option_flip)
{
   bool flip = image->getOrigin()==osg::Image::TOP_LEFT;
   if (option_flip) flip = !flip;
   
   if (useTextureRectangle)
   {
      osg::Geometry* pictureQuad = osg::createTexturedQuadGeometry(pos,
                                                                   osg::Vec3(width,0.0f,0.0f),
                                                                   xyPlane ? osg::Vec3(0.0f,height,0.0f) : osg::Vec3(0.0f,0.0f,height),
                                                                   0.0f, flip ? image->t() : 0.0, image->s(), flip ? 0.0 : image->t());
      
      osg::TextureRectangle* texture = new osg::TextureRectangle(image);
      texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
      texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
      
      
      pictureQuad->getOrCreateStateSet()->setTextureAttributeAndModes(0,
                                                                      texture,
                                                                      osg::StateAttribute::ON);
      
      return pictureQuad;
   }
   else
   {
      osg::Geometry* pictureQuad = osg::createTexturedQuadGeometry(pos,
                                                                   osg::Vec3(width,0.0f,0.0f),
                                                                   xyPlane ? osg::Vec3(0.0f,height,0.0f) : osg::Vec3(0.0f,0.0f,height),
                                                                   0.0f, flip ? 1.0f : 0.0f , 1.0f, flip ? 0.0f : 1.0f);
      
      osg::Texture2D* texture = new osg::Texture2D(image);
      texture->setResizeNonPowerOfTwoHint(false);
      texture->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
      texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
      texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
      
      
      pictureQuad->getOrCreateStateSet()->setTextureAttributeAndModes(0,
                                                                      texture,
                                                                      osg::StateAttribute::ON);
      
      return pictureQuad;
   }
}

osg::Node* createPreRenderSubGraph(osg::Node* subgraph, 
                                   unsigned tex_width, unsigned tex_height, 
                                   osg::Camera::RenderTargetImplementation renderImplementation, 
                                   bool useImage, bool useTextureRectangle, bool useHDR, 
                                   unsigned int samples, unsigned int colorSamples, std::string movie_file)
{
   if (!subgraph) return 0;
   
   // create a group to contain the flag and the pre rendering camera.
   osg::Group* parent = new osg::Group;
   
   // texture to render to and to use for rendering of flag.
   osg::Texture* texture = 0;
   if (useTextureRectangle)
   {
      osg::TextureRectangle* textureRect = new osg::TextureRectangle;
      textureRect->setTextureSize(tex_width, tex_height);
      textureRect->setInternalFormat(GL_RGBA);
      textureRect->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
      textureRect->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
      
      texture = textureRect;
   }
   else
   {
      osg::Texture2D* texture2D = new osg::Texture2D;
      texture2D->setTextureSize(tex_width, tex_height);
      texture2D->setInternalFormat(GL_RGBA);
      texture2D->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
      texture2D->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
      
      texture = texture2D;
   } 
   
   if (useHDR)
   {
      texture->setInternalFormat(GL_RGBA16F_ARB);
      texture->setSourceFormat(GL_RGBA);
      texture->setSourceType(GL_FLOAT);
   }
   
   // first create the geometry of the flag of which to view.
   { 
      // create the to visualize.
      osg::Geometry* polyGeom = new osg::Geometry();
      
      polyGeom->setName( "PolyGeom" );
      
      polyGeom->setDataVariance( osg::Object::DYNAMIC );
      polyGeom->setSupportsDisplayList(false);
      
      osg::Vec3 origin(0.0f,0.0f,0.0f);
      osg::Vec3 xAxis(1.0f,0.0f,0.0f);
      osg::Vec3 yAxis(0.0f,0.0f,1.0f);
      osg::Vec3 zAxis(0.0f,-1.0f,0.0f);
      float height = 100.0f;
      float width = 200.0f;
      int noSteps = 20;
      
      osg::Vec3Array* vertices = new osg::Vec3Array;
      osg::Vec3 bottom = origin;
      osg::Vec3 top = origin; top.z()+= height;
      osg::Vec3 dv = xAxis*(width/((float)(noSteps-1)));
      
      osg::Vec2Array* texcoords = new osg::Vec2Array;
      
      // note, when we use TextureRectangle we have to scale the tex coords up to compensate.
      osg::Vec2 bottom_texcoord(0.0f,0.0f);
      osg::Vec2 top_texcoord(0.0f, useTextureRectangle ? tex_height : 1.0f);
      osg::Vec2 dv_texcoord((useTextureRectangle ? tex_width : 1.0f)/(float)(noSteps-1),0.0f);
      
      for(int i=0;i<noSteps;++i)
      {
         vertices->push_back(top);
         vertices->push_back(bottom);
         top+=dv;
         bottom+=dv;
         
         texcoords->push_back(top_texcoord);
         texcoords->push_back(bottom_texcoord);
         top_texcoord+=dv_texcoord;
         bottom_texcoord+=dv_texcoord;
      }
      
      
      // pass the created vertex array to the points geometry object.
      polyGeom->setVertexArray(vertices);
      
      polyGeom->setTexCoordArray(0,texcoords);
      
      osg::Vec4Array* colors = new osg::Vec4Array;
      colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
      polyGeom->setColorArray(colors);
      polyGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
      
      polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP,0,vertices->size()));
      
      // new we need to add the texture to the Drawable, we do so by creating a 
      // StateSet to contain the Texture StateAttribute.
      osg::StateSet* stateset = new osg::StateSet;
      
      stateset->setTextureAttributeAndModes(0, texture,osg::StateAttribute::ON);
      
      polyGeom->setStateSet(stateset);
      
      polyGeom->setUpdateCallback(new MyGeometryCallback(origin,xAxis,yAxis,zAxis,1.0,1.0/width,0.2f));
      
      osg::Geode* geode = new osg::Geode();
      geode->addDrawable(polyGeom);
      
      parent->addChild(geode);
      
   }
   
   // then create the camera node to do the render to texture
   {   
      
      osg::Vec3 pos(0.0f,0.0f,0.0f);
      osg::Vec3 topleft = pos;
      osg::Vec3 bottomright = pos;
      
      bool xyPlane = true;
      bool useTextureRectangle = true;
      bool flip = false;
      
      osg::ref_ptr<osg::Geode> geode = new osg::Geode;
      IplImage *cvImage=NULL;
      if (liveVideo) {
         cvImage = cvQueryFrame(captureMethod);
         osg::notify(osg::NOTICE) << "getting frame from video";
      }
      else {
         //cvImage = cvLoadImage(movie_file.c_str());
         cvImage = cvQueryFrame(captureMethod);
      }
      osg::Image* image = Convert_OpenCV_to_OSG_IMAGE(cvImage);
      //osg::Image* image = osgDB::readImageFile(movie_file);
      //osg::ImageStream* imagestream = dynamic_cast<osg::ImageStream*>(image);
      //if (imagestream) 
      //{
      //   imagestream->play();
      //}
      
      if (image)
      {
         osg::notify(osg::NOTICE)<<"image->s()"<<image->s()<<" image-t()="<<image->t()<<" aspectRatio="<<image->getPixelAspectRatio()<<std::endl;
         
         float width = image->s() * image->getPixelAspectRatio();
         float height = image->t();
         
         osg::ref_ptr<osg::Drawable> drawable = myCreateTexturedQuadGeometry(pos, width, height,image, useTextureRectangle, xyPlane, flip);
         
         if (image->isImageTranslucent())
         {
            osg::notify(osg::NOTICE)<<"Transparent movie, enabling blending."<<std::endl;
            
            drawable->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
            drawable->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
         }
         drawable->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
         geode->addDrawable(drawable.get());
         
         bottomright = pos + osg::Vec3(width,height,0.0f);
         
         if (xyPlane) pos.y() += height*1.05f;
         else pos.z() += height*1.05f;
      }
      
      osg::Camera* camera = new osg::Camera;
      
      // set up the background color and clear mask.
      camera->setClearColor(osg::Vec4(1.0f,0.1f,0.3f,1.0f));
      camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      
      // set up projection.
      //camera->setProjectionMatrixAsFrustum(-proj_right,proj_right,-proj_top,proj_top,znear,zfar);
      
      // set view
      camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
      //camera->setViewMatrixAsLookAt(bs.center()-osg::Vec3(0.0f,1.0f,0.0f)*bs.radius(),bs.center(),osg::Vec3(0.0f,0.0f,1.0f));
      
      float screenAspectRatio = tex_width/(double)tex_height;
      float modelAspectRatio = (bottomright.x()-topleft.x())/(bottomright.y()-topleft.y());
      
      camera->setViewMatrix(osg::Matrix::identity());
      
      
      osg::Vec3 center = (bottomright + topleft)*0.5f;
      osg::Vec3 dx(bottomright.x()-center.x(), 0.0f, 0.0f);
      osg::Vec3 dy(0.0f, topleft.y()-center.y(), 0.0f);
      
      float ratio = modelAspectRatio/screenAspectRatio;
      
      if (ratio>1.0f)
      {
         // use model width as the control on model size.
         bottomright = center + dx - dy * ratio;
         topleft = center - dx + dy * ratio;
      }
      else
      {
         // use model height as the control on model size.
         bottomright = center + dx / ratio - dy;
         topleft = center - dx / ratio + dy;
      }
      
      camera->setProjectionMatrixAsOrtho2D(topleft.x(),bottomright.x(),topleft.y(),bottomright.y());
      
      
      // set viewport
      camera->setViewport(0,0,tex_width,tex_height);
      
      // set the camera to render before the main camera.
      camera->setRenderOrder(osg::Camera::PRE_RENDER);
      
      // tell the camera to use OpenGL frame buffer object where supported.
      camera->setRenderTargetImplementation(renderImplementation);
      
      
      // attach the texture and use it as the color buffer.
      camera->attach(osg::Camera::COLOR_BUFFER, texture, 
                     0, 0, false,
                     samples, colorSamples);
      
      // add subgraph to render
      camera->addChild(geode);
      
      parent->addChild(camera);
      
   }
   
   // then create the camera node to do the render to texture
   {    
      osg::Camera* camera = new osg::Camera;
      
      // set up the background color and clear mask.
      camera->setClearColor(osg::Vec4(0.1f,0.1f,0.3f,1.0f));
      camera->setClearMask(GL_DEPTH_BUFFER_BIT);
      
      const osg::BoundingSphere& bs = subgraph->getBound();
      if (!bs.valid())
      {
         return subgraph;
      }
      
      float znear = 1.0f*bs.radius();
      float zfar  = 3.0f*bs.radius();
      
      // 2:1 aspect ratio as per flag geometry below.
      float proj_top   = 0.25f*znear;
      float proj_right = 0.5f*znear;
      
      znear *= 0.9f;
      zfar *= 1.1f;
      
      // set up projection.
      camera->setProjectionMatrixAsFrustum(-proj_right,proj_right,-proj_top,proj_top,znear,zfar);
      
      // set view
      camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
      camera->setViewMatrixAsLookAt(bs.center()-osg::Vec3(0.0f,5.0f,0.0f)*bs.radius(),bs.center(),osg::Vec3(0.0f,0.0f,1.0f));
      
      // set viewport
      camera->setViewport(0,0,tex_width,tex_height);
      
      // set the camera to render before the main camera.
      camera->setRenderOrder(osg::Camera::PRE_RENDER);
      
      // tell the camera to use OpenGL frame buffer object where supported.
      camera->setRenderTargetImplementation(renderImplementation);
      
      
      if (useImage)
      {
         osg::Image* image = new osg::Image;
         image->allocateImage(tex_width, tex_height, 1, GL_RGBA, GL_UNSIGNED_BYTE);
         //image->allocateImage(tex_width, tex_height, 1, GL_RGBA, GL_FLOAT);
         
         // attach the image so its copied on each frame.
         camera->attach(osg::Camera::COLOR_BUFFER, image,
                        samples, colorSamples);
         
         camera->setPostDrawCallback(new MyCameraPostDrawCallback(image));
         
         // Rather than attach the texture directly to illustrate the texture's ability to
         // detect an image update and to subload the image onto the texture.  You needn't
         // do this when using an Image for copying to, as a separate camera->attach(..)
         // would suffice as well, but we'll do it the long way round here just for demonstration
         // purposes (long way round meaning we'll need to copy image to main memory, then
         // copy it back to the graphics card to the texture in one frame).
         // The long way round allows us to manually modify the copied image via the callback
         // and then let this modified image by reloaded back.
         texture->setImage(0, image);
      }
      else
      {
         // attach the texture and use it as the color buffer.
         camera->attach(osg::Camera::COLOR_BUFFER, texture, 
                        0, 0, false,
                        samples, colorSamples);
      }
      
      
      // add subgraph to render
      camera->addChild(subgraph);
      
      parent->addChild(camera);
      
   }    
   
   return parent;
}

int main( int argc, char **argv )
{
   // use an ArgumentParser object to manage the program arguments.
   osg::ArgumentParser arguments(&argc,argv);
   
   // set up the usage document, in case we need to print out how to use this program.
   arguments.getApplicationUsage()->setDescription(arguments.getApplicationName()+" is the example which demonstrates pre rendering of scene to a texture, and then apply this texture to geometry.");
   arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName()+" [options] filename ...");
   arguments.getApplicationUsage()->addCommandLineOption("-h or --help","Display this information");
   arguments.getApplicationUsage()->addCommandLineOption("--fbo","Use Frame Buffer Object for render to texture, where supported.");
   arguments.getApplicationUsage()->addCommandLineOption("--fb","Use FrameBuffer for render to texture.");
   arguments.getApplicationUsage()->addCommandLineOption("--pbuffer","Use Pixel Buffer for render to texture, where supported.");
   arguments.getApplicationUsage()->addCommandLineOption("--window","Use a separate Window for render to texture.");
   arguments.getApplicationUsage()->addCommandLineOption("--width","Set the width of the render to texture.");
   arguments.getApplicationUsage()->addCommandLineOption("--height","Set the height of the render to texture.");
   arguments.getApplicationUsage()->addCommandLineOption("--image","Render to an image, then apply a post draw callback to it, and use this image to update a texture.");
   arguments.getApplicationUsage()->addCommandLineOption("--texture-rectangle","Use osg::TextureRectangle for doing the render to texture to.");
   
   arguments.getApplicationUsage()->addCommandLineOption("--live-video","Use live video with OpenCV");
   
   // construct the viewer.
   osgViewer::Viewer viewer(arguments);
   viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);
   
   // add stats
   viewer.addEventHandler( new osgViewer::StatsHandler() );
   
   // add the record camera path handler
   viewer.addEventHandler(new osgViewer::RecordCameraPathHandler);
   
   // add the threading handler
   viewer.addEventHandler( new osgViewer::ThreadingHandler() );
   
   // if user request help write it out to cout.
   if (arguments.read("-h") || arguments.read("--help"))
   {
      arguments.getApplicationUsage()->write(std::cout);
      return 1;
   }
   
   unsigned int tex_width = 1024;
   unsigned int tex_height = 512;
   unsigned int samples = 0;
   unsigned int colorSamples = 0;
   
   if (arguments.read("--live-video")) {
      // run on the OpenCV capture
      liveVideo = true;
      captureMethod = cvCaptureFromCAM(0);
      if (captureMethod == NULL) {
         osg::notify(osg::FATAL)<<"couldn't open camera stream. exit."<<std::endl;
      }
   }
   else {
      liveVideo = false;
      captureMethod = cvCaptureFromFile(arguments[2]);
   }
   
   while (arguments.read("--width", tex_width)) {}
   while (arguments.read("--height", tex_height)) {}
   
   osg::Camera::RenderTargetImplementation renderImplementation = osg::Camera::FRAME_BUFFER_OBJECT;
   
   while (arguments.read("--fbo")) { renderImplementation = osg::Camera::FRAME_BUFFER_OBJECT; }
   while (arguments.read("--pbuffer")) { renderImplementation = osg::Camera::PIXEL_BUFFER; }
   while (arguments.read("--pbuffer-rtt")) { renderImplementation = osg::Camera::PIXEL_BUFFER_RTT; }
   while (arguments.read("--fb")) { renderImplementation = osg::Camera::FRAME_BUFFER; }
   while (arguments.read("--window")) { renderImplementation = osg::Camera::SEPERATE_WINDOW; }
   while (arguments.read("--fbo-samples", samples)) {}
   while (arguments.read("--color-samples", colorSamples)) {}
   
   bool useImage = false;
   while (arguments.read("--image")) { useImage = true; }
   
   bool useTextureRectangle = false;
   while (arguments.read("--texture-rectangle")) { useTextureRectangle = true; }
   
   bool useHDR = false;
   while (arguments.read("--hdr")) { useHDR = true; }
   
   
   // load the nodes from the commandline arguments.
   osg::Node* loadedModel = osgDB::readNodeFiles(arguments);
   
   // if not loaded assume no arguments passed in, try use default mode instead.
   if (!loadedModel) loadedModel = osgDB::readNodeFile("cessna.osg");
   
   if (!loadedModel)
   {
      return 1;
   }
   
   // create a transform to spin the model.
   osg::MatrixTransform* loadedModelTransform = new osg::MatrixTransform;
   loadedModelTransform->addChild(loadedModel);
   
   osg::NodeCallback* nc = new osg::AnimationPathCallback(loadedModelTransform->getBound().center(),osg::Vec3(0.0f,0.0f,1.0f),osg::inDegrees(45.0f));
   loadedModelTransform->setUpdateCallback(nc);
   
   osg::Group* rootNode = new osg::Group();
   rootNode->addChild(createPreRenderSubGraph(loadedModelTransform,tex_width,tex_height, renderImplementation, useImage, useTextureRectangle, useHDR, samples, colorSamples, arguments[2]));
   
   osgDB::writeNodeFile(*rootNode, "test.ive");
   
   // add model to the viewer.
   viewer.setSceneData( rootNode );
   
   return viewer.run();
}
