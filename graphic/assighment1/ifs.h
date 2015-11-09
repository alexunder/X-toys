/*  The ifs class is implemented by Alexunder
 *  Mohist Research CopyRight @ 2014-2015
 */

#ifndef __H_IFS
#define __H_IFS

#include "matrix.h"
#include "image.h"
#include <vector>

using namespace std;

class ifs
{
public:
	ifs()
	{
		mNumberOfTrans = 0;
		mCountMatrices = 0;
		mCountProbabilities = 0;
	}
	~ifs()
	{
		clearMemory();
	}

	void setNumberOfTrans(int num);
	bool addMatrix(Matrix m);
	bool addProbability(float p);
	void renderImage(Image &img, int numPoints, int numIters);
private:
	void clearMemory()
	{
		mArrayMatrices.clear();
		mProbabilitiesArray.clear();
	}

    int getTransformationIndex(float probability);
private:
	int mNumberOfTrans;
	vector<Matrix> mArrayMatrices;
	vector<float> mProbabilitiesArray;
	int mCountMatrices;
	int mCountProbabilities;
};

#endif
