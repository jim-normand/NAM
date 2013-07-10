#include "RoadExtraction.hpp"
#include "Color.hpp"
#include "IdentLabel.hpp"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

#include <time.h>


void
SetMatElem(CvMat *mat, int row, int col, int channel, int val)
{
	((float*)(mat->data.ptr+mat->step*row))[col*mat->step+channel]= val;
}

RoadExtraction::RoadExtraction():
source(0),
modified(0),
progressiveKMean(0)
{
}

RoadExtraction::~RoadExtraction()
{
	if (source)
		cvReleaseImage(&source);
	if (modified)
		cvReleaseImage(&modified);
	if (progressiveKMean)
		cvReleaseImage(&progressiveKMean);
}

void
RoadExtraction::LoadImage(const char * filename)
{
	source = cvLoadImage (filename, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
	if(!source)
		std::cout<<"Failed to load image";
	else
	{
//		IplImage* hsv = cvCreateImage( cvGetSize(source), IPL_DEPTH_8U, 3 );
//		cvCvtColor(source,source, CV_RGB2HSV);
//		cvNamedWindow("Source Hsv", CV_WINDOW_AUTOSIZE);
//		cvShowImage("Source Hsv", hsv );
	}
}

void
RoadExtraction::ShowSource()
{
	cvNamedWindow("Source Image", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("Source Image", 100, 100);
	cvShowImage("Source Image", source );
}

void
RoadExtraction::InitModified()
{
	if(modified)
		cvReleaseImage(&modified);
	modified = cvCloneImage(source);
}

void
RoadExtraction::InitKMean()
{
	progressiveKMean = cvCreateImage(cvGetSize(source),8,3);
	cvZero(progressiveKMean);
	ChiangKMean(modified,0);
}

void
RoadExtraction::MeanShift(int spatialRadius, int colorRadius)
{
	int level = 1;
	IplImage *dst_img;

/*
	CvRect roi;
	//set the ROI in source image
	roi.x = roi.y = 0;
	roi.width = source->width & -(1 << level);
	roi.height = source->height & -(1 << level);
	cvSetImageROI (source, roi);
*/

	//apply mean shift
	dst_img = cvCloneImage (modified);
	cvPyrMeanShiftFiltering(modified,dst_img,spatialRadius,colorRadius,level,cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,10,1));

	//show result image
	cvNamedWindow ("Mean shift", CV_WINDOW_AUTOSIZE);
	cvShowImage ("Mean shift", modified);

	modified = cvCloneImage(dst_img);
	cvReleaseImage (&dst_img);
}

void
RoadExtraction::ChiangKMean(IplImage* image,int profondeur)
{
	IplImage *dst_img;
	dst_img = cvCreateImage(cvGetSize(modified),8,3);
	cvZero(dst_img);

	CvMat *sample = cvCreateMat(image->height*modified->width, 1, CV_32FC(3));
	CvMat *cluster = cvCreateMat(image->height*modified->width, 1, CV_32SC1);

   // ça ressemble violemment à une copie pure et simple non ?
	for(int i=0; i<image->height ;i++)//row
	{
		for( int j=0; j<image->width; j++)//col
		{
			long int index= i*image->width+j;
			SetMatElem(sample,index,0,0,CV_IMAGE_ELEM(image,uchar,i,j*3));
			SetMatElem(sample,index,0,1,CV_IMAGE_ELEM(image,uchar,i,j*3+1));
			SetMatElem(sample,index,0,2,CV_IMAGE_ELEM(image,uchar,i,j*3+2));
		}
	}
	int K=profondeur;
	if(profondeur==0)
		K = 10;

   /**
    utilisation de cvKMeans2 : 
      - sample est la matrice des échantillons
      - K le nombre de clusters (10 ici pour la première fois, ensuite on incrémente)
      - le critère d'arrêts, ici le epsilon + le nombre d'itérations
      - les autres paramètres par défaut (1 seule exécution, pas de générateur aléatoire
      externe, pas de labels initiaux, pas de centre des clusters en sortie, pas
      d'indicateurs de compacité
   */
	cvKMeans2(sample, K, cluster,cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 10 ));

	//Assign color in filter to destination image
	for (int filternb=0;filternb<K;filternb++)
	{
		cvZero(dst_img);
		int pixelsCount = 0;
		for(int i=0; i<dst_img->height; i++)
		{
			for( int j=0; j<dst_img->width; j++)
			{
				long int index= i* dst_img->width+j;
				int cluster_idx= cluster->data.i[index];
				if (cluster_idx == filternb)
				{
					++pixelsCount;
					CV_IMAGE_ELEM(dst_img,uchar, i, j*3) = CV_IMAGE_ELEM(image,uchar, i, j*3);
					CV_IMAGE_ELEM(dst_img,uchar, i, j*3+1) = CV_IMAGE_ELEM(image,uchar, i, j*3+1);
					CV_IMAGE_ELEM(dst_img,uchar, i, j*3+2) = CV_IMAGE_ELEM(image,uchar, i, j*3+2);
				}
			}
		}
		//Manual selection of the filters
		if (pixelsCount!=0)
		{
			cvNamedWindow ("K Mean", CV_WINDOW_AUTOSIZE);
			cvShowImage ("K Mean", dst_img);
			printf("Keep filter %d? y/n\n",filternb+1);
			int key = cvWaitKey(0);
			switch(key)
			{
			case 'y':
				cvAdd(progressiveKMean,dst_img,progressiveKMean,NULL);
            printClusterValues(dst_img);
				break;
			case 't':
				ChiangKMean(dst_img,profondeur+1);
				break;
			default :
				break;
			}
		}
		else
			printf("empty filter moving on to next one\n");
	}


		//Show selected filters
		cvNamedWindow ("Selected Filters", CV_WINDOW_AUTOSIZE);
		cvShowImage ("Selected Filters", progressiveKMean);
		cvSaveImage("extracted chiang.bmp",progressiveKMean);
		//Show image
		cvNamedWindow ("K Mean", CV_WINDOW_AUTOSIZE);
		cvShowImage ("K Mean", dst_img);

   
   // affichage des valeurs de couleur des clusters foreground
   for (map<CvScalar, int>::iterator i = foreground.begin() ; i!=foreground.end() ; i++) {
      cout << "Not sorted : (" << i->first.val[0] << "," << i->first.val[1] << "," << i->first.val[2] << ") : " << i->second << endl;
   }
   
   /*
    
    NB : cannot work with maps, we need multimaps
    since if two colors have the same numbers of appearances, the last one will replace the previous ones in the map
    
   // second cluster par ordre décroissant d'occurences
   map<int,CvScalar> foregroundColors;
   for (map<CvScalar,int>::iterator i = foreground.begin() ; i!=foreground.end() ; i++) {
      foregroundColors[i->second] = i->first;
      cout << "foregroundColors[" << i->first.val[0] << "," << i->first.val[1] << "," << i->first.val[2] << "] = " << i->second << endl;
   }
   
      // affichage des valeurs de couleur des clusters foreground
   for (map<int,CvScalar>::iterator i = foregroundColors.begin() ; i!=foregroundColors.end() ; i++) {
      cout << "(" << i->second.val[0] << "," << i->second.val[1] << "," << i->second.val[2] << ") : " << i->first << endl;
   }*/
   
   // sauvegarde des valeurs de l'histogramme dans un fichier
   std::ofstream of;
   of.open("filters.dat");
   /*for (map<int,CvScalar>::iterator i = foregroundColors.begin() ; i != foregroundColors.end() ;  i++) {
      of <<  i->second.val[2] << " " << i->second.val[1] << " " << i->second.val[0] << endl;
   }*/
   
   for (map<CvScalar,int>::iterator i = foreground.begin() ; i != foreground.end() ;  i++) {
      of <<  i->first.val[2] << " " << i->first.val[1] << " " << i->first.val[0] << endl;
   }
   
   of.close();
   
	cvReleaseMat(&sample);
	cvReleaseMat(&cluster);
	cvReleaseImage(&dst_img);
}

// boolean function for comparing CvScalar (uses lexicographic order) 
bool operator<(CvScalar a,CvScalar b) {
   if (a.val[0] < b.val[0]) {
      return 1;
   }
   else {
      if (a.val[0] == b.val[0]) {
         if (a.val[1] < b.val[1]) {
            return 1;
         }
             else {
                if (a.val[1] == b.val[1]) {
                   if (a.val[2] < b.val[2]) {
                      return 1;
                   }
                   else {
                      return 0;
                   }
                }
                else {
                   return 0;
                }
             }
      }
      else {
         return 0;
      }
   }
}
             
// affiche la liste des couleurs utilisées dans un cluster
void RoadExtraction::printClusterValues(IplImage *im) {
   for (int y=0 ; y<im->height ; y++) {
      for (int x=0 ; x<im->width ; x++) {
         uchar r = CV_IMAGE_ELEM(im,uchar, y, x*3);
         uchar g = CV_IMAGE_ELEM(im,uchar, y, x*3+1);
         uchar b = CV_IMAGE_ELEM(im,uchar, y, x*3+2);
         if (r || g || b) {
            cout << "(" << (int) r << "," << (int) g << "," << (int) b << ")" << endl;
            CvScalar tmp; 
            tmp.val[0]=r; tmp.val[1]=g; tmp.val[2]=b; tmp.val[3]=0;
            if (foreground.find(tmp) != foreground.end()) {
               foreground[tmp]++;
            }
            else {
               foreground[tmp]=1;
            }
         }
      }
   }
}

void
RoadExtraction::ManualKMean(int K, int b,int g, int r)
{
	IplImage *dst_img;
	dst_img = cvCreateImage(cvGetSize(modified),8,3);
	cvZero(dst_img);

//	Color *color_tab= new Color[K]();
	CvMat *sample = cvCreateMat(modified->height*modified->width*3, 1, CV_32FC1);
	CvMat *cluster = cvCreateMat(modified->height*modified->width*3, 1, CV_32SC1);

	//Image -> List des elements a trier
	for(int i=0; i<modified->height ;i++)//row
	{
		for( int j=0; j<modified->width; j++)//col
		{
			long int index= i*modified->width+j;
			SetMatElem(sample,3*index,0,0,CV_IMAGE_ELEM(modified,uchar,i,j*3));
			SetMatElem(sample,3*index+1,0,0,CV_IMAGE_ELEM(modified,uchar,i,j*3+1));
			SetMatElem(sample,3*index+2,0,0,CV_IMAGE_ELEM(modified,uchar,i,j*3+2));
		}
	}

	//Preparation des barycentres
	int bar_ch1 = b, bar_ch2 = g, bar_ch3 = r;
	CvMat *barList = cvCreateMat(K*3, 1, CV_32FC1);
	SetMatElem(barList,0,0,0,b);
	SetMatElem(barList,1,0,0,g);
	SetMatElem(barList,2,0,0,r);

	for(int i=1;i<K;i++)
	{
		bar_ch1 = rand() % 255 + 1;
		bar_ch2 = rand() % 255 + 1;
		bar_ch3 = rand() % 255 + 1;
		while(abs(bar_ch1-b)+abs(bar_ch2-g)+abs(bar_ch3-r)<50)
		{
			srand ( time(0) );
			bar_ch1 = rand() % 255 + 1;
			bar_ch2 = rand() % 255 + 1;
			bar_ch3 = rand() % 255 + 1;
		}
		SetMatElem(barList,3*i,0,0,bar_ch1);
		SetMatElem(barList,3*i+1,0,0,bar_ch2);
		SetMatElem(barList,3*i+2,0,0,bar_ch3);
		printf("barycentre %d : %d %d %d\n",i,bar_ch1,bar_ch2,bar_ch3);
	}

	for(int index=0;index<sample->rows/3;index++)
	{
		int baryID=0;
		int baryDist=abs(barList->data.i[0]-sample->data.i[index*3])+abs(barList->data.i[1]-sample->data.i[index*3+1])+abs(barList->data.i[2]-sample->data.i[index*3+2]);
		for(int j=1;j<barList->rows/3;j++)
		{
			int curDist = abs(barList->data.i[3*j]-sample->data.i[index*3])+abs(barList->data.i[3*j+1]-sample->data.i[index*3+1])+abs(barList->data.i[3*j+2]-sample->data.i[index*3+2]);
			if(curDist<baryDist)
			{
				baryDist = curDist;
				baryID = j;
			}
			cluster->data.i[index] = baryID;
		}
	}

	printf("1st loot ok\n");

	for(int it=0;it<10;it++)
	{
		printf("loop %d\n",it);

		for(int j=0;j<barList->rows/3;j++)
		{
			printf("bary # %d\n",j);
			int nbValue=0,bSum=0,gSum=0,rSum=0;
			for(int index=0;index<sample->rows/3;index++)
			{
				if(cluster->data.i[index] == j)
				{
					bSum+=sample->data.i[index*3];
					gSum+=sample->data.i[index*3+1];
					rSum+=sample->data.i[index*3+2];
					nbValue++;
				}
			}
			printf("proche de %d valeurs\n######\n",nbValue);

			if (nbValue==0)
				++nbValue;

			SetMatElem(barList,3*j,0,0,(int)(bSum/nbValue));
			SetMatElem(barList,3*j+1,0,0,(int)(gSum/nbValue));
			SetMatElem(barList,3*j+2,0,0,(int)(rSum/nbValue));
		}
	}
	printf("k mean fini\n");
	for(int i=0; i<dst_img->height; i++)
	{
		for( int j=0; j<dst_img->width; j++)
		{
			long int index= i* dst_img->width+j;
			if (cluster->data.i[index] == 0)
			{
				CV_IMAGE_ELEM(dst_img,uchar, i, j*3) = 255;
				CV_IMAGE_ELEM(dst_img,uchar, i, j*3+1) = 255;
				CV_IMAGE_ELEM(dst_img,uchar, i, j*3+2) = 255;
			}
		}
	}
	printf("affichage pret\n");
	cvNamedWindow ("Fixed Filter", CV_WINDOW_AUTOSIZE);
	cvShowImage ("Fixed Filter", dst_img);
	cvWaitKey(0);
}

void
RoadExtraction::KMean(int K)
{
	IplImage *dst_img;
	dst_img = cvCreateImage(cvGetSize(modified),8,3);
	cvZero(dst_img);

	Color *color_tab= new Color[K]();
	CvMat *sample = cvCreateMat(modified->height*modified->width, 1, CV_32FC(1));
	CvMat *cluster = cvCreateMat(modified->height*modified->width, 1, CV_32SC1);

	for(int i=0; i<modified->height ;i++)//row
	{
		for( int j=0; j<modified->width; j++)//col
		{
			long int index= i*modified->width+j;
			SetMatElem(sample,index,0,0,CV_IMAGE_ELEM(modified,uchar,i,j*3));
			SetMatElem(sample,index,0,1,CV_IMAGE_ELEM(modified,uchar,i,j*3+1));
			SetMatElem(sample,index,0,2,CV_IMAGE_ELEM(modified,uchar,i,j*3+2));
		}
	}


	cvKMeans2(sample, K, cluster,cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 10 ));



	//Visit each pixel in original image, calculate average color in each clusters
	//and store result in Table color_tab
	for(int i=0; i<dst_img->height; i++)
	{
		for( int j=0; j<dst_img->width; j++)
		{
			long int index= i* dst_img->width+j;
			int cluster_idx= cluster->data.i[index];
			int x= CV_IMAGE_ELEM(modified,uchar, i, j*3);
			int y=  CV_IMAGE_ELEM(modified,uchar, i, j*3+1);
			int z= CV_IMAGE_ELEM(modified,uchar, i, j*3+2);

			color_tab[cluster_idx].Add(x,y,z);
		}
	}


	//Assign average color in cluster to destination image
	cvZero(modified);

	for (int filternb=0;filternb<K;filternb++)
	{
		cvZero(dst_img);
		for(int i=0; i<dst_img->height; i++)
		{
			for( int j=0; j<dst_img->width; j++)
			{
				long int index= i* dst_img->width+j;
				int cluster_idx= cluster->data.i[index];
				if (cluster_idx == filternb)
				{
					CV_IMAGE_ELEM(dst_img,uchar, i, j*3) = 255;
					CV_IMAGE_ELEM(dst_img,uchar, i, j*3+1) = 255;
					CV_IMAGE_ELEM(dst_img,uchar, i, j*3+2) = 255;
//					CV_IMAGE_ELEM(dst_img,uchar, i, j*3) = color_tab[cluster_idx].b;
//					CV_IMAGE_ELEM(dst_img,uchar, i, j*3+1) = color_tab[cluster_idx].g;
//					CV_IMAGE_ELEM(dst_img,uchar, i, j*3+2) = color_tab[cluster_idx].r;
				}
			}
		}

		if (KeepFilter(dst_img))
			cvAdd(modified,dst_img,modified,NULL);

	}

	//Show selected filters
	cvNamedWindow ("Selected Filters", CV_WINDOW_AUTOSIZE);
	cvShowImage ("Selected Filters", modified);

	//Show image
	cvNamedWindow ("K Mean", CV_WINDOW_AUTOSIZE);
	cvShowImage ("K Mean", dst_img);

	cvSaveImage("extract.bmp",modified);



	delete color_tab;
	cvReleaseMat(&sample);
	cvReleaseMat(&cluster);
	cvReleaseImage(&dst_img);
}

//TODO mem free
void
RoadExtraction::HoughLines()
{
	IplImage* dst = cvCreateImage( cvGetSize(source), 8, 1 );
	IplImage* grey_dst = cvCreateImage( cvGetSize(source), 8, 1 );
	IplImage* color_dst = cvCreateImage( cvGetSize(source), 8, 3 );
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* lines = 0;
	int i;
	cvCvtColor(modified, grey_dst, CV_BGR2GRAY);
	cvCanny( grey_dst, dst, 50, 200, 3 );
	cvCvtColor( dst, color_dst, CV_GRAY2BGR );

    lines = cvHoughLines2( dst, storage, CV_HOUGH_STANDARD, 1, CV_PI/180, 100, 0, 0 );

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
}

void
RoadExtraction::ErodeDilateTest()
{
	IplConvKernel* element = 0;
	static int elementValues[9] = {1,1,0,1,1,0,0,0,0};
	element = cvCreateStructuringElementEx(3,3,0,0,CV_SHAPE_CUSTOM,elementValues);

	IplConvKernel* elementErode = 0;
	static int elementErodeValues[9] = {1,1,1,0,1,1,1,1,0};
	elementErode = cvCreateStructuringElementEx(3,3,1,1,CV_SHAPE_CUSTOM,elementErodeValues);

	cvDilate(modified,modified,element,1);
	cvReleaseStructuringElement(&element);

	cvErode(modified,modified,elementErode,1);
	cvReleaseStructuringElement(&elementErode);

	cvNamedWindow( "Erode Dilate", 1 );
	cvShowImage( "Erode Dilate", modified );
}


int
RoadExtraction::ContourNumber(IplImage* image)
{
	IplImage* thresholded = cvCreateImage( cvGetSize(image), 8, image->nChannels );
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contour = 0;

	cvThreshold( image, thresholded, 1, 255, CV_THRESH_BINARY );
	IplImage* BWImg;
	BWImg = cvCreateImage( cvSize(image->width, image->height), IPL_DEPTH_8U, 1 );
	cvCvtColor( thresholded, BWImg, CV_BGR2GRAY );
	cvReleaseImage(&thresholded);

	int res = cvFindContours( BWImg, storage, &contour, sizeof(CvContour),CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

	cvReleaseImage(&BWImg);
	cvReleaseMemStorage(&storage);
	return res;
}

int
RoadExtraction::NonBlackPix(IplImage* image)
{
	int nonBlackPixelCount =0;
	for(int i=0; i<image->height; i++)
		for( int j=0; j<image->width; j++)
			if (CV_IMAGE_ELEM(image,uchar, i, j*3) !=0 || CV_IMAGE_ELEM(image,uchar, i, j*3+1) !=0 || CV_IMAGE_ELEM(image,uchar, i, j*3+2) !=0)
				++nonBlackPixelCount;
	return nonBlackPixelCount;
}

bool
RoadExtraction::KeepFilter(IplImage* image)
{
	//TODO automatical selection
	//Semi-Manual selection of the filters
	int nonBlackPixelCount = NonBlackPix(image);
	int contours = ContourNumber(image);
	printf("%d contours pour %d pixels\n",contours,nonBlackPixelCount);

//	if (nonBlackPixelCount>contours*10)
//	{
		{
			IplImage* grayImg;
			IplImage* cannyImg;
			grayImg = cvCreateImage( cvSize(image->width, image->height), IPL_DEPTH_8U, 1 );
			cvCvtColor( image, grayImg, CV_BGR2GRAY );
			cannyImg = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
			cvCanny( grayImg, cannyImg, 10, 100, 3 );
//			cvNamedWindow ("canny", CV_WINDOW_AUTOSIZE);
//			cvShowImage ("canny", cannyImg);
			cvReleaseImage(&cannyImg);
			cvReleaseImage(&grayImg);
		}

		cvNamedWindow ("K Mean", CV_WINDOW_AUTOSIZE);
		cvShowImage ("K Mean", image);

		printf("Keep filter ? y/n\n");

//		if (nonBlackPixelCount>image->width*image->height/2)
//		{
//			printf("Discarded: Background\n");
//			return false;
//		}
//		else
//			return true;
		int key = cvWaitKey(0);

		cvDestroyWindow("canny");
		cvDestroyWindow("K mean");

		switch(key)
		{
		case 'y':
			return true;;
		default :
			return false;
		}
//	}
//	else
//	{
//		printf("Discarded: filter with too many fragments\n");
//		cvNamedWindow ("K Mean", CV_WINDOW_AUTOSIZE);
//		cvShowImage ("K Mean", image);
//		cvWaitKey(0);
//		cvDestroyWindow("K mean");
//		return false;
//	}
}

//Marche pas
void
RoadExtraction::ConnexCount(IplImage* image)
{
//	cvNamedWindow ("connex", CV_WINDOW_AUTOSIZE);
//	cvShowImage ("connex", image);
//	IplImage* labelImage = cvCreateImage( cvSize(image->width, image->height), IPL_DEPTH_8U, 1 );
//	cvZero(labelImage);
//
//	int labelCount = 0;
//	int height = image->height;
//	int width = image->width;
//
//	IdentLabel* labelIdentique = new IdentLabel();
//	for(int i=0; i<height; i++)
//	{
//		for( int j=0; j<width; j++)
//		{
//			//If it isn't a background pixel
//			if (CV_IMAGE_ELEM(image,uchar, i, j) !=0 )
//			{
//				int value=0;
//				for (int k=0;k<4;k++)
//				{
//					if (j>1-(k%3) && i>1-(k/3))
//					{
//						value = CV_IMAGE_ELEM(image,uchar, i-1+(k/3), j-1+(k%3));
//						if (value!=0)
//						{
//							labelIdentique->Add(CV_IMAGE_ELEM(labelImage,uchar, i-1+(k/3), j-1+(k%3)),CV_IMAGE_ELEM(labelImage,uchar, i, j));
//							CV_IMAGE_ELEM(labelImage,uchar, i, j) = CV_IMAGE_ELEM(labelImage,uchar, i-1+(k/3), j-1+(k%3));
//						}
//					}
//				}
//				if (CV_IMAGE_ELEM(labelImage,uchar, i, j)==0)
//				{
//			    	printf("labelCount %d\n",labelCount);
//					++labelCount;
//					CV_IMAGE_ELEM(labelImage,uchar, i, j) = labelCount;
//				}
//			}
//		}
//	}
//
//	cvReleaseImage(&labelImage);
//	printf("labelcount = %d\nlabel differents = %d\n",labelCount,labelCount-labelIdentique->GetCount());
//	delete labelIdentique;
}
