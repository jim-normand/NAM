/*
	This file is part of UCHIYAMARKER 2.0, a software for deformable random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#include "Combination.h"

void Combination::Init(const int n, const int m)
{
	int k[200];
	int nest = 0;
	int column = 1;

	Nest(nest, column, m, n-m, k, m_data);
}

void Combination::InitAll(const int n, const int m)
{
	int k[200];			// 200 is meaningless
	int nest = 0;
	int column = 1;

	allcombi nCm;

	Nest(nest, column, m, n-m, k, nCm);

	for(int i=0;i<static_cast<int>(nCm.size());i++){
		for(int j=0;j<m;j++){
			combi tmp(m);
			for(int k=0;k<m;k++){
				if(j+k<m){
					tmp[k] = nCm[i][j+k];
				}
				else{
					tmp[k] = nCm[i][j+k-m];
				}
			}
			m_data.push_back(tmp);
		}
	}
}

allcombi::iterator Combination::begin()
{
	return m_data.begin();
}

allcombi::iterator Combination::end()
{
	return m_data.end();
}

allcombi::const_iterator Combination::begin() const
{
	return m_data.begin();
}

allcombi::const_iterator Combination::end() const
{
	return m_data.end();
}


void Combination::Nest(int nest, int column, int n1, int n2, int k[], allcombi &result)
{
	for(int i=nest+1; i<=n2+column;i++){
		k[column] = i;

		if(n1 != column){
			Nest(i,column+1,n1,n2,k,result);
		}
		else{
			std::vector<int> tmp;
			for(int j=1;j<=n1;j++){
				tmp.push_back(k[j]-1);
			}
			result.push_back(tmp);
		}
	}
}