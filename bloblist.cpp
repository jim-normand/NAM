/*
	This file is part of UCHIYAMARKERS, a software for random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#include "bloblist.h"

eblobs BlobList::CloseBlobs(const double x, const double y)
{
	int idx = static_cast<int>(x) / m_windows.size;
	int idy = static_cast<int>(y) / m_windows.size;
   
	eblobs blobs;
   
	
	int size=0;
   
	while(1){
      
		int num=0;
      
		for(int y=-size; y<=size; y++){
			for(int x=-size; x<=size; x++){
            
				xy idxy;
				idxy.x = idx-x;
				idxy.y = idx-y;
            
				if(0 <= idxy.x && idxy.x < m_windows.w && 0 <= idxy.y && idxy.y < m_windows.h){
					const bblobs *tmp = m_windows(idxy.x,idxy.y);
					num += static_cast<int>(tmp->size());
					for(bblobs::const_iterator itbl=(*tmp).begin();itbl!=(*tmp).end();++itbl){
						blobs.push_back((*itbl));
					}
				}
			}
		}
      
		if(num != 0){
			break;
		}
		else{
			++size;
		}
	}
   
	return blobs;
}

nblobs* BlobList::SortbyPaper(const int th)
{
	m_bypaper.clear();
	m_bynum.clear();

	for(eblobs::iterator ite=m_extracted.begin();ite!=m_extracted.end();++ite){
		if((*ite)->Valid()){
			unsigned paperid = (*ite)->PaperID();

			pblobs::iterator found = m_bypaper.find(paperid);

			if(found != m_bypaper.end()){
				(*found).second.push_back(*ite);
			}
			else{
				eblobs tmp;
				tmp.push_back(*ite);
				m_bypaper.insert(std::pair<unsigned, eblobs>(paperid, tmp));
			}
		}
	}

	for(pblobs::iterator itby=m_bypaper.begin();itby!=m_bypaper.end();++itby){
		int num = static_cast<int>((*itby).second.size());

		if(num > th){
			m_bynum.insert(std::pair<int, pblobs::iterator>(num, itby));
		}
	}

	return &m_bynum;
}

void BlobList::SetNeighbor(const int n)
{
	for(eblobs::iterator ite=m_extracted.begin();ite!=m_extracted.end();++ite){
	
		int size=1;
		std::vector<xy> buckets;

		while(1){

			int candidates = 0;
			for(int y=-size; y<=size; y++){
				for(int x=-size; x<=size; x++){

					xy idxy;
					idxy.x = (*ite)->idxy.x-x;
					idxy.y = (*ite)->idxy.y-y;

					if(0 <= idxy.x && idxy.x < m_windows.w && 0 <= idxy.y && idxy.y < m_windows.h){
						candidates += static_cast<int>(m_windows(idxy.x,idxy.y)->size());
						buckets.push_back(idxy);
					}
				}
			}

			if(n < candidates){
				break;
			}

			size++;
			buckets.clear();
		}

		typedef std::multimap<double, blob*> evaluation;
		evaluation dis;
		for(std::vector<xy>::iterator itbu=buckets.begin();itbu!=buckets.end();++itbu){
			const bblobs* blobs = m_windows((*itbu).x, (*itbu).y);

			for(bblobs::const_iterator itbl=(*blobs).begin();itbl!=(*blobs).end();++itbl){

				double tmp = (*ite)->Distance2((*itbl));

				if(static_cast<int>(dis.size()) < n+1){
					dis.insert(std::pair<double,blob*>(tmp, *itbl));
				}
				else{
					evaluation::iterator idis = dis.end();
					idis--;
					if(tmp < (*idis).first){
						dis.erase(idis);
						dis.insert(std::pair<double, blob*>(tmp, *itbl));
					}
				}
			}
		}

		dis.erase(dis.begin());

		evaluation ang;

		for(evaluation::iterator itdi=dis.begin();itdi!=dis.end();++itdi){
			ang.insert(std::pair<double, blob*>((*ite)->Angle((*itdi).second), (*itdi).second));
		}

		for(evaluation::iterator itan=ang.begin();itan!=ang.end();++itan){
			(*ite)->nblobs.push_back((*itan).second);
		}
	}
}

eblobs* BlobList::GetExtracted()
{	
	return &m_extracted;
}

const eblobs* BlobList::GetExtracted() const
{	
	return &m_extracted;
}

void BlobList::CheckLength(const int num)
{
	if(num > static_cast<int>(m_blobs.size())){
		m_blobs.resize(num+num/2);
	}

	for(int i=0;i<num;i++){
		m_blobs[i].Clear();
	}

	m_windows.Clear();
	m_extracted.clear();
}

blob* BlobList::GetBlob(const int labelnum)
{
	if(labelnum == 0){
		return NULL;
	}

	return &m_blobs[labelnum-1];
}

void BlobList::SetBlobs(const MyLabel &label, const int th)
{
	CheckLength(label.num);
   
   // Jim's modification
   m_width  = label.w;
   m_height = label.h;
   // End of modification

	for(int y=1;y<label.h-1;y++){
		for(int x=1;x<label.w-1;x++){
			int labelnum = label(x,y);
			if(labelnum != 0){
				m_blobs[labelnum-1].sumx += x;
				m_blobs[labelnum-1].sumy += y;
				m_blobs[labelnum-1].sum++;
			}
		}
	}

	for(int i=0;i<label.num;i++){
		if(m_blobs[i].sum > th){

			m_blobs[i].rawx = static_cast<float>(m_blobs[i].sumx) / static_cast<float>(m_blobs[i].sum);
			m_blobs[i].rawy = static_cast<float>(m_blobs[i].sumy) / static_cast<float>(m_blobs[i].sum);

			m_blobs[i].idxy.x = static_cast<int>(m_blobs[i].rawx) / m_windows.size;
			m_blobs[i].idxy.y = static_cast<int>(m_blobs[i].rawy) / m_windows.size;

			m_windows.Add(m_blobs[i].idxy.x, m_blobs[i].idxy.y, &m_blobs[i]);

			m_extracted.push_back(&m_blobs[i]);
		}
	}
}

void BlobList::SetBlobs(const char *name)
{
	std::ifstream in(name);
	
	int num;
	in >> num;   
   
	CheckLength(num);
   
	for(int i=0;i<num;i++){

		in >> m_blobs[i].x >> m_blobs[i].y;

		m_blobs[i].idxy.x = 0;
		m_blobs[i].idxy.y = 0;

		m_windows.Add(m_blobs[i].idxy.x, m_blobs[i].idxy.y, &m_blobs[i]);

		m_extracted.push_back(&m_blobs[i]);

	}

	in.close();
}

// Jim's modification
void BlobList::SetBlobsWithSize(const char *name)
{
	std::ifstream in(name);
	
	int num;
	in >> num;   
   
	CheckLength(num);
   
   
   // Beware: the files have to be modified to include the size of the marker
   // at the beginning of the text file containing the dots' coordinates
   
   // Jim's modification
   in >> m_width;
   in >> m_height;
   // End of modification
   
   
	for(int i=0;i<num;i++){
      
		in >> m_blobs[i].x >> m_blobs[i].y;
      
      // Jim's modification
      m_blobs[i].y = m_height - m_blobs[i].y;
      // End of modification
      
		m_blobs[i].idxy.x = 0;
		m_blobs[i].idxy.y = 0;
      
		m_windows.Add(m_blobs[i].idxy.x, m_blobs[i].idxy.y, &m_blobs[i]);
      
		m_extracted.push_back(&m_blobs[i]);
      
	}
   
	in.close();
}
// End of modification

// Jim's modification
void BlobList::SetBlobs(const std::vector<CvPoint> dots)
{
   // Size should be set prior to this call
   
   // Checking length
   int num = (int)dots.size();
   CheckLength(num);
   
   for(int i=0;i<num;i++) {
      
      m_blobs[i].x = dots[i].x;
      m_blobs[i].y = dots[i].y;
      
      m_blobs[i].idxy.x = 0;
      m_blobs[i].idxy.y = 0;
      
      m_windows.Add(m_blobs[i].idxy.x, m_blobs[i].idxy.y, &m_blobs[i]);
      
		m_extracted.push_back(&m_blobs[i]);
   }
}


void BlobList::Init(const int iw, const int ih, const int windowsize)
{
	m_windows.Init(iw/windowsize+1, ih/windowsize+1, windowsize);
}

// Jim's modification
void BlobList::SetWidth(int w)
{
   m_width = w;
}

void BlobList::SetHeight(int h)
{
   m_height = h;
}

int BlobList::GetWidth()
{
   return m_width;
}

int BlobList::GetHeight()
{
   return m_height;
}

void BlobList::SetSize(int w, int h)
{
   m_width  = w;
   m_height = h;
}

// End of modification