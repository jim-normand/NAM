/*
	This file is part of UCHIYAMARKER 2.0, a software for deformable random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#include "mesh.h"

mesh Mesh::GetTrianges()
{
	mesh tmp;
	tmp.tri = &m_triangles;
	tmp.ver = &m_vertice;
	tmp.initver = &m_initvertice;
	return tmp;
}

bool Mesh::Recover(const eblobs &blobs, const ptlist &pts, const int min)
{
	int count = 0;
	for(double sigma=3.0;sigma>2.0;sigma/=2.0){

		double sigman = pow(sigma, 4);
		double sigma2 = sigma*sigma;

		bx.Clean();
		by.Clean();
		A.Clean();

		count = 0;
		for(eblobs::const_iterator itbs=blobs.begin();itbs!=blobs.end();++itbs){

			unsigned ptid = (*itbs)->PtID();

			if(pts[ptid].inside){

				double tmpx = X(pts[ptid].centerid[0],0)*pts[ptid].centerval[0]+X(pts[ptid].centerid[1],0)*pts[ptid].centerval[1]+X(pts[ptid].centerid[2],0)*pts[ptid].centerval[2];
				double tmpy = Y(pts[ptid].centerid[0],0)*pts[ptid].centerval[0]+Y(pts[ptid].centerid[1],0)*pts[ptid].centerval[1]+Y(pts[ptid].centerid[2],0)*pts[ptid].centerval[2];

				double residual = (tmpx-(*itbs)->x)*(tmpx-(*itbs)->x)+(tmpy-(*itbs)->y)*(tmpy-(*itbs)->y);
				
				if(residual < sigma2){
					for(int i=0;i<3;++i){

						bx(pts[ptid].centerid[i], 0) += (*itbs)->x*pts[ptid].centerval[i]/sigman;
						by(pts[ptid].centerid[i], 0) += (*itbs)->y*pts[ptid].centerval[i]/sigman;
						for(int j=0;j<3;++j){
							A(pts[ptid].centerid[i],pts[ptid].centerid[j]) += pts[ptid].centerval[i]*pts[ptid].centerval[j]/sigman;
						}
					}
					++count;
				}
			}
		}

		KA.Add(K,A);
		invKA.Inv(KA);

		X.Mul(invKA, bx);
		Y.Mul(invKA, by);
	}

	if(count > min){
		for(int i=0;i<m_num;++i){
			m_vertice[i].x = X(i,0);
			m_vertice[i].y = Y(i,0);
		}
		return true;
	}
	else{
		return false;
	}
}

void Mesh::Init(const MyMat &H)
{
	MyMat src(3,1), dst(3,1);

	for(int i=0;i<m_num;++i){
		src(0,0) = initX(i,0);
		src(1,0) = initY(i,0);
		src(2,0) = 1.0;

		dst.Mul(H,src);

		m_vertice[i].x = X(i,0) = dst(0,0)/dst(2,0);
		m_vertice[i].y = Y(i,0) = dst(1,0)/dst(2,0);
	}
}

bool Mesh::GetBarycenter(const double x, const double y, int &id1, double &val1, int &id2, double &val2, int &id3, double &val3)
{
	double th = m_size*sin(PI/180.0*60.0);

	int idy = static_cast<int>(y/th);

	if(idy < m_numh-1){
		int vertexpos = idy*m_numw;

		if(idy%2==0){

			int idx1 = static_cast<int>(x/m_size);

			id1 = idx1+vertexpos;
			id2 = idx1+1+vertexpos;
			id3 = idx1+m_numw+vertexpos;

			if(idx1 < m_numw-1 && Inside(x,y,id1,val1,id2,val2,id3,val3)){
				return true;
			}

			int idx2 = static_cast<int>((x+m_size/2)/m_size);

			id1 = idx2+vertexpos+m_numw;
			id2 = idx2+1+vertexpos+m_numw;
			id3 = idx2;

			if(idx2 < m_numw-1 && Inside(x,y,id1,val1,id2,val2,id3,val3)){
				return true;
			}
		}
		else{

			int idx1 = static_cast<int>((x+m_size/2)/m_size);

			id1 = idx1+vertexpos;
			id2 = idx1+1+vertexpos;
			id3 = idx1+vertexpos+m_numw;

			if(idx1 < m_numw-1 && Inside(x,y,id1,val1,id2,val2,id3,val3)){
				return true;
			}

			int idx2 = static_cast<int>(x/m_size);

			id1 = idx2+vertexpos+m_numw;
			id2 = idx2+1+vertexpos+m_numw;
			id3 = idx2+1+vertexpos;

			if(idx2 < m_numw-1 && Inside(x,y,id1,val1,id2,val2,id3,val3)){
				return true;
			}
		}
	}

	return false;
}

void Mesh::Base(const double markerwidth, const double markerheight, const double trianglesize)
{
	double th = trianglesize*sin(PI/180.0*60.0);

	m_numw = static_cast<int>((markerwidth-trianglesize)/trianglesize)+1+1;
	m_numh = static_cast<int>(markerheight/th)+1;
	m_size = trianglesize;

	m_num = m_numw * m_numh;

	Make();
	SetK();
}

bool Mesh::Inside(const double x, const double y, const int id1, double &val1, const int id2, double &val2, const int id3, double &val3)
{
	MyMat x1(2,1), x2(2,1), x3(2,1);

	x1(0,0) = X(id1,0)-x;
	x1(1,0) = Y(id1,0)-y;
	x2(0,0) = X(id2,0)-x;
	x2(1,0) = Y(id2,0)-y;
	x3(0,0) = X(id3,0)-x;
	x3(1,0) = Y(id3,0)-y;

	double ang = x1.Angle(x2) + x2.Angle(x3) + x3.Angle(x1);

	if(fabs(PI2-ang) > 0.0001){
		return false;
	}

	val1 = ((Y(id2,0)-Y(id3,0))*(x-X(id3,0))+(X(id3,0)-X(id2,0))*(y-Y(id3,0)))/((Y(id2,0)-Y(id3,0))*(X(id1,0)-X(id3,0))+(X(id3,0)-X(id2,0))*(Y(id1,0)-Y(id3,0)));
	val2 = ((Y(id3,0)-Y(id1,0))*(x-X(id3,0))+(X(id1,0)-X(id3,0))*(y-Y(id3,0)))/((Y(id3,0)-Y(id1,0))*(X(id2,0)-X(id3,0))+(X(id1,0)-X(id3,0))*(Y(id2,0)-Y(id3,0)));
	val3 = 1.0-val1-val2;

	return true;
}

void Mesh::SetK()
{
	int triplet = 3;
	int row = triplet*(m_numw-3)+1+1+2;	// two 1s for both ends and 2 for one before or after end
	int numborder = (m_numw-2)*2;
	int numall = row*(m_numh-2)+numborder;

	MyMat Kd(numall, m_num), KdT(m_num, numall);

	int arraypos = 0;
	int vertexpos;

	// set triplet
	for(int i=1;i<m_numh-1;++i){

		if(i%2==0){
			for(int j=1;j<m_numw-2;++j){
				vertexpos = j+i*m_numw;
				Kd(arraypos, vertexpos-1) = 1.0;
				Kd(arraypos, vertexpos) = -2.0;
				Kd(arraypos, vertexpos+1) = 1.0;
				++arraypos;

				Kd(arraypos, vertexpos-m_numw) = 1.0;
				Kd(arraypos, vertexpos) = -2.0;
				Kd(arraypos, vertexpos+m_numw+1) = 1.0;
				++arraypos;

				Kd(arraypos, vertexpos+m_numw) = 1.0;
				Kd(arraypos, vertexpos) = -2.0;
				Kd(arraypos, vertexpos-m_numw+1) = 1.0;
				++arraypos;
			}

			vertexpos = 0+i*m_numw;
			Kd(arraypos, vertexpos-m_numw) = 1.0;
			Kd(arraypos, vertexpos) = -2.0;
			Kd(arraypos, vertexpos+m_numw) = 1.0;
			++arraypos;

			vertexpos = m_numw-1+i*m_numw;
			Kd(arraypos, vertexpos-m_numw) = 1.0;
			Kd(arraypos, vertexpos) = -2.0;
			Kd(arraypos, vertexpos+m_numw) = 1.0;
			++arraypos;

			vertexpos = m_numw-2+i*m_numw;
			Kd(arraypos, vertexpos-m_numw) = 1.0;
			Kd(arraypos, vertexpos) = -2.0;
			Kd(arraypos, vertexpos+m_numw+1) = 1.0;
			++arraypos;

			Kd(arraypos, vertexpos+m_numw) = 1.0;
			Kd(arraypos, vertexpos) = -2.0;
			Kd(arraypos, vertexpos-m_numw+1) = 1.0;
			++arraypos;
		}
		else{
			for(int j=2;j<m_numw-1;++j){
				vertexpos = j+i*m_numw;
				Kd(arraypos, vertexpos-1) = 1.0;
				Kd(arraypos, vertexpos) = -2.0;
				Kd(arraypos, vertexpos+1) = 1.0;
				++arraypos;

				Kd(arraypos, vertexpos-m_numw-1) = 1.0;
				Kd(arraypos, vertexpos) = -2.0;
				Kd(arraypos, vertexpos+m_numw) = 1.0;
				++arraypos;

				Kd(arraypos, vertexpos+m_numw-1) = 1.0;
				Kd(arraypos, vertexpos) = -2.0;
				Kd(arraypos, vertexpos-m_numw) = 1.0;
				++arraypos;
			}

			vertexpos = 0+i*m_numw;
			Kd(arraypos, vertexpos-m_numw) = 1.0;
			Kd(arraypos, vertexpos) = -2.0;
			Kd(arraypos, vertexpos+m_numw) = 1.0;
			++arraypos;

			vertexpos = m_numw-1+i*m_numw;
			Kd(arraypos, vertexpos-m_numw) = 1.0;
			Kd(arraypos, vertexpos) = -2.0;
			Kd(arraypos, vertexpos+m_numw) = 1.0;
			++arraypos;

			vertexpos = 1+i*m_numw;
			Kd(arraypos, vertexpos-m_numw-1) = 1.0;
			Kd(arraypos, vertexpos) = -2.0;
			Kd(arraypos, vertexpos+m_numw) = 1.0;
			++arraypos;

			Kd(arraypos, vertexpos+m_numw-1) = 1.0;
			Kd(arraypos, vertexpos) = -2.0;
			Kd(arraypos, vertexpos-m_numw) = 1.0;
			++arraypos;
		}
	}

	// set border
	int lastrow = (m_numh-1)*m_numw;

	if(m_numh%2==0){
		for(int i=1;i<m_numw-2;++i){
			Kd(arraypos, i-1) = 1.0;
			Kd(arraypos, i+0) = -2.0;
			Kd(arraypos, i+1) = 1.0;
			++arraypos;

			Kd(arraypos, i+0+lastrow) = 1.0;
			Kd(arraypos, i+1+lastrow) = -2.0;
			Kd(arraypos, i+2+lastrow) = 1.0;
			++arraypos;
		}
	}
	else{
		for(int i=1;i<m_numw-2;++i){
			Kd(arraypos, i-1) = 1.0;
			Kd(arraypos, i+0) = -2.0;
			Kd(arraypos, i+1) = 1.0;
			++arraypos;

			Kd(arraypos, i-1+lastrow) = 1.0;
			Kd(arraypos, i+0+lastrow) = -2.0;
			Kd(arraypos, i+1+lastrow) = 1.0;
			++arraypos;
		}
	}

	KdT.Transpose(Kd);
	K.Mul(KdT, Kd);

	K.Scale(0.0003);	// *lambda
}

/*
void Mesh::SetK()
{
	int triplet = 3;
	int numtriplet = (m_numw-2)*triplet*(m_numh-2);
	int numborder = (m_numw-2)+(m_numh-2);
	int numarrow = (m_numh-2)*2;
	int numall = numtriplet+numborder+numarrow;

	MyMat Kd(numall, m_num), KdT(m_num, numall);

	// set triplet
	for(int i=1;i<m_numh-1;++i){
		for(int j=1;j<m_numw-1;++j){

			int arraypos = ((j-1)+(i-1)*(m_numw-2))*triplet;
			int vertexpos = j+i*m_numw;

			if(i%2==1){	
				Kd(0+arraypos, vertexpos-1) = 1.0;
				Kd(0+arraypos, vertexpos) = -2.0;
				Kd(0+arraypos, vertexpos+1) = 1.0;

				Kd(1+arraypos, vertexpos-m_numw) = 1.0;
				Kd(1+arraypos, vertexpos) = -2.0;
				Kd(1+arraypos, vertexpos+m_numw+1) = 1.0;

				Kd(2+arraypos, vertexpos+m_numw) = 1.0;
				Kd(2+arraypos, vertexpos) = -2.0;
				Kd(2+arraypos, vertexpos-m_numw+1) = 1.0;
			}
			else{
				Kd(0+arraypos, vertexpos-1) = 1.0;
				Kd(0+arraypos, vertexpos) = -2.0;
				Kd(0+arraypos, vertexpos+1) = 1.0;

				Kd(1+arraypos, vertexpos-m_numw-1) = 1.0;
				Kd(1+arraypos, vertexpos) = -2.0;
				Kd(1+arraypos, vertexpos+m_numw) = 1.0;

				Kd(2+arraypos, vertexpos+m_numw-1) = 1.0;
				Kd(2+arraypos, vertexpos) = -2.0;
				Kd(2+arraypos, vertexpos-m_numw) = 1.0;
			}
		}
	}

	// set border
	int lastrow = (m_numh-1)*m_numw;
	for(int i=1;i<m_numw-1;++i){

		int arraypos = numtriplet+2*(i-1);

		Kd(0+arraypos, i-1) = 1.0;
		Kd(0+arraypos, i+0) = -2.0;
		Kd(0+arraypos, i+1) = 1.0;

		Kd(1+arraypos, i-1+lastrow) = 1.0;
		Kd(1+arraypos, i+0+lastrow) = -2.0;
		Kd(1+arraypos, i+1+lastrow) = 1.0;
	}

	// set arrow
	for(int i=1;i<m_numh-1;++i){
		int arraypos = numtriplet+numborder+2*(i-1);

		if(i%2==1){
			int vertexpos = i*m_numw;

			Kd(0+arraypos, vertexpos-m_numw) = 1.0;
			Kd(0+arraypos, vertexpos) = -2.0;
			Kd(0+arraypos, vertexpos+m_numw+1) = 1.0;

			Kd(1+arraypos, vertexpos-m_numw+1) = 1.0;
			Kd(1+arraypos, vertexpos) = -2.0;
			Kd(1+arraypos, vertexpos+m_numw) = 1.0;
		}
		else{
			int vertexpos = i*m_numw+m_numw-1;

			Kd(0+arraypos, vertexpos-m_numw-1) = 1.0;
			Kd(0+arraypos, vertexpos) = -2.0;
			Kd(0+arraypos, vertexpos+m_numw) = 1.0;

			Kd(1+arraypos, vertexpos-m_numw) = 1.0;
			Kd(1+arraypos, vertexpos) = -2.0;
			Kd(1+arraypos, vertexpos+m_numw-1) = 1.0;
		}
	}

	KdT.Transpose(Kd);
	K.Mul(KdT, Kd);

	K.Scale(0.0003);	// *lambda
}
*/

void Mesh::Make()
{
	K.Init(m_num, m_num);
	A.Init(m_num, m_num);
	bx.Init(m_num, 1);
	by.Init(m_num, 1);
	KA.Init(m_num, m_num);		// lambda*K+A
	invKA.Init(m_num, m_num);
	X.Init(m_num, 1);
	Y.Init(m_num, 1);
	initX.Init(m_num, 1);
	initY.Init(m_num, 1);

	m_vertice.resize(m_num);
	m_initvertice.resize(m_num);

	double th = m_size*sin(PI/180.0*60.0);

	// make vertice
	for(int i=0;i<m_numh;++i){
		if(i%2 == 0){
			for(int j=0;j<m_numw-1;++j){
				int pos = j+i*m_numw;
				m_initvertice[pos].x = initX(pos,0) = X(pos,0) = j*m_size;
				m_initvertice[pos].y = initY(pos,0) = Y(pos,0) = i*th;
			}

			int pos = m_numw-1+i*m_numw;
			m_initvertice[pos].x = initX(pos,0) = X(pos,0) = (m_numw-2)*m_size+m_size/2;
			m_initvertice[pos].y = initY(pos,0) = Y(pos,0) = i*th;
		}
		else{
			int pos = 0+i*m_numw;
			m_initvertice[pos].x = initX(pos,0) = X(pos,0) = 0;
			m_initvertice[pos].y = initY(pos,0) = Y(pos,0) = i*th;

			for(int j=1;j<m_numw;++j){
				int pos = j+i*m_numw;
				m_initvertice[pos].x = initX(pos,0) = X(pos,0) = (j-1)*m_size+m_size/2;
				m_initvertice[pos].y = initY(pos,0) = Y(pos,0) = i*th;
			}
		}
	}

	// make mesh
	triangle tmp;

	for(int i=0;i<m_numh-1;++i){

		if(i%2 == 0){

			tmp.id[0] = 0+i*m_numw;
			tmp.id[1] = 0+(i+1)*m_numw;
			tmp.id[2] = 1+(i+1)*m_numw;
			m_triangles.push_back(tmp);

			for(int j=0;j<m_numw-2;++j){

				tmp.id[0] = j+i*m_numw;
				tmp.id[1] = j+1+i*m_numw;
				tmp.id[2] = j+1+(i+1)*m_numw;
				m_triangles.push_back(tmp);

				tmp.id[0] = j+1+i*m_numw;
				tmp.id[1] = j+1+(i+1)*m_numw;
				tmp.id[2] = j+2+(i+1)*m_numw;
				m_triangles.push_back(tmp);
			}

			tmp.id[0] = m_numw-2+i*m_numw;
			tmp.id[1] = m_numw-1+i*m_numw;
			tmp.id[2] = m_numw-1+(i+1)*m_numw;
			m_triangles.push_back(tmp);
		}
		else{

			tmp.id[0] = 0+i*m_numw;
			tmp.id[1] = 1+i*m_numw;
			tmp.id[2] = 0+(i+1)*m_numw;
			m_triangles.push_back(tmp);

			for(int j=1;j<m_numw-1;++j){

				tmp.id[0] = j+i*m_numw;
				tmp.id[1] = j-1+(i+1)*m_numw;
				tmp.id[2] = j+0+(i+1)*m_numw;
				m_triangles.push_back(tmp);

				tmp.id[0] = j+i*m_numw;
				tmp.id[1] = j+1+i*m_numw;
				tmp.id[2] = j+(i+1)*m_numw;
				m_triangles.push_back(tmp);
			}

			tmp.id[0] = m_numw-1+i*m_numw;
			tmp.id[1] = m_numw-2+(i+1)*m_numw;
			tmp.id[2] = m_numw-1+(i+1)*m_numw;
			m_triangles.push_back(tmp);
		}
	}
}
