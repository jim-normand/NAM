/*
This file is part of UCHIYAMARKERS, a software for random dot markers.
Copyright (c) 2011 Hideaki Uchiyama

You can use, copy, modify and re-distribute this software
for non-profit purposes.
*/

#ifdef WIN32
#include <windows.h>
#endif

#ifdef _OPENMP
#include <omp.h>
#endif

#include <cstdlib>
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
const GLsizei NUMTEXTURE = 2;
GLuint m_texture[NUMTEXTURE];

#include <iostream>

#include "mylib/glwindow.h"
GLWindow m_window;

#include "mylib/mymat.h"
#include "mylib/mycam.h"
MyCam m_cam;

#include "mylib/myimage.h"
MyImage m_camimg;
MyImage m_nextcamimg;
MyImage m_img;
MyImage m_nextimg;

#include "LLAH.h"
LLAH m_llah;

#include "mylib/mytimer.h"


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

// Jim
int binarizationThreshold = 100;
int thresholdIncr = 10;
void modifyBinarizationThreshold(int i){binarizationThreshold += i; printf("New Binarization Threshold: %d",binarizationThreshold);}
