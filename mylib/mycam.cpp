/*
	This file is part of UCHIYAMARKERS, a software for random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#include "mycam.h"

MyCam::MyCam():m_cap(NULL)
{
}

MyCam::~MyCam()
{
	if(m_cap != NULL){
		cvReleaseCapture(&m_cap);
		m_cap = NULL;
	}
}

bool MyCam::Get(IplImage* dst)
{
	IplImage *tmp = cvQueryFrame(m_cap);
	if(tmp == NULL){
		return false;
	}
	else{
		cvCopyImage(tmp, dst);
		return true;
	}
}

bool MyCam::Init(const int iw, const int ih) 
{ 
	if(NULL == (m_cap = cvCreateCameraCapture(0)) || NULL ==cvQueryFrame(m_cap)){
		return false;
	}
	else{
		cvSetCaptureProperty(m_cap, CV_CAP_PROP_FRAME_WIDTH, iw);
		cvSetCaptureProperty(m_cap, CV_CAP_PROP_FRAME_HEIGHT, ih); 
		w=iw, h=ih;
		return true;
	}
}






