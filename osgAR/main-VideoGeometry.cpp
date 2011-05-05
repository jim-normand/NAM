#include <osgUtil/Optimizer>
#include <osgViewer/ViewerEventHandlers>


#include "videogeometry.h"

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



// A 3D model
ref_ptr<PositionAttitudeTransform>  model;
//ref_ptr<Node>                       model;


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
      videoPlane = videoGeode->createVideoPlane(5, true);
      videoPlane->setPosition(Vec3(0,0,8));
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
   rotate.makeRotate(-time/10.0, 0, 0, 1);
   videoPlane->setAttitude(rotate);
   
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
   
   std::string libName = osgDB::Registry::instance()->createLibraryNameForExtension("ffmpeg");
   osgDB::Registry::instance()->loadLibrary(libName);
      
   // load the nodes from the commandline arguments.
   model = new PositionAttitudeTransform();
   Node *osgModel = osgDB::readNodeFiles(arguments);
   
   // if not loaded assume no arguments passed in, try use default mode instead.
   if (!osgModel) osgModel = osgDB::readNodeFile("cessna.osg"); // I can't get Xcode to search in this directory (OSG_FILE_PATH) so this does not work
   
   if (!osgModel)
   {
      //return 1;
   }
   model->addChild(osgModel);
   
   Group *scene = startup();
	if (!scene) return 1;
   
   // create a light
	lightSource = new LightSource();
	lightSource->setLight(createLight(Vec4(0.9f, 0.9f, 0.9f, 1.0f)));
	// enable the light for the entire scene
	lightSource->setLocalStateSetModes(StateAttribute::ON);
	lightSource->setStateSetModes(*lightStateSet, StateAttribute::ON);
	
	lightTransform = new PositionAttitudeTransform();
	lightTransform->addChild(lightSource);
	lightTransform->setPosition(Vec3(0, -2, 5));
   
   //lightTransform->addChild(model);
   
   // Adding the 3D model to the scene
   scene->addChild(lightTransform);
   scene->addChild(model);
   
   
   // Testing the movie
   //osg::Image* image = osgDB::readImageFile(arguments[2]);
   
   cout<<"Number of arguments: "<<arguments.argc()<<endl;
   
   if(arguments.argc() >= 3 && arguments.isString(2)){
      
      cout<<"Movie name: "<<arguments[2]<<endl;
      
      
      // not sure this is actually useful
		Material *mat = new Material();
		mat->setEmission(Material::FRONT, Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		mat->setAmbient(Material::FRONT,  Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		mat->setShininess(Material::FRONT, 25.0f);
      
      //movieGeode->prepareMaterial(mat);
      //moviePlane = movieGeode->createMoviePlane(Vec3(5,5,5), 5, 5, true); // position, width, height, repeatTexture
      //scene->addChild(moviePlane);
   }
   
   // Creating the viewer
	osgViewer::Viewer viewer;
	viewer.setSceneData(scene);
   viewer.addEventHandler( new osgViewer::StatsHandler );
	viewer.setCameraManipulator(new osgGA::TrackballManipulator());
	viewer.realize();
   

	while (!viewer.done()) {
		update(viewer.elapsedTime());
      videoGeode->updateVideoTexture();
      // TEST MOVIE
      //movieGeode->updateMovieTexture();
		viewer.frame();
   }
}
