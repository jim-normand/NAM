/*#include <osg/Matrix>
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
#include <iostream>*/

#include "videogeometry.h"



using namespace osg;

// factor for speed of whole animation
const float ANIMATION_SPEED = 2.0;

PositionAttitudeTransform *videoPlane;

int uniqueLightNumber = 0;
PositionAttitudeTransform *lightTransform;
StateSet *lightStateSet;

VideoGeode *videoGeode;




Light *createLight(Vec4 color)
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

Node *startup()
{
	// we need the scene's state set to enable the light for the entire scene
	Group *scene = new Group();
	lightStateSet = scene->getOrCreateStateSet();
	lightStateSet->ref();
	
	// create a light
	LightSource *lightSource = new LightSource();
	lightSource->setLight(createLight(Vec4(0.9, 0.9, 0.9, 1.0)));
	// enable the light for the entire scene
	lightSource->setLocalStateSetModes(StateAttribute::ON);
	lightSource->setStateSetModes(*lightStateSet, StateAttribute::ON);
	
	lightTransform = new PositionAttitudeTransform();
	lightTransform->addChild(lightSource);
	lightTransform->setPosition(Vec3(3, 0, 0));
	
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
      videoPlane = videoGeode->createVideoPlane(300, true);    
	} catch (char *e) {
		std::cerr << e;
	}
	
	//lightTransform->addChild(sun);
	scene->addChild(lightTransform);
   scene->addChild(videoPlane);
	
	return scene;
}

void update(double time)
{
	time *= ANIMATION_SPEED;
	const float SUN_DISTANCE = 1500;
	const float BASE_PERIOD = -time / 50;
	const float BASE_ROTATION = time / 2;
	Quat rotate;
   
   // Jim
   videoPlane->setPosition(Vec3(500,500,500));
}

int main()
{
	Node *scene = startup();
	if (!scene) return 1;
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
