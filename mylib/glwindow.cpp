/*
	This file is part of UCHIYAMARKER 2.0, a software for deformable random dot markers.
	Copyright (c) 2011 Hideaki Uchiyama

	You can use, copy, modify and re-distribute this software
	for non-profit purposes.
*/

#include "GLWindow.h"

void GLWindow::Draw(const char* data)
{
	glutSetWindow(m_ID);

	glBindTexture(GL_TEXTURE_2D, m_texture);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_w, m_h, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);

	glTexCoord2d(m_wr, 0.0);		glVertex2d(1, 1);
	glTexCoord2d(m_wr, m_hr);		glVertex2d(1, -1);
	glTexCoord2d(0.0, m_hr);		glVertex2d(-1, -1);
	glTexCoord2d(0.0, 0.0);			glVertex2d(-1, 1);

	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void GLWindow::SetTexture(const int texturenum)
{
	m_texture = texturenum;

	glBindTexture(GL_TEXTURE_2D, m_texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, MAXWINDOW, MAXWINDOW, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, 0);
}

void GLWindow::Init(const int iw, const int ih, const char* name)
{
	m_w = iw, m_h = ih;
	m_wr = static_cast<double>(iw) / static_cast<double>(MAXWINDOW);
	m_hr = static_cast<double>(ih) / static_cast<double>(MAXWINDOW);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(iw, ih);
	m_ID = glutCreateWindow(name);
}
