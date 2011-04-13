/*
	This file is part of UCHIYAMARKERS, a software for random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#include "mytimer.h"

static std::vector<task> m_tasks;

void MyTimer::Pop()
{
#ifdef WIN32	
	unsigned long diff = timeGetTime() - m_tasks.back().stime;
	std::cout << m_tasks.back().name.c_str() << ": " << static_cast<int>(diff) << " ms" << std::endl;
#else
	clock_t diff = clock() - m_tasks.back().stime;
	std::cout << m_tasks.back().name.c_str() << ": " << 1000*diff/CLOCKS_PER_SEC << " ms" << std::endl;
#endif
}

void MyTimer::Push(const char* name)
{
	task tmp;
	tmp.name = name;

#ifdef WIN32
	tmp.stime = timeGetTime();
#else
	tmp.stime = clock();
#endif
	m_tasks.push_back(tmp);
}