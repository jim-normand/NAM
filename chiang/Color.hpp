#ifndef COLOR_HPP_
#define COLOR_HPP_

class Color
{
    int num;
public:
    int b, g, r;

    Color()
    {
    	b= 0;
    	g= 0;
    	r=0;
    	num=0;
    }

    Color(int x, int y, int z)
    {
    	b=x;
    	g= y;
    	r =z;
    	num=0;
    }

    void Add( int x, int y, int z)
    {
        b=( b*num+x)/(num+1);
        g=( g*num+y)/(num+1);
        r=( r*num+z)/(num+1);
        ++num;
    }
};

#endif
