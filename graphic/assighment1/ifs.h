/*  The ifs class is implemented by Alexunder
 *  Mohist Research CopyRight @ 2014-2015
 */

#ifndef __H_IFS
#define __H_IFS

#include "matrix.h"
#include <Vector>

class ifs
{
public:
	ifs()
	{
		mNumberOfTrans = 0;
		mArrayMatrices = NULL;
		mProbabilitiesArray = NULL;
		mCountMatrices = 0;
		mCountProbablities = 0;
	}
	~ifs()
	{
        clearMemory();
	}


	void setNumberOfTrans(int num);
	//void readDescription();
	bool addMatrix(Matrix m);
	bool addProbability(float p);
	void renderImage();
private:
    void clearMemory()
    {
		if (mArrayMatrices != NULL)
			delete [] mArrayMatrices;
		
		if (mProbabilitiesArray != NULL)
			delete [] mProbibilitiesArray;
    }
private:
	int mNumberOfTrans;
	Vector<Matrix> mArrayMatrices;
	Vector<float> mProbabilitiesArray;
	int mCountMatrices;
	int mCountProbabilities;
};

#endif
