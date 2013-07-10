/*
 This file is part of UCHIYAMARKER 2.0, a software for deformable random dot markers.
 Copyright (c) 2011 Hideaki Uchiyama
 
 You can use, copy, modify and re-distribute this software
 for non-profit purposes.
 */

#include "paper.h"

Paper::Paper(const double w, const double h):m_detected(false), H(3,3)
{
	m_mesh.Base(w,h,w/10);
}

ptlist* Paper::GetPts()
{
	return &m_pts;
}

void Paper::Reprojection(const MyMat &db, MyMat &im)
{
	im.Mul(H, db);
}

void Paper::InitHomography(const MyMat &db, const MyMat &im, const double repro)
{
	H.Homography(db,im,repro);
}

bool Paper::Deformed()
{
	return m_detected;
}

void Paper::Detected()
{
	m_detected = true;
	m_mesh.Init(H);
}

void Paper::Lost()
{
	m_detected = false;
}

mesh Paper::GetMesh()
{
	return m_mesh.GetTrianges();
}

void Paper::RecoverMesh(const eblobs &blobs, const int min)
{
	m_detected = m_mesh.Recover(blobs, m_pts, min);
}

void Paper::AddPt(const pt &p)
{
	m_pts.push_back(p);
}

void Paper::ComputeBarycenter(pt &p)
{
	p.inside = m_mesh.GetBarycenter(p.x,p.y,p.centerid[0],p.centerval[0],p.centerid[1],p.centerval[1],p.centerid[2],p.centerval[2]);
}

unsigned Paper::GetID()
{
	return m_id;
}

void Paper::SetID(const unsigned in)
{
	m_id = in;
}

void Paper::GetColor(double &r, double &g, double &b)
{
	r = m_r;
	g = m_g;
	b = m_b;
}

void Paper::SetColor(const double r, const double g, const double b)
{
	m_r = r;
	m_g = g;
	m_b = b;
}












