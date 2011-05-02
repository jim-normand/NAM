#include "videogeometry.h"

// Test Skeletonization
#include "../skeletonization/Skeletonize.h"

using namespace osg;

// factor for speed of whole animation
const float ANIMATION_SPEED = 2.0;
int uniqueLightNumber = 0;

// Global Pointers (bad!)
ref_ptr<PositionAttitudeTransform>  videoPlane;
ref_ptr<PositionAttitudeTransform>  lightTransform;
ref_ptr<StateSet>                   lightStateSet;
ref_ptr<LightSource>                lightSource;
ref_ptr<VideoGeode>                 videoGeode;
ref_ptr<PositionAttitudeTransform>  model;
//ref_ptr<Node>                       model;


Light* createLight(Vec4 color)
{	
	Light *light = new Light();
	// each light must have a unique number
	light->setLightNum(uniqueLightNumber++);
	// we set the light's position via a PositionAttitudeTransform object
	light->setPosition(Vec4(0.0, 0.0, 0.0, 1.0));
	light->setDiffuse(color);
	light->setSpecular(Vec4(1.0, 1.0, 1.0, 1.0));
	light->setAmbient( Vec4(0.0, 0.0, 0.0, 1.0));
	
	return light;
}

Group* startup()
{
	// we need the scene's state set to enable the light for the entire scene
	Group *scene = new Group();
	lightStateSet = scene->getOrCreateStateSet();
	lightStateSet->ref();
	
	/*
   // create a light
	LightSource *lightSource = new LightSource();
	lightSource->setLight(createLight(Vec4(0.9, 0.9, 0.9, 1.0)));
	// enable the light for the entire scene
	lightSource->setLocalStateSetModes(StateAttribute::ON);
	lightSource->setStateSetModes(*lightStateSet, StateAttribute::ON);
	
	lightTransform = new PositionAttitudeTransform();
	lightTransform->addChild(lightSource);
	lightTransform->setPosition(Vec3(3, 0, 0));*/
	
	// create VideoGeometry
	try {
		videoGeode = new VideoGeode();
		
		// stars / starfield
		Material *material = new Material();
		material->setEmission(Material::FRONT, Vec4(1.0, 1.0, 1.0, 1.0));
		material->setAmbient(Material::FRONT,  Vec4(1.0, 1.0, 1.0, 1.0));
		material->setShininess(Material::FRONT, 25.0);
		// creating a video plane
		videoGeode->prepareMaterial(material);
      videoPlane = videoGeode->createVideoPlane(5, true);
      videoPlane->setPosition(Vec3(0,0,8));
	} catch (char *e) {
		std::cerr << e;
	}
	
	//lightTransform->addChild(sun);
	//scene->addChild(lightTransform);
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
   
   // Model
   rotate.makeRotate(time/20.0, 0,1,0);
   model->setAttitude(rotate);
}

int main(int argc, char **argv)
{
   // use an ArgumentParser object to manage the program arguments.
   osg::ArgumentParser arguments(&argc,argv);
   
   // load the nodes from the commandline arguments.
   model = new PositionAttitudeTransform();
   Node *osgModel = osgDB::readNodeFiles(arguments);
   
   // if not loaded assume no arguments passed in, try use default mode instead.
   if (!osgModel) osgModel = osgDB::readNodeFile("/Users/jim/Documents/Dev/OpenSceneGraph/OpenSceneGraph-Data/Images/cessna.osg"); // cause I can't get Xcode to search in this directory (OSG_FILE_PATH)
   
   if (!osgModel)
   {
      //return 1;
   }
   model->addChild(osgModel);
   
   Group *scene = startup();
	if (!scene) return 1;
   
   // create a light
	lightSource = new LightSource();
	lightSource->setLight(createLight(Vec4(0.9, 0.9, 0.9, 1.0)));
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
   
   
   // Creating the viewer
	osgViewer::Viewer viewer;
	viewer.setSceneData(scene);
	viewer.setCameraManipulator(new osgGA::TrackballManipulator());
	viewer.realize();
	
	while (!viewer.done()) {
		update(viewer.elapsedTime());
      videoGeode->updateVideoTexture();
		viewer.frame();
	}
}
