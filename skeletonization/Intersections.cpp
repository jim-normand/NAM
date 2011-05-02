#include "Intersections.h"

using namespace std;

Intersections::Intersections(IplImage *src)
 : m_src(src)
{
}

vector<CvPoint> Intersections::operator()(void)
{
    const int cMin = 1, cMax = m_src->width - 2;
    const int lMin = 1, lMax = m_src->height - 2;
    const int rowOffset = m_src->widthStep;
    IplImage *blobImg = cvCloneImage(m_src);
    vector<CvPoint> intersections;

    unsigned char *srcData = reinterpret_cast<unsigned char*>(m_src->imageData);
    unsigned char *blobData = reinterpret_cast<unsigned char*>(blobImg->imageData);

    for (int l = lMin; l <= lMax; ++l)
    {
        for (int c = cMin; c <= cMax; ++c)
        {
            int offset = l * rowOffset + c;
            if (srcData[offset] == 255) // White
            {
                int neibghours = 0;

                if (srcData[offset-rowOffset-1] == 255) ++neibghours;
                if (srcData[offset-rowOffset  ] == 255) ++neibghours;
                if (srcData[offset-rowOffset+1] == 255) ++neibghours;
                if (srcData[offset          -1] == 255) ++neibghours;
                if (srcData[offset          +1] == 255) ++neibghours;
                if (srcData[offset+rowOffset-1] == 255) ++neibghours;
                if (srcData[offset+rowOffset  ] == 255) ++neibghours;
                if (srcData[offset+rowOffset+1] == 255) ++neibghours;

                if (neibghours < 3)
                {
                    blobData[offset] = 0;
                }
            }
        }
    }

    for (int l = lMin; l <= lMax; ++l)
    {
        for (int c = cMin; c <= cMax; ++c)
        {
            int offset = l * rowOffset + c;
            if (blobData[offset] == 255) // White
            {
                CvConnectedComp comp;
                cvFloodFill(blobImg, cvPoint(c, l), cvScalarAll(0),
                            cvScalarAll(0), cvScalarAll(0), &comp, 8);

                intersections.push_back(cvPoint(comp.rect.x + comp.rect.width / 2,
                                                comp.rect.y + comp.rect.height / 2));
            }
        }
    }

    cvReleaseImage(&blobImg);

    return intersections;
}
