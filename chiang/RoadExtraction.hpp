#ifndef ROADEXTRACTION_HPP_
#define ROADEXTRACTION_HPP_

#include <opencv/cv.h>
#include <opencv/highgui.h>

void SetMatElem(CvMat *mat, int row, int col, int channel, int val);

class RoadExtraction
{
public:
	RoadExtraction();
	~RoadExtraction();

public:
	void LoadImage(const char * filename);
	void ShowSource();
	void InitModified();
	void InitKMean();
	void MeanShift(int spatialRadius, int colorRadius);
	void ManualKMean(int K, int b,int g, int r);
	void KMean(int K);
	void HoughLines();
	void ErodeDilateTest();
	void ChiangKMean(IplImage* image,int profondeur);
	void ConnexCount(IplImage* image);

public:
	static int NonBlackPix(IplImage* image);
	static int ContourNumber(IplImage* image);
	static bool KeepFilter(IplImage* image);

private:
	IplImage* source;
	IplImage* modified;
	IplImage* progressiveKMean;
};

#endif
