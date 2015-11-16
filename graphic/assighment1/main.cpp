/*
 *  The real first assignment of graphic
 *  Developed by Mohist Research
 */
#include<stdio.h>
#include<string.h>

#include "scene_parser.h"
#include "OrthographicCamera.h" 
#include "Group.h"
#include "image.h"

int main(int argc, char ** argv)
{
    char * input_file = NULL;
    int    width = 100;
    int    height = 100;
    char * output_file = NULL;
    float  depth_min = 0;
    float  depth_max = 1;
    char * depth_file = NULL;

    // raytracer -input scene1_1.txt -size 200 200 -output output1_1.tga -depth 9 10 depth1_1.tga

    for (int i = 1; i < argc; i++) 
    {
        if (!strcmp(argv[i],"-input")) 
        {
            i++; 
            assert (i < argc); 
            input_file = argv[i];
        } 
        else if (!strcmp(argv[i],"-size")) 
        {
            i++; 
            assert (i < argc); 
            width = atoi(argv[i]);
            i++; 
            assert (i < argc); 
            height = atoi(argv[i]);
        } 
        else if (!strcmp(argv[i],"-output")) 
        {
            i++;
            assert (i < argc); 
            output_file = argv[i];
        } 
        else if (!strcmp(argv[i],"-depth")) 
        {
            i++;
            assert (i < argc); 
            depth_min = atof(argv[i]);
            i++;
            assert (i < argc); 
            depth_max = atof(argv[i]);
            i++;
            assert (i < argc); 
            depth_file = argv[i];
        } 
        else 
        {
            printf ("whoops error with command line argument %d: '%s'\n",i,argv[i]);
            assert(0);
        }
    }

	// crop the output image, to make sure the rectangle is the squre.
	if (width < height)
	{
		height = width;
	}
	else if (width > height)
	{
		width = height;	
	}

    //Now we start to parse the scene file
	SceneParser parser(input_file);

	OrthographicCamera * pCamera  =(OrthographicCamera*)parser.getCamera();
	Vec3f backColor = parser.getBackgroundColor();
	Group * objGroups = parser.getGroup();

	Image outImg(width, height);
	outImg.SetAllPixels(backColor);

	//Generate the image content
	int i;
	int j;
    float tmin = 0.00001;

	for (j = 0; j < height; j++)
	for (i = 0; i < width; i++)
	{
        float u = (i + 0.5) / width;
        float v = (j + 0.5) / height;
        Vec2f p(u, v);
	    Ray	r = pCamera->generateRay(p);

        bool ishit = false;
        Hit h;
        ishit = objGroups.intersect(r, h, tmin);

        if (ishit)
        {
            tmin = h.getT();
            Vec3f color = h.getMaterial().getDiffuseColor();
            outImg.setPixel(i, j, color);
        }
	}

	outImg.SaveTGA(output_file);
}
