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
		mCountMatrices = 0;
		mCountProbiblity = 0;
	}
	~ifs()
	{
        clearMemory();
	}

    void setNumberOfTrans(int num)
    {
        mNumberOfTrans = num;
        clearMemory();

        mArrayMatrices = new Matrix[mNumberOfTrans];
        mProbibilitiesArray = new float[mNumberOfTrans];
    }

	//void readDescription();
	bool addMatrix(Matrix m);
	bool addProbibility(float p);
	void renderImage();
private:
    void clearMemory()
    {
		if (mArrayMatrices != NULL)
			delete [] mArrayMatrices;
		
		if (mProbibilitiesArray != NULL)
			delete [] mProbibilitiesArray;
    }
private:
	int mNumberOfTrans;
	Matrix * mArrayMatrices;
	float  * mProbibilitiesArray;
	int mCountMatrices;
	int mCountProbiblity;
};

#endif
