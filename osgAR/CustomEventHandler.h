/*
 *  CustomEventHandler.h
 *  RandomDotMarkers
 *
 *  Created by Jean-Marie Normand on 16/05/11.
 *  Copyright 2011 Centrale Innovation. All rights reserved.
 *
 */

#ifndef NAM_CUSTOM_EVENT_HANDLER
#define NAM_CUSTOM_EVENT_HANDLER 1

#include <osgGA/GUIEventHandler>

#include "videogeometry.h";

class CustomEventHandler : public osgGA::GUIEventHandler {

public:
   CustomEventHandler(VideoGeode* videoNode);
   
   virtual bool handle(const osgGA::GUIEventAdapter &ea,
                       osgGA::GUIActionAdapter &aa);


protected:
   // not sure what to put here
   ref_ptr<VideoGeode>  _videoGeode;
};

#endif