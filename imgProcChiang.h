//
//  imgProcChiang.h
//  RandomDotMarkers
//
//  Created by Guillaume Moreau on 13/07/11.
//  Copyright 2011 Ecole Centrale de Nantes. All rights reserved.
//

#include "Skeletonize.h"

class imgProcChiang : public Skeletonize {
private:
   bool thresTable[256][256][256];
   
public:
   // constructor
   imgProcChiang(IplImage *src);
   
   void processImage(IplImage *colorSrcImg, int threshold=230);
};



