/*
	This file is part of UCHIYAMARKERS, a software for random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#ifndef MYIMAGE_H
#define MYIMAGE_H

#include "opencvpath.h"

class MyImage
{
public:
	MyImage();
	MyImage(const MyImage &src);
	~MyImage();

	void Flip(const MyImage &src);
	void Release();
	void Clone(const MyImage &src);
	void Binarization(const MyImage &src, int threshold);
	void GraytoColor(const MyImage &src);
	void ColortoGray(const MyImage &src);
	void Swap(MyImage &src);
	void Resize(const MyImage &src);
	void Init(const int iw, const int ih, const int c=3, const int d=8);
	void Clean();
	void Save(const char *name) const;
	void Load(const char *name);

	void Circle(const int x, const int y, const int radius=1, const int thickness=1, const unsigned char r=0, const unsigned char g=0, const unsigned char b=0);

	operator const char* () const;
	operator char* ();
	operator const IplImage* () const;
	operator IplImage* ();

private:
	IplImage* m_img;

public:
	int w,h;
};

#endif