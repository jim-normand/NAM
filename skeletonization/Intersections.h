#ifndef INTERSECTIONS_H_23022011114829
#define INTERSECTIONS_H_23022011114829

#include <opencv/cv.h>

class Intersections
{
    public:
        Intersections(IplImage *src);
        std::vector<CvPoint> operator()(void);

    private:
        IplImage *m_src;
};

#endif
