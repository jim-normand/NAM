/*
	This file is part of UCHIYAMARKERS, a software for random dot markers.
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

struct pt
{
	double rawx, rawy, x, y;
	unsigned id;
	descriptors descs;
};

typedef std::vector<pt> ptlist;

struct Paper
{
	Paper();

	MyMat H,R,t;
	unsigned id;
	ptlist pts;
	double r,g,b;
};

#endif



