/*
	This file is part of UCHIYAMARKERS, a software for random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#ifndef MYTIMER_H
#define MYTIMER_H

#include <string>
#include <vector>
#include <iostream>

#ifdef WIN32
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#else
#include <time.h>
#endif

struct task{
	std::string name;

#ifdef WIN32
	unsigned long stime;
#else
	clock_t stime;
#endif
};

class MyTimer
{
public:
	static void Pop();
	static void Push(const char* name);
};

#endif