/*
 *  CustomEventHandler.cpp
 *  RandomDotMarkers
 *
 *  Created by Jean-Marie Normand on 16/05/11.
 *  Copyright 2011 Centrale Innovation. All rights reserved.
 *
 */

#include "CustomEventHandler.h"
#include <iostream>

CustomEventHandler::CustomEventHandler(VideoGeode * videoNode):_videoGeode(videoNode)
{

}

bool CustomEventHandler::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa){

   switch (ea.getEventType()) {
      case osgGA::GUIEventAdapter::KEYDOWN:
         switch (ea.getKey()) {
            case 'u':case 'U':
               std::cout<<"U pressed"<<std::endl;
               _videoGeode->increaseThreshold();
               break;
            case 'i':case 'I':
               std::cout<<"I pressed"<<std::endl;
               _videoGeode->decreaseThreshold();
               break;

            default:
               return false;
               break;
         }
         break;
      default:
         return false;
         break;
   }
   return false;
   
}