/*
 *  CustomCameraManipulator.cpp
 *  RandomDotMarkers
 *
 *  Created by Jean-Marie Normand on 11/05/11.
 *  Copyright 2011 Centrale Innovation. All rights reserved.
 *
 */

#include "CustomCameraManipulator.h"
#include <osg/ComputeBoundsVisitor>
#include <osg/io_utils>

using namespace osg;
using namespace osgGA;

/// Constructor.
CustomCameraManipulator::CustomCameraManipulator(osg::Vec3 pos, osg::Vec3 target, int flags )
: inherited( flags )
{
   _homePosition     = pos;
   _targetPosition   = target;
   setVerticalAxisFixed( false );
}


/// Constructor.
CustomCameraManipulator::CustomCameraManipulator( const CustomCameraManipulator& tm, const CopyOp& copyOp )
: inherited( tm, copyOp )
{
}


bool CustomCameraManipulator::performMovementLeftMouseButton( const double eventTimeDelta, const double dx, const double dy )
{
   // rotate camera
   if( getVerticalAxisFixed() )
      rotateWithFixedVertical( dx, dy );
   else
      rotateTrackball( _ga_t0->getXnormalized(), _ga_t0->getYnormalized(),
                      _ga_t1->getXnormalized(), _ga_t1->getYnormalized(),
                      getThrowScale( eventTimeDelta ) );
   return true;
}
   
bool CustomCameraManipulator::performMovementMiddleMouseButton( const double eventTimeDelta, const double dx, const double dy )
{
   return false;
}

bool CustomCameraManipulator::performMovementRightMouseButton( const double eventTimeDelta, const double dx, const double dy )
{
   return false;
}


// Test
void CustomCameraManipulator::computeHomePosition(const osg::Camera *camera, bool useBoundingBox)
{
   if (getNode())
   {
      osg::BoundingSphere boundingSphere;
      
      OSG_INFO<<" CustomCameraManipulator::computeHomePosition("<<camera<<", "<<useBoundingBox<<")"<<std::endl;
      
      if (useBoundingBox)
      {
         // compute bounding box
         // (bounding box computes model center more precisely than bounding sphere)
         osg::ComputeBoundsVisitor cbVisitor;
         getNode()->accept(cbVisitor);
         osg::BoundingBox &bb = cbVisitor.getBoundingBox();
         
         if (bb.valid()) boundingSphere.expandBy(bb);
         else boundingSphere = getNode()->getBound();
      }
      else
      {
         // compute bounding sphere
         boundingSphere = getNode()->getBound();
      }
      
      OSG_INFO<<"    boundingSphere.center() = ("<<boundingSphere.center()<<")"<<std::endl;
      OSG_INFO<<"    boundingSphere.radius() = "<<boundingSphere.radius()<<std::endl;
      
      // set dist to default
      double dist = 3.5f * boundingSphere.radius();
      
      if (camera)
      {
         
         // try to compute dist from frustrum
         double left,right,bottom,top,zNear,zFar;
         if (camera->getProjectionMatrixAsFrustum(left,right,bottom,top,zNear,zFar))
         {
            double vertical2 = fabs(right - left) / zNear / 2.;
            double horizontal2 = fabs(top - bottom) / zNear / 2.;
            double dim = horizontal2 < vertical2 ? horizontal2 : vertical2;
            double viewAngle = atan2(dim,1.);
            dist = boundingSphere.radius() / sin(viewAngle);
         }
         else
         {
            // try to compute dist from ortho
            if (camera->getProjectionMatrixAsOrtho(left,right,bottom,top,zNear,zFar))
            {
               dist = fabs(zFar - zNear) / 2.;
            }
         }
      }
      
      // set home position
      setHomePosition(_homePosition,//boundingSphere.center() + osg::Vec3d(0.0,-dist,0.0f),
                      _targetPosition,//boundingSphere.center(),
                      osg::Vec3d(0.0f,0.0f,1.0f),
                      _autoComputeHomePosition);
   }
   
}
