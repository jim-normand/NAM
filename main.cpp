/*
This file is part of UCHIYAMARKERS, a software for random dot markers.
Copyright (c) 2011 Hideaki Uchiyama

You can use, copy, modify and re-distribute this software
for non-profit purposes.
*/

#include "main.h"

// set projection matrix for opengl
void setH(const MyMat& src, GLdouble *dst)
{
	dst[0] = src(0,0);
	dst[1] = src(1,0);
	dst[2] = 0.0f;
	dst[3] = src(2,0);

	dst[4] = src(0,1);
	dst[5] = src(1,1);
	dst[6] = 0.0f;
	dst[7] = src(2,1);

	dst[8] = 0.0f;
	dst[9] = 0.0f;
	dst[10] = 1.0f;
	dst[11] = 0.0f;

	dst[12] = src(0,2);
	dst[13] = src(1,2);
	dst[14] = 0.0f;
	dst[15] = src(2,2);
}

// show CG in opengl window
void drawCG()
{
	visible *papers = m_llah.GetVisiblePaper();
	
	// for detected papers
	for(visible::iterator itpa=(*papers).begin();itpa!=(*papers).end();++itpa){

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, m_cam.w, 0, m_cam.h, -1, 1);

		GLdouble m[16];
		setH((*itpa)->H, m);
		glMultMatrixd(m);

		printf("Paper ID: %d\n",(*itpa)->id);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glColor3d((*itpa)->r, (*itpa)->g, (*itpa)->b);


		glLineWidth(5);

		glBegin(GL_LINE_LOOP);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(600.0f,0.0f,0.0f);
		glVertex3f(600.0f,600.0f,0.0f);
		glVertex3f(0.0f,600.0f,0.0f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(600.0f,0.0f,0.0f);
		glVertex3f(0.0f,600.0f,0.0f);
		glVertex3f(600.0f,600.0f,0.0f);
		glEnd();


		glPopMatrix();

	}
}

// show camera image
void drawimg()
{
	m_window.Draw(m_camimg);
}

// get image
void getimg()
{
	m_cam.Get(m_nextcamimg);
	m_nextimg.Resize(m_nextcamimg);
}

// show image
void showimg()
{
	if(m_binarymode){
		m_llah.DrawBinary(m_camimg);

		if(m_ptmode){
			m_llah.DrawPts(m_camimg);
		}
	}

	drawimg();

	if(m_viewmode && !m_binarymode){
		drawCG();
	}

	update();
}

// update opengl window
void update()
{
	glutSwapBuffers();
}

// main process
void mainloop()
{
	MyTimer::Push("Time");

	m_llah.SetPts();
	m_llah.CoordinateTransform(static_cast<double>(m_cam.h));

	showimg();

	m_img.Swap(m_nextimg);
	m_camimg.Swap(m_nextcamimg);

#ifdef _OPENMP
#pragma omp parallel
	{
#pragma omp master
		{
			m_llah.RetrievebyTracking();
			m_viewmode = m_llah.FindPaper(4);
			m_llah.RetrievebyMatching();
			m_viewmode |= m_llah.FindPaper(8);
		}
#pragma omp single
		{			
			getimg();
			m_llah.Extract(m_nextimg,binarizationThreshold);
		}
	}
#else
	m_llah.RetrievebyTracking();
	m_viewmode = m_llah.FindPaper(4);
	m_llah.RetrievebyMatching();
	m_viewmode |= m_llah.FindPaper(8);
   
	getimg();
	m_llah.Extract(m_nextimg,binarizationThreshold);
#endif
	MyTimer::Pop();	
}

// close
void terminator()
{
	glDeleteTextures(NUMTEXTURE, m_texture);
	exit(1);
}

// for glut
void display()
{
	mainloop();
}

// full screen or normal screen
void displaysize()
{
	static bool full = false;

	if(full){
		glutPositionWindow(100, 100);
		glutReshapeWindow(m_cam.w, m_cam.h);
	}
	else{
		glutFullScreen();
	}

	full = !full;
}

// stop camera stream
void stopdisplay()
{
	if(m_stopmode){
		glutDisplayFunc(display);
	}
	else{
		glutDisplayFunc(showimg);
	}
	m_stopmode = !m_stopmode;
}

// for glut
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 0x1b:		// ESC
		terminator();		// close 
		break;
	case 'a':		
		displaysize();		// full screen or normal screen
		break;
	case 's':		// show binary image
		m_binarymode = !m_binarymode;
		break;
	case 'd':		// show extracted keypoints in binary image
		m_ptmode = !m_ptmode;
		break;
	case 'z':		
		stopdisplay();		// stop camera stream
		break;
         
   case 't': // increase binarization threshold
      modifyBinarizationThreshold(thresholdIncr);
      break;
   case 'y': // decrease binarization threshold
      modifyBinarizationThreshold(-thresholdIncr);
      break;
         
	default:
		break;
	}
}

// show commands
void showmode()
{
	std::cout << "ESC: exit" << std::endl;
	std::cout << "  a: full screen" << std::endl;
	std::cout << "  s: show binary" << std::endl;
	std::cout << "  d: show pts" << std::endl;
	std::cout << "  z: stop" << std::endl;
}

// for glut
void idle()
{
	glutPostRedisplay();
}

// for glut
void reshape(const int w, const int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// tracking initialization
void trackingInit()
{
	m_llah.Init(m_img.w, m_img.h);		// set image size

	// load markers
	char name[256];
	int nummarker = 1;

	for(int i=0;i<nummarker;i++){
		//sprintf_s(name,sizeof(name),"./data/%d.txt",i);
      sprintf(name,"./data/%d.txt",i);
		m_llah.AddPaper(name);
		std::cout << name << " loaded" << std::endl;
	}

	showmode();		// show commands
}

// opengl initialization
void glInit()
{
	m_window.Init(m_cam.w, m_cam.h, "UCHIYAMARKERS");	// generate window

	// glut setting
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);

	glGenTextures(NUMTEXTURE, m_texture);	// call after m_window.Init
	m_window.SetTexture(m_texture[0]);		// set texture for opengl window
}

// camera initialization
void camInit()
{
	if(!m_cam.Init()){	// open camera
		exit(1);
	}

	m_camimg.Init(m_cam.w, m_cam.h);		// allocate image
	m_nextcamimg.Init(m_cam.w, m_cam.h);	// allocate image

	m_img.Init(m_cam.w, m_cam.h);			// allocate image
	m_nextimg.Init(m_cam.w, m_cam.h);		// allocate image

}

// main function
int main(int argc, char **argv)
{
	camInit();			// camera initialization

	glutInit(&argc, argv);
	glInit();			// opengl initialization
	trackingInit();		// tracking initialization

	glutMainLoop();		// for glut

	return 0;

}





