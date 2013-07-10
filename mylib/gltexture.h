/*
	This file is part of UCHIYAMARKER 2.0, a software for deformable random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#ifndef GLTEXTURE_H
#define GLTEXTURE_H

#ifdef WIN32
#include <windows.h>
#endif

#include <OpenGL/gl.h>

const int MAXTEXTURE = 1024;

class GLTexture
{
public:
	void ConvertTexCoordinate(const double x, const double y, double &dx, double &dy);
	void Bind();
	void Init(const int iw, const int ih, const int texturenum, const char *data);

private:
	GLuint m_texture;
	int m_ID;
	double m_wr, m_hr;
	int m_w, m_h;
};



#endif