
#include "ifs.h"
#include <string.h>

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
    int j;
    time_t t;
    srand((unsigned) time(&t));
    int height = img.Height();
    int width = img.Width();
    Vec3f color(0.0, 1.0, 0.0);

    float coordinate_x = 0.0;
    float coordinate_y = 0.0;

    Vec3f iteratedPoint(0, 0, 0);
    for (i = 0; i < numPoints; i++)
    {
        coordinate_x = rand() / (float)(RAND_MAX);
        coordinate_y = rand() / (float)(RAND_MAX);
        iteratedPoint.Set(coordinate_x, coordinate_y, 0.0);

        for (j = 0; j < numIters; j++)
        {
            int indexProbability = 0;
            float probability =(float)(rand()) /(float)(RAND_MAX);
            //printf("probability=%f\n", probability);
            int transIndex = getTransformationIndex(probability);
            //printf("transIndex=%d\n", transIndex);
            Matrix m = mArrayMatrices[transIndex];
            m.Transform(iteratedPoint);

        }

        coordinate_x = iteratedPoint[0];
        coordinate_y = iteratedPoint[1];

        if (coordinate_x >= 0 && coordinate_x < 1 &&
                coordinate_y >= 0 && coordinate_y < 1 )
        {
            img.SetPixel(coordinate_x * width, coordinate_y * height, color);
        }
    }
}

int ifs::getTransformationIndex(float probability)
{
    float * pTemp = new float[mNumberOfTrans];
    memset(pTemp, 0, mNumberOfTrans*sizeof(float));

    int i;
    pTemp[0] = mProbabilitiesArray[0];
    for (i = 1; i < mNumberOfTrans; i++)
    {
        pTemp[i] = pTemp[i - 1] + mProbabilitiesArray[i];
    }

    for (i = 0; i < mNumberOfTrans; i++)
    {
        if (probability <= pTemp[i])
            break;
    }

    delete [] pTemp;

    return i;
}
