/*
	This file is part of UCHIYAMARKERS, a software for random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#ifndef LLAH_H
#define LLAH_H

#include <vector>
#include <algorithm>
#include <iostream>

#include "mylib/myimage.h"
#include "mylib/mymat.h"
#include "mylib/mylabel.h"
#include "llahparam.h"
#include "bloblist.h"
#include "hashtable.h"
#include "paperlist.h"
#include "paper.h"

typedef std::list<Paper*> visible;

class LLAH
{
public:

	void SetHash(Paper* paper, const eblobs *blobs);
	void AddPaper(const char *name);

	visible* GetVisiblePaper();
	void UpdateTracking(Paper* paper, const double repro);
	bool FindPaper(const int min, const double repro=5.0);

	double ComputeAffine(const blob *target, const combi &nm, const combi &mk) const;
	unsigned ComputeIndex(const blob *target, const combi &nm) const;
	void ComputeDescriptors(blob *target);
	void RetrievebyTracking();
	void RetrievebyMatching();

	void CoordinateTransform(const double h);
	void SetPts();
	void Extract(const MyImage &src);
	void Init(const int iw, const int ih);

	void DrawBinary(MyImage &dst) const;
	void DrawPts(MyImage &dst) const;
   void DrawMatch(MyImage &dst, const int size);

   // Jim's modification
   void ExtractWithThreshold(const MyImage &src, int threshold);
   void AddPaperWithSize(const char *name);
   // End of modification
   
private:
	MyImage     m_gray;
	MyImage     m_binary;
   MyImage     m_smooth;
	MyLabel     m_label;
	LLAHParam   m_param;
	BlobList    m_bloblist;
	HashTable   m_table;
	HashTable   m_trackingtable;
	PaperList   m_paperlist;
	visible     m_visible;
};

#endif