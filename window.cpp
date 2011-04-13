/*
	This file is part of UCHIYAMARKERS, a software for random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#include "window.h"

void window::Add(const int idx, const int idy, blob *b)
{
	bucket[idx+idy*w].push_back(b);
}

void window::Clear()
{
	for(int i=0;i<num;i++){
		bucket[i].clear();
	}
}

void window::Init(const int iw, const int ih, const int isize)
{
	w = static_cast<unsigned>(iw), h = static_cast<unsigned>(ih);
	num = iw*ih;
	bucket.resize(num);
	size = isize;
}

const bblobs* window::operator() (const int idx, const int idy) const
{
	return &bucket[idx+idy*w];
}


