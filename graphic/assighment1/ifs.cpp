
#include "ifs.h"


void ifs::setNumberOfTrans(int num)
{
	mNumberOfTrans = num;
	clearMemory();
}

bool ifs::addMatrix(Matrix m)
{
	if (mCountMatrices == mNumerOfTrans)
		return false;

	mArrayMatrices.pushback(m); 
	mCountMatrices++;
}

bool ifs::addProbability(float p)
{
	if (mCountProbablities == mNumerOfTrans)
		return false;

	mProbabilitiesArray.pushback(m); 
	mCountProbablities++;
}
