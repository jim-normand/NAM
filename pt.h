/*
	This file is part of UCHIYAMARKER 2.0, a software for deformable random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#ifndef PT_H
#define PT_H

struct pt
{
	double rawx, rawy, x, y;
	int centerid[3];
	double centerval[3];
	bool inside;
	unsigned id;
	descriptors descs;
};

typedef std::vector<pt> ptlist;

#endif