//
//  imgProcGM.h
//  RandomDotMarkers
//
//  Created by Guillaume Moreau on 19/05/11.
//  Copyright 2011 Ecole Centrale de Nantes. All rights reserved.
//

#include "Skeletonize.h"

class imgProcGM : public Skeletonize {
public:
   void processImage(IplImage *colorSrcImg, int threshold=230);
};