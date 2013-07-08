//
//  skelChiangtest.cpp
//  RandomDotMarkers
//
//  Created by Guillaume Moreau on 01/03/12.
//  Copyright 2012 Ecole Centrale de Nantes. All rights reserved.
//

/**
 objectif de ce petit bout de code : tester imgProcChiang
*/
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "imgProcChiang.h"

int main(int argc,char *argv[]) {
   if (argc != 2) {
      std::cout << "usage: " << argv[0] << " imageFile" << std::endl;
      return -1;
   }
   IplImage *curImage = cvLoadImage(argv[1]);
   
   imgProcChiang *_skel = new imgProcChiang(curImage);
   _skel->setDebugVizMode();
   
   std::cout << "start" << std::endl;
   
   _skel->processImage(curImage);
   
   cvWaitKey();
   
}
