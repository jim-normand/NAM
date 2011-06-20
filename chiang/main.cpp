#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>
#include "RoadExtraction.hpp"


int main(int argc, char *argv[])
{
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	RoadExtraction* CapturedMap = new RoadExtraction();

	CapturedMap->LoadImage("chiang2.bmp");
	CapturedMap->ShowSource();
	CapturedMap->InitModified();
	CapturedMap->MeanShift(3,25);

	cvWaitKey(0);

	CapturedMap->InitKMean();

	cvWaitKey(0);
	return 0;
}

