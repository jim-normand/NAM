/*
	This file is part of UCHIYAMARKERS, a software for random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#ifndef PAPERLIST_H
#define PAPERLIST_H

#include <cstdlib> 
#include <map>

#include "mylib/myimage.h"
#include "paper.h"
#include "bloblist.h"

typedef std::map<unsigned, Paper> paperlist;

class PaperList
{
public:
	Paper* FindPaper(const int paperid);
	void Delete(const int paperid);
	unsigned Add(eblobs *blobs);
	void Clear();

private:
	paperlist m_papers;
};

#endif