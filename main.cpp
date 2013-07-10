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
   
	// for detected markers
	for(visible::iterator itpa=(*papers).begin();itpa!=(*papers).end();++itpa){
      
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, m_camimg.w, 0, m_camimg.h, -1, 1);
      
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
      
		mesh m = (*itpa)->GetMesh();
      
		unsigned int id = (*itpa)->GetID();
      
		if(m_texturemode){
         
			// rendering texture on deformed surface
			double x,y;
			m_texture[id].Bind();
			glEnable(GL_TEXTURE_2D);
         
			// for each triangle
			for(triangles::iterator ittr=m.tri->begin(); ittr!=m.tri->end(); ++ittr){
				glBegin(GL_TRIANGLES);
            
				vertex deformed = (*m.ver)[(*ittr).id[0]];
				vertex init = (*m.initver)[(*ittr).id[0]];
				m_texture[id].ConvertTexCoordinate(init.x,init.y,x,y);
				glTexCoord2d(x,y); glVertex3d(deformed.x, deformed.y,0.0f);
            
				deformed = (*m.ver)[(*ittr).id[1]];
				init = (*m.initver)[(*ittr).id[1]];
				m_texture[id].ConvertTexCoordinate(init.x,init.y,x,y);
				glTexCoord2d(x,y); glVertex3d(deformed.x, deformed.y,0.0f);
            
				deformed = (*m.ver)[(*ittr).id[2]];
				init = (*m.initver)[(*ittr).id[2]];
				m_texture[id].ConvertTexCoordinate(init.x,init.y,x,y);
				glTexCoord2d(x,y); glVertex3d(deformed.x, deformed.y,0.0f);
            
				glEnd();
			}
			glDisable(GL_TEXTURE_2D);
		}
		else{
         
			// rendering triangle mesh
			double r,g,b;
			(*itpa)->GetColor(r,g,b);
         
			glColor3d(r,g,b);
         
			glLineWidth(4);
         
			for(triangles::iterator ittr=m.tri->begin(); ittr!=m.tri->end(); ++ittr){
				glBegin(GL_LINE_LOOP);
				vertex tmp = (*m.ver)[(*ittr).id[0]];
				glVertex3d(tmp.x, tmp.y,0.0f);
				tmp = (*m.ver)[(*ittr).id[1]];
				glVertex3d(tmp.x, tmp.y,0.0f);
				tmp = (*m.ver)[(*ittr).id[2]];
				glVertex3d(tmp.x, tmp.y,0.0f);
				glEnd();
			}
		}
      
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
	m_llah.CoordinateTransform(static_cast<double>(m_camimg.h));

	showimg();

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
			// Jim m_llah.Extract(m_nextimg,binarizationThreshold);
         m_llah.Extract(m_nextcamimg);
		}
	}
#else
	m_llah.RetrievebyTracking();
	m_viewmode = m_llah.FindPaper(4);
	m_llah.RetrievebyMatching();
	m_viewmode |= m_llah.FindPaper(8);
   
	getimg();
	// Jim m_llah.Extract(m_nextimg,binarizationThreshold);
   m_llah.Extract(m_nextcamimg);
#endif
	MyTimer::Pop();	
}

// close
void terminator()
{
	glDeleteTextures(m_nummarker+1, m_texturenum);
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
		glutReshapeWindow(m_camimg.w, m_camimg.h);
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
   case 'f':
         m_texturemode = !m_texturemode;
      break;
	case 'z':		
		stopdisplay();		// stop camera stream
		break;
   case 'n':
      glReadPixels(0, 0, m_camimg.w, m_camimg.h, GL_BGR_EXT, GL_UNSIGNED_BYTE, static_cast<char*>(m_camimg));	//! clearly do cast
      m_camimg.Flip(m_camimg);
      m_camimg.Save("result.png");
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
	std::cout << "  f: show texture" << std::endl;
	std::cout << "  z: stop" << std::endl;
	std::cout << "  n: save image" << std::endl;
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
	m_llah.Init(m_camimg.w, m_camimg.h);		// set image size

	// load markers
	char name[256];
	for(int i=0;i<m_nummarker;i++){
      sprintf(name,"./data/%d.txt",i);
		//m_llah.AddPaper(name);
      m_llah.AddPaperWithSize(name);
		std::cout << name << " loaded" << std::endl;
	}

}

// opengl initialization
void glInit()
{
	m_window.Init(m_camimg.w, m_camimg.h, "Deformable Random Dot Markers");	// generate window

	// glut setting
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);

	glGenTextures(m_nummarker+1, m_texturenum);	// call after m_window.Init
	m_window.SetTexture(m_texturenum[0]);		// set texture for opengl window
   
   // load texture
	char name[256];
	for(int i=0;i<m_nummarker;i++){
		sprintf(name,"./data/t%d.png",i);
		m_textureimg[i].Load(name);
		m_texture[i].Init(m_textureimg[i].w, m_textureimg[i].h, m_texturenum[i+1], m_textureimg[i]);
      
		std::cout << name << " loaded" << std::endl;
   }
}

// camera initialization
void camInit()
{
	if(!m_cam.Init()){	// open camera
		exit(1);
	}

	m_camimg.Init(m_cam.w, m_cam.h);		// allocate image
	m_nextcamimg.Init(m_cam.w, m_cam.h);	// allocate image

}

// main function
int main(int argc, char **argv)
{
	camInit();			// camera initialization
   trackingInit();		// tracking initialization
   
	glutInit(&argc, argv);
	glInit();			// opengl initialization
	
   showmode();       // show commands
   
	glutMainLoop();		// for glut

	return 0;

}





