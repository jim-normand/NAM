/*
	This file is part of UCHIYAMARKERS, a software for random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#include "llahparam.h"

const int numstep = 32 - 1;
const double measuredstep[numstep] = 
{
0.0336963	,
0.0819746	,
0.131507	,
0.176273	,
0.217802	,
0.255762	,
0.296017	,
0.33022	,
0.366761	,
0.413245	,
0.461068	,
0.496194	,
0.533282	,
0.588809	,
0.652065	,
0.720762	,
0.795556	,
0.873983	,
0.950395	,
1.01295	,
1.09403	,
1.2102	,
1.35679	,
1.53251	,
1.75568	,
2.02188	,
2.39235	,
3.00036	,
4.05752	,
6.21834	,
13.1458	,
};


LLAHParam::LLAHParam()
{
	n = 7;
	m = 5;
	k = 4;

	stepbit = 5;
	hashbit = 32;

	hashsize = static_cast<unsigned>(static_cast<int>(pow(2.0, hashbit))-1);

	nm.Init(n,m); mk.Init(m,k);
	rnm.InitAll(n,m);

	for(int i=0;i<numstep;i++){
		step.push_back(measuredstep[i]);
	}
}

unsigned LLAHParam::Quantization(double val) const
{
	for(unsigned i=0;i<numstep;i++){
		if(val < step[i]){
			return i;
		}
		else if(step[numstep-1-i] < val){
			return numstep-i;
		}
	}

	return 0xffffffff;
}



