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
#include "hit.h"
#include "material.h"
#include "light.h" 

int main(int argc, char ** argv)
{
    char * input_file = NULL;
    int    width = 100;
    int    height = 100;
    char * output_file = NULL;
    float  depth_min = 0;
    float  depth_max = 1;
    char * depth_file = NULL;
    char * normal_file = NULL;

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
        else if (!strcmp(argv[i], "-normals"))
        {
            i++;
            assert (i < argc);
            normal_file = argv[i];
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
	Vec3f ambientLight = parser.getAmbientLight();

    Group * objGroups = parser.getGroup();

    int numberLights = parser.getNumLights();

    Image outImg(width, height);
	outImg.SetAllPixels(backColor);

	//Generate the image content
	int i;
	int j;
    float tmin = pCamera->getTMin();

#ifdef DEBUG
	printf("tmin=%f\n", tmin);
#endif

	for (j = 0; j < height; j++)
	for (i = 0; i < width; i++)
	{
        float u = (i + 0.5) / width;
        float v = (j + 0.5) / height;
        Vec2f p(u, v);
	    Ray	r = pCamera->generateRay(p);

        bool ishit = false;
        Hit h;
        ishit = objGroups->intersect(r, h, tmin);

        if (ishit)
        {
#ifdef DEBUG
        printf("Pixel(%d, %d), t=%f\n", i, j, h.getT());
#endif
            Material * pM = h.getMaterial();
            Vec3f normal = h.getNormal();
            Vec3f point = h.getIntersectionPoint();

            Vec3f diffuseColor = pM->getDiffuseColor();

            Vec3f pixelColor(diffuseColor[0] * ambientLight[0],
                             diffuseColor[1] * ambientLight[2],
                             diffuseColor[2] * ambientLight[2] );



            Vec3f delata(0.0, 0.0, 0.0);

            int k;
            for (k = 0; k < numberLights; k++)
            {
                Light * plight = parser.getLight(k);

                if (plight == NULL)
                    continue;

                Vec3f lightDir;
                Vec3f lightColor;
                plight->getIllumination(point, lightDir, lightColor);
                float d = lightDir.Dot3(normal);

                if (d < 0)
                    continue;

                Vec3f temp(lightColor[0] * diffuseColor[0],
                           lightColor[1] * diffuseColor[1],
                           lightColor[2] * diffuseColor[2]);

                temp = d * temp;

                delata += temp;
            }

            pixelColor += delata;
            outImg.SetPixel(i, j, pixelColor);
        }
	}

	outImg.SaveTGA(output_file);

	//Now let us render the gray image.
	Image depthtImg(width, height);
	depthtImg.SetAllPixels(backColor);

	for (j = 0; j < height; j++)
	for (i = 0; i < width; i++)
	{
        float u = (i + 0.5) / width;
        float v = (j + 0.5) / height;
        Vec2f p(u, v);
	    Ray	r = pCamera->generateRay(p);

        bool ishit = false;
        Hit h;
        ishit = objGroups->intersect(r, h, tmin);

        if (ishit)
        {
			float t = h.getT();
#ifdef DEBUG
			printf("t=%f, tmin=%f, tmax=%f\n", t, depth_min, depth_max);
#endif
			if (t > depth_max || t < depth_min)
				continue;

			float GrayComponent = (t - depth_min) / (depth_max - depth_min);
			GrayComponent = 1 - GrayComponent;
#ifdef DEBUG
			printf("GrayComponent =%f\n", GrayComponent);
#endif
            Vec3f color(GrayComponent, GrayComponent, GrayComponent);
            depthtImg.SetPixel(i, j, color);
        }

	}

	depthtImg.SaveTGA(depth_file);
}
