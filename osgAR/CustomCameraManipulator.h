/*
 *  CustomCameraManipulator.h
 *  RandomDotMarkers
 *
 *  Created by Jean-Marie Normand on 11/05/11.
 *  Copyright 2011 Centrale Innovation. All rights reserved.
 *
 */

#ifndef NAM_CUSTOM_CAMERA_MANIPULATOR
#define NAM_CUSTOM_CAMERA_MANIPULATOR 1

#include <osgGA/OrbitManipulator>

using namespace osgGA;

class CustomCameraManipulator : public OrbitManipulator
{
   typedef OrbitManipulator inherited;
   
public:
   
   CustomCameraManipulator( osg::Vec3 pos, osg::Vec3 target, int flags = DEFAULT_SETTINGS );
   CustomCameraManipulator( const CustomCameraManipulator& tm,
                            const osg::CopyOp& copyOp = osg::CopyOp::SHALLOW_COPY );
   
   void computeHomePosition(const osg::Camera *camera = NULL, bool useBoundingBox = false);

   
   bool performMovementLeftMouseButton( const double eventTimeDelta, const double dx, const double dy );
   bool performMovementMiddleMouseButton( const double eventTimeDelta, const double dx, const double dy );
   bool performMovementRightMouseButton( const double eventTimeDelta, const double dx, const double dy );
   
protected:
   osg::Vec3      _homePosition;
   osg::Vec3      _targetPosition;
};

#endif /* NAM_CUSTOM_CAMERA_MANIPULATOR */
