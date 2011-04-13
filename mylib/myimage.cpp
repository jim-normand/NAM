/*
	This file is part of UCHIYAMARKERS, a software for random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#include "myimage.h"

MyImage::MyImage():m_img(NULL)
{
}

MyImage::MyImage(const MyImage &src):m_img(NULL)
{
	Clone(src);
}

MyImage::~MyImage()
{
	Release();
}


void MyImage::Flip(const MyImage &src)
{
	cvFlip(src.m_img, m_img);
}

void MyImage::Release()
{
	if(m_img != NULL){
		cvReleaseImage(&m_img);
		m_img = NULL;
	}
}

void MyImage::Clone(const MyImage &src)
{
	Release();
	m_img = cvCloneImage(src.m_img);
	w = m_img->width;
	h = m_img->height;
}

void MyImage::Binarization(const MyImage &src, int threshold)
{
	cvThreshold(src.m_img, m_img, threshold, 255, CV_THRESH_BINARY_INV);
}

void MyImage::GraytoColor(const MyImage &src)
{
	cvCvtColor(src.m_img, m_img, CV_GRAY2BGR);
}

void MyImage::ColortoGray(const MyImage &src)
{
	cvCvtColor(src.m_img, m_img, CV_BGR2GRAY);
}

void MyImage::Swap(MyImage &src)
{
	IplImage *tmp = m_img; 
	m_img = src.m_img; 
	src.m_img = tmp;
}

void MyImage::Resize(const MyImage &src)
{
	cvResize(src.m_img, m_img);
}

void MyImage::Init(const int iw, const int ih, const int c, const int d)
{
	w = iw, h = ih;
	m_img = cvCreateImage(cvSize(iw, ih), d, c);
	Clean();
}

void MyImage::Clean()
{
	cvSetZero(m_img);
}

void MyImage::Save(const char *name) const
{
	cvSaveImage(name, m_img);
}

void MyImage::Load(const char *name)
{
	Release();
	m_img = cvLoadImage(name);
	w = m_img->width;
	h = m_img->height;
}

void MyImage::Circle(const int x, const int y, const int radius, const int thickness, const unsigned char r, const unsigned char g, const unsigned char b)
{
	cvCircle(m_img,cvPoint(x,y),radius,CV_RGB(r,g,b),thickness);
}

MyImage::operator const char* () const
{
	return m_img->imageData;
}

MyImage::operator char* ()
{
	return m_img->imageData;
}

MyImage::operator const IplImage* () const
{
	return m_img;
}

MyImage::operator IplImage* ()
{
	return m_img;
}

