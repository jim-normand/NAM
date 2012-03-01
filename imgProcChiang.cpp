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
         // cout << "ajout de (" << r << "," << g << "," << b << ")" << endl;
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
   cvShowImage("source",colorSrcImg);
   
   cout << cvGetSize(colorSrcImg).width << "w" <<  cvGetSize(colorSrcImg).height << endl;
   cout << "--frame " << colorSrcImg->nChannels <<endl;
   // seuillage rapide : parcours de l'ensemble des pixels et validation par la table
   uchar *redPointer = (uchar*) colorSrcImg->imageData;
   uchar *greyPointer = (uchar*) graySrcImg->imageData;
   for (int y=0 ; y<colorSrcImg->height ; y++) {
      //cout << "line " << y ;
      for (int x=0 ; x<colorSrcImg->width ; x++) {
         int bb = (int) *redPointer++;
         int gg = (int) *redPointer++;
         int rr = (int) *redPointer++;
         
         if (thresTable[rr][gg][bb]) {
            //if (rr != 255 || gg != 255 || bb != 255) {
            //   cout << "+ajout de (" << (int)rr << "," << (int)gg << "," << (int)bb << ")" << endl;
            //}
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
   cvShowImage("Binarization", graySrcImg);
   
   // morpho math
   cvMorphologyEx(graySrcImg, graySrcImg, NULL,
                  cvCreateStructuringElementEx(3, 3, 2, 2, CV_SHAPE_RECT),
                  CV_MOP_CLOSE,
                  2);
   cvShowImage("Morpho-math", graySrcImg);
   
   cvReleaseImage(&graySrcImg);
}


