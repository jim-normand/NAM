/*
	This file is part of UCHIYAMARKERS, a software for random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#ifndef WINDOW_H
#define WINDOW_H

#include <vector>

#include "blob.h"

typedef std::vector<blob*> bblobs;

struct window
{
	void Add(const int idx, const int idy, blob *b);
	void Clear();
	void Init(const int iw, const int ih, const int isize);

	const bblobs* operator() (const int idx, const int idy) const;

	std::vector<bblobs> bucket;

	unsigned w, h; 
	int num, size;
};


#endif