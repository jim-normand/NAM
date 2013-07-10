/*
 This file is part of UCHIYAMARKER 2.0, a software for deformable random dot markers.
 Copyright (c) 2011 Hideaki Uchiyama
 
 You can use, copy, modify and re-distribute this software
 for non-profit purposes.
 */

#include "paperlist.h"

Paper* PaperList::Find(const unsigned paperid)
{
	paperlist::iterator found = m_papers.find(paperid);
   
	if(found == m_papers.end()){
		return NULL;
	}
	else{
		return &(*found).second;
	}
}

void PaperList::Delete(const unsigned paperid)
{
	paperlist::iterator found = m_papers.find(paperid);
   
	if(found != m_papers.end()){
		m_papers.erase(found); 
	}
}

unsigned PaperList::Add(eblobs *blobs)
{
	static int numpaper = -1;
	numpaper++;
   
	Paper tmp(816,576);
   
	tmp.SetID(numpaper);
   
	double r = static_cast<double>(rand())/(static_cast<double>(RAND_MAX)+0.1);
	double g = static_cast<double>(rand())/(static_cast<double>(RAND_MAX)+0.1);
	double b = static_cast<double>(rand())/(static_cast<double>(RAND_MAX)+0.1);
   
	tmp.SetColor(r,g,b);
   
	int num=0;
   
	for(eblobs::iterator iteb=blobs->begin();iteb!=blobs->end();++iteb){
      
		(*iteb)->SetID(numpaper, num);
      
		pt p;
		p.id = (*iteb)->id;
		p.descs = (*iteb)->descs;
		p.rawx = (*iteb)->rawx;
		p.rawy = (*iteb)->rawy;
		p.x = (*iteb)->x;
		p.y = (*iteb)->y;
      
		tmp.ComputeBarycenter(p);
		tmp.AddPt(p);
      
		++num;
	}
   
	m_papers.insert(std::pair<unsigned, Paper>((unsigned)numpaper, tmp));
   
	return (unsigned)numpaper;	
}


unsigned PaperList::Add(eblobs *blobs, int width, int height)
{
	static int numpaper = -1;
	numpaper++;
   
	//Paper tmp(816,576);
   Paper tmp(width, height);
   
	tmp.SetID(numpaper);
   
	double r = static_cast<double>(rand())/(static_cast<double>(RAND_MAX)+0.1);
	double g = static_cast<double>(rand())/(static_cast<double>(RAND_MAX)+0.1);
	double b = static_cast<double>(rand())/(static_cast<double>(RAND_MAX)+0.1);
   
	tmp.SetColor(r,g,b);
   
	int num=0;
   
	for(eblobs::iterator iteb=blobs->begin();iteb!=blobs->end();++iteb){
      
		(*iteb)->SetID(numpaper, num);
      
		pt p;
		p.id = (*iteb)->id;
		p.descs = (*iteb)->descs;
		p.rawx = (*iteb)->rawx;
		p.rawy = (*iteb)->rawy;
		p.x = (*iteb)->x;
		p.y = (*iteb)->y;
      
		tmp.ComputeBarycenter(p);
		tmp.AddPt(p);
      
		++num;
	}
   
	m_papers.insert(std::pair<unsigned, Paper>((unsigned)numpaper, tmp));
   
	return (unsigned)numpaper;	
}


void PaperList::Clear()
{
	m_papers.clear();
}



