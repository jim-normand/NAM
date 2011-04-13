/*
	This file is part of UCHIYAMARKERS, a software for random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#include "blob.h"

double blob::Triangle(const blob *p1, const blob *p2) const 
{ 
	return 0.5*fabs((p1->x-x)*(p2->y-y)-(p1->y-y)*(p2->x-x)); 
}

double blob::Angle(const blob *src) const
{
	return cvFastArctan(static_cast<float>(src->y-y), static_cast<float>(src->x-x)); 
}

double blob::Distance2(const blob *src) const
{
	return (x-src->x)*(x-src->x)+(y-src->y)*(y-src->y);
}

double blob::Distance(const double ix, const double iy) const
{
	return cvSqrt((x-ix)*(x-ix)+(y-iy)*(y-iy));
}

void blob::Clear()
{
	sumx = sumy = sum = 0;
	rawx = rawy = x = y = 0.0;
	idxy.x = idxy.y = 0;
	id = NOID;
	nblobs.clear();
	descs.clear();
	found = false;
}

bool blob::Valid() const
{
	return (id != NOID);
}


unsigned blob::PaperID() const 
{
	return (id >> PTBIT); 
}

unsigned blob::PtID() const 
{
	return (0x0000ffff & id); 
}

void blob::SetID(const short unsigned paperid, const short unsigned ptid)
{
	id = paperid;
	id = id << PTBIT;
	id = id | ptid;
}
