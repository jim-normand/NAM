/*
	This file is part of UCHIYAMARKERS, a software for random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#ifndef BLOBLIST_H
#define BLOBLIST_H


#include <map>
#include <vector>
#include <list>
#include <fstream>

#include "mylib/mylabel.h"
#include "blob.h"
#include "window.h"

#include "opencvpath.h"

typedef std::vector<blob> blobarray;
typedef std::list<blob*> eblobs;
typedef std::map<unsigned, eblobs> pblobs;
typedef std::multimap<int, pblobs::iterator> nblobs;

class BlobList
{
public:
   eblobs CloseBlobs(const double x, const double y);
	nblobs* SortbyPaper(const int th);
	void SetNeighbor(const int n);
	eblobs* GetExtracted();
	const eblobs* GetExtracted() const;	
	void CheckLength(const int num);
	blob* GetBlob(const int labelnum);
	void SetBlobs(const MyLabel &label, const int th = 3);
	void SetBlobs(const char *name);
	void Init(const int iw, const int ih, const int windowsize = 10);

   // Jim's modification
   void  SetBlobsWithSize(const char *name);
   void  SetWidth(int w);
   void  SetHeight(int h);
   int   GetWidth();
   int   GetHeight();
   void  SetSize(int w, int h);
   
   void  SetBlobs(const std::vector<CvPoint> dots);
   
private:
	nblobs m_bynum;
	pblobs m_bypaper;
	eblobs m_extracted;
	window m_windows;
	blobarray m_blobs;
   
   // Jim's modification
   int   m_width;
   int   m_height;
   // End of modification
};


#endif