#ifndef IDENTLABEL_HPP_
#define IDENTLABEL_HPP_

#include <list>
#include <stdio.h>
#include <stdlib.h>

class IdentLabel
{
public:
	std::list<int> xList;
	std::list<int> yList;

    IdentLabel()
    {
    }

    void Add(int x, int y)
    {
    	if (x>y)
    	{
    		int z =x;
    		x = y;
    		y = z;
    	}
    	if (x!=0 && y!=0 && x!=y)
    	{
    		bool doNothing = false;
    		for(unsigned int i=0;i<xList.size();i++)
    		{
    			if (xList.front()==x)
    				if (yList.front()==y)
    				{
    					doNothing=true;
    					break;
    				}
				int temp = xList.front();
				xList.pop_front();
				xList.push_back(temp);
				temp = yList.front();
				yList.pop_front();
				yList.push_back(temp);
    		}
    		if(!doNothing)
    		{
    			xList.push_front(x);
    			yList.push_front(y);
    		}
    	}
    }
    unsigned int GetCount()
    {
    	return xList.size();
    }
};

#endif
