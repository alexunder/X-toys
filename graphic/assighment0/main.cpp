/*
 *  The first assignment of graphic
 *  Developed by Mohist Research
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "matrix.h"
#include "ifs.h"
#include "image.h"

int main(int argc, char ** argv)
{
    char *input_file = NULL;
    int num_points = 10000;
    int num_iters = 10;
    int size = 100;
    char * output_file = NULL;
    char * foreColor = NULL;
    char * backColor = NULL;

    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i],"-input"))
        {
            i++; assert (i < argc);
            input_file = argv[i];
        }
        else if (!strcmp(argv[i],"-points"))
        {
            i++; assert (i < argc);
            num_points = atoi(argv[i]);
        }
        else if (!strcmp(argv[i],"-iters"))
        {
            i++; assert (i < argc);
            num_iters = atoi(argv[i]);
        }
        else if (!strcmp(argv[i],"-size"))
        {
            i++; assert (i < argc);
            size = atoi(argv[i]);
        }
        else if (!strcmp(argv[i],"-output"))
        {
            i++; assert (i < argc);
            output_file = argv[i];
        }
        else if (!strcmp(argv[i],"-fcolor"))
        {
            i++; assert (i < argc);
            foreColor = argv[i];
        }
        else if (!strcmp(argv[i],"-bcolor"))
        {
            i++; assert (i < argc);
            backColor = argv[i];
        }
		else
		{
			printf ("whoops error with command line argument %d: '%s'\n",i,argv[i]);
			assert(0);
		}
	}
	
	// open the file
	FILE *input = fopen(input_file,"r");
	assert(input != NULL);

    Vec3f fColor(0.0, 0.0, 0.0);
    Vec3f bColor(1.0, 1.0, 1.0);

    if (foreColor != NULL)
    {
        if (!strcmp(foreColor, "red"))
        {
            fColor.Set(1.0, 0.0, 0.0);
        }
        else if (!strcmp(foreColor, "green"))
        {
            fColor.Set(0.0, 1.0, 0.0);
        }
        else if (!strcmp(foreColor, "blue"))
        {
            fColor.Set(0.0, 0.0, 1.0);
        }
    }

    if (backColor != NULL)
    {
        if (!strcmp(backColor, "white"))
        {
            bColor.Set(1.0, 1.0, 1.0);
        }
        else if (!strcmp(backColor, "black"))
        {
            bColor.Set(0.0, 0.0, 0.0);
        }
    }

	ifs ifsInstance;
	// read the number of transforms
	int num_transforms;
	fscanf(input,"%d",&num_transforms);
	ifsInstance.setNumberOfTrans(num_transforms);

    int i;
	// read in the transforms
	for (i = 0; i < num_transforms; i++) 
	{
		float probability; 
		fscanf (input,"%f",&probability);
		ifsInstance.addProbability(probability);

		Matrix m;
		m.Read3x3(input);
		ifsInstance.addMatrix(m);
	}

	Image outImg(size, size);
	outImg.SetAllPixels(bColor);
	ifsInstance.renderImage(outImg, num_points, num_iters, fColor);
	outImg.SaveTGA(output_file);
	// close the file
	fclose(input);
	return 0;
}
