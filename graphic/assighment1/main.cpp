/*
 *  The first assignment of graphic
 *  Developed by Mohist Research
 */
#include <stdlib.h>
#include <stdio.h>
#include "matrix.h"
#include "ifs.h"

int main(int argc, int ** argv)
{
	char *input_file = NULL;
	int num_points = 10000;
	int num_iters = 10;
	int size = 100;
	char *output_file = NULL;

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
		else 
		{
			printf ("whoops error with command line argument %d: '%s'\n",i,argv[i]);
			assert(0);
		}
	}
	
	// open the file
	FILE *input = fopen(input_file,"r");
	assert(input != NULL);

	ifs ifsInstance;
	// read the number of transforms
	int num_transforms;
	fscanf(input,"%d",&num_transforms);
	infsInstance.setNumberOfTrans(num_transforms);

	// read in the transforms
	for (int i = 0; i < num_transforms; i++) 
	{
		float probability; 
		fscanf (input,"%f",&probability);
		infsInstance.addProbability(probability);

		Matrix m;
		m.Read3x3(input);
		infsInstance.addMatrix(m);
		// < DO SOMETHING WITH probability and m >
	}

	// close the file
	fclose(input);
}
