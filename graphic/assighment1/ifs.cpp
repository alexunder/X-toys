
#include "ifs.h"


void ifs::setNumberOfTrans(int num)
{
	mNumberOfTrans = num;
	clearMemory();
}

bool ifs::addMatrix(Matrix m)
{
	if (mCountMatrices == mNumberOfTrans)
		return false;

	mArrayMatrices.push_back(m); 
	mCountMatrices++;
}

bool ifs::addProbability(float p)
{
	if (mCountProbabilities == mNumberOfTrans)
		return false;

	mProbabilitiesArray.push_back(p); 
	mCountProbabilities++;
}

void ifs::renderImage(Image &img, int numPoints, int numIters)
{
	int i;
	time_t t;
    srand((unsigned) time(&t));
	
	if (numIters == 0)
	{
		for (i = 0; i < numPoints; i++)
		{
			int coordinate_x = rand() % img.Width();
        	int coordinate_y = rand() % img.Height();
        	Vec3f color(0.0, 0.0, 0.0);
        	img.SetPixel(coordinate_x, coordinate_y, color);
		}
	}

	//for (i = 0; i < numIters; i++)
	{
		int trans_index = rand() % mNumberOfTrans;
		Matrix m = mArrayMatrices[trans_index];
	}
}