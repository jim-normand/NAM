/*
	This file is part of UCHIYAMARKER 2.0, a software for deformable random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#include "mymat.h"

MyMat::MyMat():m_data(NULL)
{
}

MyMat::MyMat(const int rows, const int cols, const int type):m_data(NULL)
{
	Init(rows, cols, type);
}

MyMat::MyMat(const MyMat &src):m_data(NULL)
{
	Clone(src);
}

MyMat::~MyMat()
{
	Release();
} 

void MyMat::Add(const MyMat &src1, const MyMat &src2)
{
	cvAdd(src1.m_data, src2.m_data, m_data);
}

double MyMat::Angle(const MyMat &src)
{
	return acos(cvDotProduct(m_data, src.m_data)/(cvNorm(m_data)*cvNorm(src.m_data)));
}

void MyMat::Transpose(const MyMat& src)
{
	cvTranspose(src.m_data, m_data);
}

void MyMat::Inv(const MyMat& src)
{
	cvInvert(src.m_data, m_data, CV_SVD);
}

void MyMat::Scale(const double val)
{
	cvConvertScale(m_data, m_data, val);
}

void MyMat::Mul(const MyMat& src1, const MyMat& src2)
{
	cvMatMul(src1.m_data, src2.m_data, m_data);
}

void MyMat::Cross(const MyMat &src1, const MyMat &src2)
{
	cvCrossProduct(src1.m_data, src2.m_data, m_data);
}

void MyMat::RtfromH(const MyMat &invintrinsic, MyMat &R, MyMat &t) const
{
	MyMat Rdt(3,3);
	Rdt.Mul(invintrinsic, (*this));

	double scal = 0.0;

	for(int x=0;x<Rdt.m_data->rows;x++){
		scal += Rdt(x,0)*Rdt(x,0);
	}

	scal = cvSqrt(scal);

	for(int y=0;y<Rdt.m_data->cols;y++){
		for(int x=0;x<Rdt.m_data->rows;x++){
			Rdt(x,y) /= scal;
		}
	}

	MyMat r1(3,1), r2(3,1), r1xr2(3,1);

	for(int x=0;x<3;x++){
		r1(x,0) = Rdt(x,0);
		r2(x,0) = Rdt(x,1);
	}

	r1xr2.Cross(r1, r2);

	for(int y=0;y<2;y++){
		for(int x=0;x<3;x++){
			R(x,y) = Rdt(x,y);
		}
	}

	for(int x=0;x<3;x++){
		R(x,2) = -r1xr2(x,0);
		t(x,0) = Rdt(x,2);
	}
}


void MyMat::Homography(const MyMat& src, const MyMat& dst, const double repro)
{
	cvFindHomography(src.m_data, dst.m_data, m_data, CV_RANSAC, repro);
}

void MyMat::Clone(const MyMat &src)
{
	Release();
	if(src.m_data != NULL){
		m_data = cvCloneMat(src.m_data);
	}
}

void MyMat::Release()
{
	if(m_data != NULL){
		cvReleaseMat(&m_data);
		m_data = NULL;
	}
}

void MyMat::Clean()
{
	cvSetZero(m_data);
}

void MyMat::Init(const int rows, const int cols, const int type)
{
	m_data =cvCreateMat(rows, cols, type);
	Clean();
}

void MyMat::Print() const
{
	std::cout << "("<< m_data->rows << " x " << m_data->cols << ")" << std::endl;
	for(int i=0;i<m_data->rows;i++){
		for(int j=0;j<m_data->cols;j++){
			std::cout << operator()(i,j) << " ";
		}
		std::cout << std::endl;
	}
}

double& MyMat::operator() (const int row, const int col)
{
	return m_data->data.db[col+row*m_data->cols];
}

const double& MyMat::operator() (const int row, const int col) const
{
	return m_data->data.db[col+row*m_data->cols];
}