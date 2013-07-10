/*
	This file is part of UCHIYAMARKER 2.0, a software for deformable random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#ifndef MYCAM_H
#define MYCAM_H

#include "opencvpath.h"

class MyCam
{
public:
	MyCam();
	~MyCam();

	bool Get(IplImage* dst);
	bool Init(const int iw=640, const int ih=480);
   bool Init(std::string videoName, const int iw=640, const int ih=480);

private:
	CvCapture *m_cap;

public:
	int w,h;
};



#endif
