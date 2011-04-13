/*
	This file is part of UCHIYAMARKERS, a software for random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#ifndef COMBINATION_H
#define COMBINATION_H

#include <vector>

typedef std::vector<int> combi;
typedef std::vector< combi > allcombi;

class Combination
{

public:
	void Init(const int n, const int m);
	void InitAll(const int n, const int m);

	allcombi::iterator begin();
	allcombi::iterator end();
	allcombi::const_iterator begin() const;
	allcombi::const_iterator end() const;

private:
	void Nest(int nest, int column, int n1, int n2, int k[], allcombi &result);

private:
	allcombi m_data;
};



#endif