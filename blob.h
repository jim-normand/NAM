/*
	This file is part of UCHIYAMARKERS, a software for random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#ifndef BLOB_H
#define BLOB_H

#include <vector>
#include <set>

#include "mylib/opencvpath.h"

const unsigned NOID = 0xffffffff;

#define PAPERBIT 16
#define PTBIT 16

struct xy
{
	unsigned x, y;
};

struct blob;

typedef std::vector<blob*> neighbors;
typedef std::set<unsigned> descriptors;

struct blob
{
	double Triangle(const blob *p1, const blob *p2) const;
	double Angle(const blob *src) const;
	double Distance2(const blob *src) const;
	double Distance(const double x, const double y) const;
	void Clear();
	bool Valid() const;
	unsigned PaperID() const;
	unsigned PtID() const;
	void SetID(const short unsigned paperid, const short unsigned ptid);

	int sumx, sumy, sum;
	double rawx, rawy, x, y;
	xy idxy;
	unsigned id;
	neighbors nblobs;
	descriptors descs;
	bool found;
};


#endif