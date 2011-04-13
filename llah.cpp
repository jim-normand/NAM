/*
	This file is part of UCHIYAMARKERS, a software for random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#include "LLAH.h"

void LLAH::SetHash(Paper* paper, const eblobs *blobs)
{
	for(eblobs::const_iterator itbl=blobs->begin();itbl!=blobs->end();++itbl){
		for(descriptors::iterator itde=(*itbl)->descs.begin();itde!=(*itbl)->descs.end();++itde){
			bool flag = m_table.Set(static_cast<unsigned>(*itde), (*itbl)->id);

			if(flag){
				paper->pts[(*itbl)->PtID()].descs.insert(static_cast<unsigned>(*itde));
			}
		}
	}
}

void LLAH::AddPaper(const char *name)
{
	m_bloblist.SetBlobs(name);

	eblobs *blobs = m_bloblist.GetExtracted();

	if(static_cast<int>(blobs->size()) > m_param.n){
		m_bloblist.SetNeighbor(m_param.n);

		for(eblobs::iterator itbl=(*blobs).begin();itbl!=(*blobs).end();++itbl){

			for(allcombi::iterator itnm=m_param.rnm.begin();itnm!=m_param.rnm.end();++itnm){
				unsigned index = ComputeIndex(*itbl, *itnm);
				(*itbl)->descs.insert(index);
			}

			ComputeDescriptors(*itbl);
			m_table.ComputeID(*itbl);
		}
	}

	unsigned paperid = m_paperlist.Add(blobs);
	Paper* paper = m_paperlist.FindPaper(paperid);

	SetHash(paper, blobs);
}


visible* LLAH::GetVisiblePaper()
{
	return &m_visible;
}

void LLAH::UpdateTracking(Paper *paper, const double repro)
{
	int allpts = static_cast<int>(paper->pts.size());

	MyMat allimpts(3, allpts), alldbpts(3, allpts);

	int i=0;
	for(ptlist::iterator itpt=paper->pts.begin();itpt!=paper->pts.end();++itpt){
		alldbpts(0,i) = (*itpt).x;
		alldbpts(1,i) = (*itpt).y;
		alldbpts(2,i) = 1.0;
		++i;
	}			

	allimpts.Mul(paper->H, alldbpts);

	i=0;

	for(ptlist::iterator itpt=paper->pts.begin();itpt!=paper->pts.end();++itpt){

		double x = allimpts(0,i)/allimpts(2,i);
		double y = allimpts(1,i)/allimpts(2,i);
		int lx = static_cast<int>(x);
		int ly = m_label.h-static_cast<int>(y);

		if(0 <= lx && lx < m_label.w && 0 <= ly && ly < m_label.h){

			int labelnum = m_label(lx,ly);

			blob* bb = m_bloblist.GetBlob(labelnum);

			if(bb != NULL && bb->Distance(x,y) < repro){

				for(descriptors::iterator itde= (*bb).descs.begin();itde!=(*bb).descs.end();++itde){
					bool flag = m_trackingtable.Set(static_cast<unsigned>(*itde), (*itpt).id);
				}

				(*bb).found = true;
			}
		}
		++i;
	}
}

bool LLAH::FindPaper(const int min, const double repro)
{
	bool flag = false;
	
	nblobs *bloblist = m_bloblist.SortbyPaper(min);

	for(nblobs::iterator itbl=bloblist->begin();itbl!=bloblist->end();++itbl){

		unsigned paperid = (*(*itbl).second).first;

		Paper* paper = m_paperlist.FindPaper(paperid);

		if(paper != NULL && find(m_visible.begin(), m_visible.end(), paper) == m_visible.end()){

			eblobs *blobs = &(*(*itbl).second).second;

			int num = static_cast<int>(blobs->size());

			MyMat impts(3,num), dbpts(3,num);

			int i = 0;
			for(eblobs::iterator itbs=blobs->begin();itbs!=blobs->end();++itbs){

				impts(0,i) = (*itbs)->x;
				impts(1,i) = (*itbs)->y;
				impts(2,i) = 1.0;

				unsigned ptid = (*itbs)->PtID();

				dbpts(0,i) = paper->pts[ptid].x;
				dbpts(1,i) = paper->pts[ptid].y;
				dbpts(2,i) = 1.0;
				
				++i;
			}

			paper->H.Homography(dbpts, impts, repro);

			impts.Mul(paper->H, dbpts);

			i=0;
			int count = 0;
			for(eblobs::iterator itbs=blobs->begin();itbs!=blobs->end();++itbs){
				double x = impts(0,i)/impts(2,i);
				double y = impts(1,i)/impts(2,i);

				double distance = (*itbs)->Distance(x, y);
				if(distance < repro){
					count++;
				}

				++i;
			}

			if(count > min){
				flag = true;
				m_visible.push_back(paper);
				UpdateTracking(paper,repro);
			}
		}
	}
	
	return flag;
}

double LLAH::ComputeAffine(const blob *target, const combi &nm, const combi &mk) const
{
	blob* p0 = target->nblobs[nm[mk[0]]];
	blob* p1 = target->nblobs[nm[mk[1]]];
	blob* p2 = target->nblobs[nm[mk[2]]];
	blob* p3 = target->nblobs[nm[mk[3]]];

	return p0->Triangle(p1,p2)/(p0->Triangle(p2,p3)+0.00000001);
}

unsigned LLAH::ComputeIndex(const blob *target, const combi &nm) const
{
	unsigned index=0;

	for(allcombi::const_iterator itmk=m_param.mk.begin();itmk!=m_param.mk.end();++itmk){
		index = index << m_param.stepbit;
		double val = ComputeAffine(target, nm, *itmk);
		index += m_param.Quantization(val);

		if(index > m_param.hashsize){
			index = index % m_param.hashsize;
		}
	}

	return index % m_param.hashsize;
}

void LLAH::ComputeDescriptors(blob *target) 
{
	for(allcombi::iterator itnm=m_param.nm.begin();itnm!=m_param.nm.end();++itnm){
		unsigned index = ComputeIndex(target, *itnm);
		target->descs.insert(index);
	}
}

void LLAH::RetrievebyTracking()
{
	m_visible.clear();

	eblobs *blobs = m_bloblist.GetExtracted();

	if(static_cast<int>(blobs->size()) > m_param.n){
		m_bloblist.SetNeighbor(m_param.n);

		for(eblobs::iterator itbl=(*blobs).begin();itbl!=(*blobs).end();++itbl){
			ComputeDescriptors(*itbl);
			(*itbl)->id = m_trackingtable.ComputeID(*itbl);
		}
	}

	m_trackingtable.Clear();
}

void LLAH::RetrievebyMatching()
{
	eblobs *blobs = m_bloblist.GetExtracted();

	if(static_cast<int>(blobs->size()) > m_param.n){
		for(eblobs::iterator itbl=(*blobs).begin();itbl!=(*blobs).end();++itbl){
			if((*itbl)->found){
				(*itbl)->id = NOID;
			}
			else{
				ComputeDescriptors(*itbl);
				(*itbl)->id = m_table.ComputeID(*itbl);
			}
		}
	}
}

void LLAH::CoordinateTransform(const double h)
{
	eblobs* blobs = m_bloblist.GetExtracted();

	for(eblobs::iterator itb=(*blobs).begin();itb!=(*blobs).end();++itb){

		(*itb)->x = (*itb)->rawx;
		(*itb)->y = h - (*itb)->rawy;
	}
}

void LLAH::SetPts()
{
	m_label.Labeling(m_binary);
	m_bloblist.SetBlobs(m_label);
}

void LLAH::Extract(const MyImage &src, int threshold)
{
	m_gray.ColortoGray(src);
	m_binary.Binarization(m_gray, threshold);
}

void LLAH::Init(const int iw, const int ih)
{
	m_gray.Init(iw,ih,1);
	m_binary.Init(iw,ih,1);
	m_label.Init(iw,ih);
	m_bloblist.Init(iw,ih,10);
}

void LLAH::DrawBinary(MyImage &dst) const
{
	MyImage tmp;
	tmp.Init(m_binary.w, m_binary.h, 3);

	tmp.GraytoColor(m_binary);
	dst.Resize(tmp);

}

void LLAH::DrawPts(MyImage &dst) const
{
	const eblobs *blobs = m_bloblist.GetExtracted();

	for(eblobs::const_iterator itbl=(*blobs).begin();itbl!=(*blobs).end();++itbl){
		if((*itbl)->found){
			dst.Circle(static_cast<int>((*itbl)->rawx), static_cast<int>((*itbl)->rawy), 3, -1, 0,255);
		}
		else{
			dst.Circle(static_cast<int>((*itbl)->rawx), static_cast<int>((*itbl)->rawy), 3, -1, 255);
		}
	}
}