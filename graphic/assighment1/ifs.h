/*  The ifs class is implemented by Alexunder
 *  Mohist Research CopyRight @ 2014-2015
 */

#ifndef __H_IFS
#define __H_IFS

#include "matrix.h"

class ifs
{
public:
	ifs()
	{
		mNumberOfTrans = 0;
		mArrayMatrices = NULL;
		mProbibilitiesArray = NULL;
	}
	~ifs()
	{
		if (mArrayMatrices != NULL)
			delete [] mArrayMatrices;
		
		if (mProbibilitiesArray != NULL)
			delete [] mProbibilitiesArray;
	}

	void readDescription();
	void renderImage();
private:
	int mNumberOfTrans;
	Matrix * mArrayMatrices;
	float  * mProbibilitiesArray;	
};

#endif
