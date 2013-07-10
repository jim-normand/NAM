/*
	This file is part of UCHIYAMARKER 2.0, a software for deformable random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#ifndef MESH_H
#define MESH_H

#include <vector>
#include <cmath>

#include "mylib/mymat.h"
#include "bloblist.h"
#include "pt.h"

const double PI = 3.14159265;
const double PI2 = PI*2;

struct vertex
{
	double x, y;
};

struct triangle
{
	unsigned id[3];
};

typedef std::vector<triangle> triangles;
typedef std::vector<vertex> vertice;

struct mesh
{
	triangles *tri;
	vertice *ver;
	vertice *initver;
};

class Mesh
{
public:
	mesh GetTrianges();
	
	bool Recover(const eblobs &blobs, const ptlist &pts, const int min);
	void Init(const MyMat &H);
	bool GetBarycenter(const double x, const double y, int &id1, double &val1, int &id2, double &val2, int &id3, double &val3);  
	void Base(const double markerwidth, const double markerheight, const double trianglesize);
	
private:
	bool Inside(const double x, const double y, const int id1, double &val1, const int id2, double &val2, const int id3, double &val3);
	void SetK();
	void Make();

private:
	int m_numw, m_numh, m_num;
	double m_size;
	triangles m_triangles;
	vertice m_vertice, m_initvertice;
	MyMat K, A, bx, by, KA, invKA, initX, initY, X, Y;
};

#endif



