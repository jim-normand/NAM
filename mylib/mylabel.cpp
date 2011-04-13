/*
	This file is part of UCHIYAMARKERS, a software for random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#include "MyLabel.h"

void MyLabel::Labeling(const char *binary)
{
	int nsort[5];
	int map[30000];	

	map[0] = 0;

	for(int y=1;y<h-1;y++){
		for(int x=1;x<w-1;x++){
			int pos = x+w*y;
			m_label[pos] = static_cast<unsigned char>(binary[pos]);
		}
	}

	for(int x=0;x<w;x++){
		m_label[x] = m_label[x+w*(h-1)] = 0;
	}
	for(int y=0;y<h;y++){
		m_label[w*y] = m_label[w-1+w*y] = 0;
	}

	int label = 0;
	for(int y=1;y<h-1;y++){
		for(int x=1;x<w-1;x++){

			map[0] = 0;

			int pos = x+w*y;
			if(m_label[pos] == 0){
				continue;
			}

			nsort[0] = 0;
			nsort[1] = m_label[pos-1-w];
			nsort[2] = m_label[pos-w];
			nsort[3] = m_label[pos+1-w];
			nsort[4] = m_label[pos-1];

			int flag, countk, i;
			for(flag = 1; flag == 1;){
				for(flag = 0, i = 2; i < 5; i++){
					if (nsort[i] < nsort[i-1]){
						countk = nsort[i];
						nsort[i] = nsort[i-1];
						nsort[i-1] = countk;
						flag = 1;
					}
				}
			}
			countk = 0;
			
			for(i = 1; i < 5; i++){
				if(nsort[i] == 0){
					continue;
				}
				if(nsort[i] == nsort[i-1]){
					continue;
				}
				countk++;
				nsort[countk] = nsort[i];
			}

			i = countk;
			if(i == 0){
				label++;
				m_label[pos] = label;
				map[label] = label;
			}
			else if(i == 1){
				countk = nsort[1];
				while(map[countk] != countk){
					countk = map[countk];
				}
				m_label[pos]=countk;
			}
			else if(i > 1){
				countk = nsort[1];
				while(map[countk] != countk){
					countk = map[countk];
				}
				m_label[pos] = countk;
				for (int p = 2; p <= i; p++){
					map[nsort[p]] = countk;
				}
			}		
		}
	}

	num=0;

	for(int labelnumber=1;labelnumber<=label;labelnumber++){
		if(map[labelnumber] == labelnumber){
			num++;                       
			map[labelnumber] = num;
		}
		else{
			map[labelnumber] = map[map[labelnumber]];
		}
	}

	num++;

	for(int x=1;x<w-1;x++){
		for(int y=1;y<h-1;y++){
			int pos = x+w*y;
			if(m_label[pos] != 0){
				m_label[pos] = map[m_label[pos]];
			}
		}
	}
}

void MyLabel::Init(const int iw, const int ih)
{
	w=iw, h=ih;
	m_label.resize(w*h);
	memset(&m_label[0],0,sizeof(int)*w*h);

}

int& MyLabel::operator() (const int x, const int y)
{
	return m_label[x+w*y]; 
}

const int& MyLabel::operator() (const int x, const int y) const 
{
	return m_label[x+w*y]; 
}



