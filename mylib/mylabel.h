/*
	This file is part of UCHIYAMARKERS, a software for random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#ifndef MYLABEL_H
#define MYLABEL_H

#include <cstring>
#include <vector>

class MyLabel
{
public:
	void Labeling(const char *binary);
	void Init(const int iw, const int ih);

	int& operator() (const int x, const int y); 
	const int& operator() (const int x, const int y) const;

private:
	std::vector<int> m_label;

public:
	int w, h;
	int num;
};


#endif