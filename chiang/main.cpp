#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "RoadExtraction.hpp"


int main(int argc, char *argv[])
{
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	RoadExtraction* CapturedMap = new RoadExtraction();

   if (argc != 2) {
      std::cout << "usage: " << argv[0] << " imageFile" << endl;
      return -1;
   }
	CapturedMap->LoadImage(argv[1]);
	CapturedMap->ShowSource();
	CapturedMap->InitModified();
	CapturedMap->MeanShift(3,25);

	cvWaitKey(0);

	CapturedMap->InitKMean();

	cvWaitKey(0);
	return 0;
}

