/*
 This file is part of UCHIYAMARKER 2.0, a software for deformable random dot markers.
 Copyright (c) 2011 Hideaki Uchiyama
 
 You can use, copy, modify and re-distribute this software
 for non-profit purposes.
 */

#ifndef __MAIN_H__
#define __MAIN_H__


#ifdef _OPENMP
#include <omp.h>
#endif

const int m_nummarker = 1;	// number of markers //6

#include <cstdlib>
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
GLuint m_texturenum[m_nummarker+1];

#include <iostream>

#include "mylib/glwindow.h"
GLWindow m_window;

#include "mylib/mymat.h"

#include "mylib/mycam.h"
MyCam m_cam;

#include "mylib/myimage.h"
MyImage m_camimg;
MyImage m_nextcamimg;

#include "mylib/gltexture.h"
GLTexture m_texture[m_nummarker];
MyImage m_textureimg[m_nummarker];

#include "mylib/mytimer.h"


#include "LLAH.h"
LLAH m_llah;

void setH(const MyMat& H, GLdouble *dst);
void drawCG();
void drawimg();
void getimg();
void showimg();
void update();

void mainloop();

void terminator();

void display();
void displaysize();
void stopdisplay();
void keyboard(unsigned char key, int x, int y);
void idle();
void reshape(const int w, const int h);

void showmode();
void trackingInit();
void camInit();
void glInit();

bool m_binarymode = false;
bool m_stopmode = false;
bool m_viewmode = false;
bool m_ptmode = false;
bool m_texturemode = false;

#endif


// Jim
int binarizationThreshold = 100;
int thresholdIncr = 10;
void modifyBinarizationThreshold(int i){binarizationThreshold += i; printf("New Binarization Threshold: %d",binarizationThreshold);}
