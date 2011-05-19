#include <osgUtil/Optimizer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/io_utils>

#include "videogeometry.h"

#include "CustomCameraManipulator.h"
#include "CustomEventHandler.h"

// Test Skeletonization
#include "../skeletonization/Skeletonize.h"

#include <iostream>

using namespace osg;
using namespace std;

// factor for speed of whole animation
const float ANIMATION_SPEED = 2.0f;
int uniqueLightNumber = 0;

// Global Pointers (bad!)

// Lighting stuff
ref_ptr<PositionAttitudeTransform>  lightTransform;
ref_ptr<StateSet>                   lightStateSet;
ref_ptr<LightSource>                lightSource;

// Video Geometry Stuff
ref_ptr<PositionAttitudeTransform>  videoPlane;
ref_ptr<VideoGeode>                 videoGeode;
std::string                         movieName;

ref_ptr<Group>                      scene;

// A 3D model
ref_ptr<PositionAttitudeTransform>  model;

// 3D positions for camera management
osg::Vec3                           cameraPosition;
osg::Vec3                           cameraTarget;
osg::Vec3                           cameraUpVector;


Light* createLight(Vec4 color)
{	
	Light *light = new Light();
	// each light must have a unique number
	light->setLightNum(uniqueLightNumber++);
	// we set the light's position via a PositionAttitudeTransform object
	light->setPosition(Vec4(0.0f, 0.0f, 0.0f, 1.0f));
	light->setDiffuse(color);
	light->setSpecular(Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	light->setAmbient( Vec4(0.0f, 0.0f, 0.0f, 1.0f));
	
	return light;
}

Group* startup()
{
	// we need the scene's state set to enable the light for the entire scene
	Group *scene = new Group();
	lightStateSet = scene->getOrCreateStateSet();
	lightStateSet->ref();
	
	// create VideoGeometry
	try {
		videoGeode = new VideoGeode();
		
		// stars / starfield
		Material *material = new Material();
		material->setEmission(Material::FRONT, Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		material->setAmbient(Material::FRONT,  Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		material->setShininess(Material::FRONT, 25.0f);
		// creating a video plane
		videoGeode->prepareMaterial(material);
      //videoPlane = videoGeode->createVideoPlane(1,1, true);
      videoPlane = videoGeode->createVideoPlane(2,2, true);
      videoPlane->setPosition(Vec3(0,0,0));
	} catch (char *e) {
		std::cerr << e;
	}
	
   scene->addChild(videoPlane);
	
	return scene;
}

void update(double time)
{
	time *= ANIMATION_SPEED;
	Quat rotate;
   
   // VideoPlane
   //rotate.makeRotate(-time/10.0, 0, 0, 1);
   //videoPlane->setAttitude(rotate);
   
   // MoviePlane
   //rotate.makeRotate(-time, 1,0,0);
   //moviePlane->setAttitude(rotate);
   
   // Model
   rotate.makeRotate(time/5.0, 0,1,0);
   model->setAttitude(rotate);
}

int main(int argc, char **argv)
{
   // use an ArgumentParser object to manage the program arguments.
   osg::ArgumentParser arguments(&argc,argv);
   
   
   // we need the scene's state set to enable the light for the entire scene
	scene = new Group();
	lightStateSet = scene->getOrCreateStateSet();
	lightStateSet->ref();
	
   cout<<"Number of arguments: "<<arguments.argc()<<endl;
   movieName = "";
   if(arguments.argc() >= 3 && arguments.isString(2)){
      movieName = arguments[2];
      cout<<"Movie name: "<<movieName<<endl;
   }
   
	// create VideoGeometry
	try {
		videoGeode = new VideoGeode(movieName);
		
		// stars / starfield
		Material *material = new Material();
		material->setEmission(Material::FRONT, Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		material->setAmbient(Material::FRONT,  Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		material->setShininess(Material::FRONT, 25.0f);
		// creating a video plane
		videoGeode->prepareMaterial(material);
      //videoPlane = videoGeode->createVideoPlane(1,1, true);
      videoPlane = videoGeode->createVideoPlane(2,2, true);
      videoPlane->setPosition(Vec3(0,0,0));
	} catch (char *e) {
		std::cerr << e;
	}
	
   scene->addChild(videoPlane);
   
      
   // load the nodes from the commandline arguments.
   model = new PositionAttitudeTransform();
        
   Node *osgModel = osgDB::readNodeFiles(arguments);
   
   // if not loaded assume no arguments passed in, try use default mode instead.
   if (!osgModel) osgModel = osgDB::readNodeFile("cessna.osg"); // I can't get Xcode to search in this directory (OSG_FILE_PATH) so this does not work
   
   if (osgModel)
   {
      // Resizing the model so that it fits in the window
      BoundingSphere bs = osgModel->getBound();
      float bsR = bs.radius();
      float targetSize = 1.0f; // we set up the projection matrix as an ortho 2d between -1 and 1 --> hence the radius of the model should be of size 1
      float factor = targetSize / bsR;
      std::cout << "Model Bounding Sphere radius is: " << bsR << std::endl;
      std::cout << "Scale factor is: " << factor << std::endl;
      model->setScale(osg::Vec3d(factor,factor,factor));
   }
   
   // Adding the model to the PAT node
   model->addChild(osgModel);
   
   //Group *scene = startup();
	//if (!scene) return 1;
   
   // create a light
	lightSource = new LightSource();
	lightSource->setLight(createLight(Vec4(0.9f, 0.9f, 0.9f, 1.0f)));
	// enable the light for the entire scene
	lightSource->setLocalStateSetModes(StateAttribute::ON);
	lightSource->setStateSetModes(*lightStateSet, StateAttribute::ON);
	
	lightTransform = new PositionAttitudeTransform();
	lightTransform->addChild(lightSource);
	lightTransform->setPosition(Vec3(0, 0, 100));
   
   // Adding the 3D model to the scene
   scene->addChild(lightTransform);
   scene->addChild(model);
   
   
   // Testing the movie
   //osg::Image* image = osgDB::readImageFile(arguments[2]);
   
   //cout<<"Number of arguments: "<<arguments.argc()<<endl;
   
   //if(arguments.argc() >= 3 && arguments.isString(2)){
      
      //cout<<"Movie name: "<<arguments[2]<<endl;
      
      
      // not sure this is actually useful
		//Material *mat = new Material();
		//mat->setEmission(Material::FRONT, Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		//mat->setAmbient(Material::FRONT,  Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		//mat->setShininess(Material::FRONT, 25.0f);
      
      //movieGeode->prepareMaterial(mat);
      //moviePlane = movieGeode->createMoviePlane(Vec3(5,5,5), 5, 5, true); // position, width, height, repeatTexture
      //scene->addChild(moviePlane);
   //}
   
    
   // Creating the viewer
	osgViewer::Viewer viewer;
   viewer.setUpViewOnSingleScreen(0);
   
   // Checking screen specs
   osgViewer::Viewer::Windows windows;
   viewer.getWindows(windows);
   int x,y,w,h;
   windows[0]->getWindowRectangle(x,y,w,h);
   cout<<"Window: "<<x<<" "<<y<<" "<<w<<" "<<h<<endl;
   
   //viewer.setUpViewInWindow(0,0,1280,800,0);
   //viewer.setUpViewInWindow(x,y,w,h,0);
   
   // Camera management
   cameraPosition = osg::Vec3(0.0f,-5.0f,0.0f);
   cameraTarget   = osg::Vec3(0.0f,0.0f,0.0f);
   cameraUpVector = osg::Vec3(0.0f,0.0f,1.0f);
   
   viewer.setSceneData(scene);
   viewer.addEventHandler( new osgViewer::StatsHandler );
	viewer.setCameraManipulator(new CustomCameraManipulator(cameraPosition, cameraTarget));

   
   ref_ptr<CustomEventHandler> customHdler = new CustomEventHandler(videoGeode.get());
   viewer.addEventHandler(customHdler.get());
   
   
   // New test 11/05/2011
   viewer.getCamera()->setViewMatrixAsLookAt(cameraPosition, cameraTarget,cameraUpVector);
   //viewer.getCamera()->setProjectionMatrixAsOrtho2D(-1,1,-1,1);
   viewer.getCamera()->setProjectionMatrixAsOrtho(-1,1,-1,1,-5,5);
   
   //osg::Matrixd m,m2;
   //m = viewer.getCamera()->getInverseViewMatrix();
   //std::cout << "Camera inverse view matrix: " << m << std::endl;
   
   // Compulsory call
   viewer.realize();
  
	while (!viewer.done()) {
		update(viewer.elapsedTime());
      videoGeode->updateVideoTexture();
     
      //m2 = viewer.getCamera()->getInverseViewMatrix();
      //std::cout << "Camera inverse view matrix: " << m2 << std::endl;
      
      viewer.frame();
   }
}
