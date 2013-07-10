/*
 This file is part of UCHIYAMARKER 2.0, a software for deformable random dot markers.
 Copyright (c) 2011 Hideaki Uchiyama
 
 You can use, copy, modify and re-distribute this software
 for non-profit purposes.
 */

#ifndef PAPER_H
#define PAPER_H

#include <vector>

#include "mylib/mymat.h"
#include "mylib/myimage.h"

#include "blob.h"
#include "bloblist.h"
#include "mesh.h"
#include "pt.h"


class Paper
{
public:
	Paper(const double w, const double h);
   
	void Reprojection(const MyMat &db, MyMat &im);
	void InitHomography(const MyMat &db, const MyMat &im, const double repro);
	bool Deformed();
	void Detected();
	void Lost();
	mesh GetMesh();
	void RecoverMesh(const eblobs &blobs, const int min);
	ptlist* GetPts();
	void AddPt(const pt &p);
	void ComputeBarycenter(pt &p);
	unsigned GetID();
	void SetID(const unsigned in);
	void GetColor(double &r, double &g, double &b);
	void SetColor(const double r, const double g, const double b);
   
private:
	MyMat H;
	unsigned m_id;
	ptlist m_pts;
	Mesh m_mesh;
	double m_r,m_g,m_b;
	bool m_detected;
};

#endif



