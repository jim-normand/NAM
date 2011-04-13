/*
	This file is part of UCHIYAMARKERS, a software for random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#ifndef LLAHPARAM_H
#define LLAHPARAM_H

#include <cmath>
#include <vector>

#include "mylib/combination.h"

struct LLAHParam
{
	LLAHParam();

	unsigned Quantization(double val) const;

	int n;
	int m;
	int k;
	int stepbit;
	int hashbit;

	unsigned hashsize;

	std::vector<double> step;

	Combination nm;
	Combination mk;
	Combination rnm;
	Combination rmk;
};

#endif