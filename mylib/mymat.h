/*
	This file is part of UCHIYAMARKERS, a software for random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#ifndef MYMAT_H
#define MYMAT_H

#include <iostream>

#include "opencvpath.h"

class MyMat
{
public:
	MyMat();
	MyMat(const int rows, const int cols, const int type=CV_64F);
	MyMat(const MyMat &src);
	~MyMat();

	void Inv(const MyMat& src);
	void Mul(const MyMat& src1, const MyMat& src2);
	void Cross(const MyMat &src1, const MyMat &src2);
	void RtfromH(const MyMat &invintrinsic, MyMat &R, MyMat &t) const;
	void Homography(const MyMat& src, const MyMat& dst, const double repro);
	void Clone(const MyMat &src);
	void Release();
	void Clean();
	void Init(const int rows, const int cols, const int type=CV_64F);
	void Print() const;

	double& operator() (const int row, const int col);
	const double& operator() (const int row, const int col) const;

private:
	CvMat *m_data;
};


#endif