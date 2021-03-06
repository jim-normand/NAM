//
//  imgProcChiang.cpp
//  RandomDotMarkers
//
//  Created by Guillaume Moreau on 13/07/11.
//  Copyright 2011 Ecole Centrale de Nantes. All rights reserved.
//
#include "Skeletonize.h"
#include "Intersections.h"

#include <iostream>
#include <fstream>

using namespace std;

#include "imgProcChiang.h"

/**
 implémentation adaptée de Fast and Inexpensive Color Image Segmentation for
 interactive robots. J. Bruce, T. Balch, M. Veloso. Proc. IEEE/RSJ International
 Conference on Intelligent Robots and Systems, 2000.
*/
imgProcChiang::imgProcChiang(IplImage *src) : Skeletonize(src) {
   // initialisation de la threshold table à 0
   for (int i=0 ; i < 256 ; i++) {
      for (int j=0 ; j < 256 ; j++) {
         for (int k=0 ; k<256 ; k++) {
            thresTable[i][j][k] = false;
         }
      }
   }
   
   // debug mode à faux par défaut
   debugVizMode = false;
   
   // remplissage avec les valeurs données dans le fichier
   // @bug: le dernier point sera lu en double (eof() positionné après tentative
   // de lecture. mais sans effet sur la suite...
   ifstream inf;
   inf.open("filters.dat");
   int line = 0; // pour compter le nombre de lignes lues
   while (!inf.eof()) {
      int r,g,b;
      inf >> r >> g >> b;
      if (r>=0 && r<256 && g>=0 && g<256 && b>=0 && b<256) {
         thresTable[r][g][b] = true;
         line++;
         // debug lecture
         cout << "ajout de (" << r << "," << g << "," << b << ")" << endl;
      }
      else {
         cerr << "pb a la lecture du fichier de filtrage" << endl;
      }
   }
   cout << line << " lignes lues dans le fichier de filtre" << endl;
   inf.close();
}

void imgProcChiang::processImage(IplImage *colorSrcImg, int threshold) {
   IplImage *graySrcImg = cvCreateImage(cvGetSize(colorSrcImg), IPL_DEPTH_8U, 1);
   
   if (debugVizMode) { 
      cvShowImage("source",colorSrcImg);
   }
   
   cout << cvGetSize(colorSrcImg).width << "w" <<  cvGetSize(colorSrcImg).height << endl;
   cout << "--frame " << colorSrcImg->nChannels <<endl;
   // seuillage rapide : parcours de l'ensemble des pixels et validation par la table
   uchar *redPointer = (uchar*) colorSrcImg->imageData;
   uchar *greyPointer = (uchar*) graySrcImg->imageData;
   for (int y=0 ; y<colorSrcImg->height ; y++) {
      //cout << "line " << y ;
      redPointer = (uchar*) (colorSrcImg->imageData+y*colorSrcImg->widthStep);
      greyPointer = (uchar*) (graySrcImg->imageData+y*graySrcImg->widthStep);
      for (int x=0 ; x<colorSrcImg->width ; x++) {
         int bb = (int) *redPointer++;
         int gg = (int) *redPointer++;
         int rr = (int) *redPointer++;
         
         if (thresTable[rr][gg][bb]) {
            if (rr != 255 || gg != 255 || bb != 255) {
               cout << "+ajout de (" << (int)rr << "," << (int)gg << "," << (int)bb << ")" << endl;
            }
            *greyPointer++ = 255;
         }
         else {
            *greyPointer++ = 0;
         }
      }
      //*redPointer += colorSrcImg->width;
      //cout << endl;
   }
   // on montre le résultat
   if (debugVizMode) {
      cvShowImage("Binarization", graySrcImg);
   }
   
   // morpho math
   /*cvMorphologyEx(graySrcImg, graySrcImg, NULL,
                  cvCreateStructuringElementEx(3, 3, 2, 2, CV_SHAPE_RECT),
                  CV_MOP_CLOSE,
                  2);*/
   
   cvMorphologyEx(graySrcImg, graySrcImg, NULL,
                  cvCreateStructuringElementEx(5, 5, 2, 2, CV_SHAPE_RECT),
                  CV_MOP_CLOSE,
                  2);
   
   if (debugVizMode) {
      cvShowImage("Morpho-math", graySrcImg);
   }
   
   // Applying the Skeletonization
   m_src = graySrcImg;
   skeletonize();
   
   if (debugVizMode) {
      cvShowImage("Skeletonization", graySrcImg);
   }
   
   /*
   // Test Jim
   CvMemStorage* storage = cvCreateMemStorage(0);
   CvSeq* lines = 0;
   IplImage* dst = cvCreateImage( cvGetSize(graySrcImg), 8, 1 );
   IplImage* color_dst = cvCreateImage( cvGetSize(graySrcImg), 8, 3 );
   int i;
   
   lines = cvHoughLines2( dst,
                         storage,
                         CV_HOUGH_MULTI_SCALE,
                         1,
                         0.01,
                         75,
                         0,
                         0 );
   
   for( i = 0; i < MIN(lines->total,100); i++ )
   {
      float* line = (float*)cvGetSeqElem(lines,i);
      float rho = line[0];
      float theta = line[1];
      CvPoint pt1, pt2;
      double a = cos(theta), b = sin(theta);
      double x0 = a*rho, y0 = b*rho;
      pt1.x = cvRound(x0 + 1000*(-b));
      pt1.y = cvRound(y0 + 1000*(a));
      pt2.x = cvRound(x0 - 1000*(-b));
      pt2.y = cvRound(y0 - 1000*(a));
      cvLine( color_dst, pt1, pt2, CV_RGB(255,0,0), 3, 8 );
   }
   
   cvNamedWindow( "Hough", 1 );
   cvShowImage( "Hough", color_dst );
   // End Test
   */

   // Computing Intersections and Drawing in Red them on the original Image
   vector<CvPoint> crossroads = Intersections(graySrcImg)();
   for (unsigned int i = 0; i < crossroads.size(); ++i)
   {
      //cvCircle(colorSrcImg, crossroads[i], 2, CV_RGB(255, 0, 0), -1);
      cvCircle(colorSrcImg, crossroads[i], 4, CV_RGB(0, 0, 0), -1);

   }
   if (debugVizMode) {
      cvShowImage("source avec intersections",colorSrcImg);
   }

   cvReleaseImage(&graySrcImg);
}


