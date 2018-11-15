/*
 *  The real first assignment of graphic
 *  Developed by Mohist Research
 */
#include<stdio.h>
#include <stdarg.h>
#include<string.h>
#include<GL/glut.h>

#include "scene_parser.h"
#include "OrthographicCamera.h" 
#include "PerspectiveCamera.h"
#include "Group.h"
#include "image.h"
#include "hit.h"
#include "material.h"
#include "light.h" 
#include "glCanvas.h"
#include "Sphere.h"
#include "RayTracer.h"
#include "Grid.h"
#include "trace.h"

#define TRACE_CMD

char * input_file = NULL;
int    width = 100;
int    height = 100;
char * output_file = NULL;
float  depth_min = 0;
float  depth_max = 1;
char * depth_file = NULL;
char * normal_file = NULL;
bool needShadeBack = false;
int theta_steps = 10;
int phi_steps = 5;
bool guiMode = false;
SceneParser * parser = NULL;
bool renderShadow = false;
float weight = 0.0;
int bounces = 0;
RayTracer * pTracer = NULL;

Grid * pGrid = NULL;
bool visualize_grid = false;
bool malloc_grid = false;
int nx = 1;
int ny = 1;
int nz = 1;

void log_trace(const char * TAG, const char * format, ...)
{
    char buffer[1024];
    buffer[0] = 0;
    /*
    strncpy(buffer, TAG, strlen(TAG));
    strcat(buffer, ":");
    */

    char content[256];
    va_list ap;
    va_start(ap, format);
    vsnprintf(content, 256, format, ap);
    va_end(ap);

    strncat(buffer, content, 256);
    strcat(buffer, "\n");
#ifdef TRACE_CMD
    printf("%s", buffer);
#endif

#ifdef TRACE_FILE
    FILE * pfile = NULL;
    pfile = fopen("log.txt", "a+");
    fwrite(buf, strlen(buf), 1, pfile);
    fclose(pfile);
#endif
}

void parseArgs(int argc, char **argv)
{
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
        else if (!strcmp(argv[i], "-shade_back"))
        {
            needShadeBack = true;
        }
        else if (!strcmp(argv[i],"-tessellation"))
        {
            i++;
            assert (i < argc);
            theta_steps = atof(argv[i]);
            i++;
            assert (i < argc);
            phi_steps = atof(argv[i]);
        }
        else if (!strcmp(argv[i], "-gui"))
        {
            guiMode = true;
        }
        else if (!strcmp(argv[i], "-shadows"))
        {
            renderShadow = true;
        }
        else if (!strcmp(argv[i], "-weight"))
        {
            i++;
            assert (i < argc);
            weight = atof(argv[i]);
        }
        else if (!strcmp(argv[i], "-bounces"))
        {
            i++;
            assert (i < argc);
            bounces = atoi(argv[i]);
        }
        else if (!strcmp(argv[i], "-visualize_grid"))
        {
            visualize_grid = true;
        }
        else if (!strcmp(argv[i],"-grid"))
        {
            malloc_grid = true;
            i++;
            assert (i < argc);
            nx = atoi(argv[i]);
            i++;
            assert (i < argc);
            ny = atoi(argv[i]);
            i++;
            assert (i < argc);
            nz = atoi(argv[i]);
        }
        else 
        {
            printf ("whoops error with command line argument %d: '%s'\n",i,argv[i]);
            assert(0);
        }
    }
}

void RenderSceneV2()
{
	Camera * pCamera = parser->getCamera();
    
    if (pCamera->getCameraType() == CameraType::Orthographic)
    {
        // crop the output image, to make sure the rectangle is the squre.
        if (width < height)
        {
            height = width;
        }
        else if (width > height)
        {
            width = height;
        }
    }
    else if (pCamera->getCameraType() == CameraType::Perspective)
    {
        float ratio = ((float)width) / ((float)height);
        ((PerspectiveCamera*)pCamera)->setRatio(ratio);
    }

	int i;
	int j;
   
    float airRefraction = 1.0;
    Vec3f backColor = parser->getBackgroundColor();
    
    Image outImg(width, height);
	//outImg.SetAllPixels(backColor);

	for (j = 0; j < height; j++)
	for (i = 0; i < width; i++)
	{
        float u = (i + 0.5) / width;
        float v = (j + 0.5) / height;
        Vec2f p(u, v);
	    Ray	r = pCamera->generateRay(p);

        printf("Render,pixel at x=%d, y=%d\n", i, j);
        cout << "ray=" << r << endl;
        int bounces = 0;
        Hit hit;
        Vec3f pixelcolor = pTracer->traceRay(r, 0.0, bounces, weight,
            airRefraction, hit); 
        
        pixelcolor.Clamp();
        outImg.SetPixel(i, j, pixelcolor);
        cout << "pixel Value:=" << pixelcolor << endl;
    }
	
    outImg.SaveTGA(output_file);

    delete pTracer;
    pTracer = NULL;
}

void RenderScene()
{
	Camera * pCamera = parser->getCamera();

    if (pCamera == NULL)
    {
        fprintf(stderr, "The camera is not defined in config file, so quit.");
        return;
    }
    
    if (pCamera->getCameraType() == CameraType::Orthographic)
    {
#ifdef DEBUG
        printf("The Camera type is Orthographic.\n");
#endif
        // crop the output image, to make sure the rectangle is the squre.
        if (width < height)
        {
            height = width;
        }
        else if (width > height)
        {
            width = height;
        }
    }
    else if (pCamera->getCameraType() == CameraType::Perspective)
    {
#ifdef DEBUG
        printf("The Camera type is Perspective.\n");
#endif
        float ratio = ((float)width) / ((float)height);
        ((PerspectiveCamera*)pCamera)->setRatio(ratio);
    }

    Vec3f backColor = parser->getBackgroundColor();
	Vec3f ambientLight = parser->getAmbientLight();

    Group * objGroups = parser->getGroup();

    int numberLights = parser->getNumLights();

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
                             diffuseColor[1] * ambientLight[1],
                             diffuseColor[2] * ambientLight[2] );



            Vec3f delata(0.0, 0.0, 0.0);

            int k;
            for (k = 0; k < numberLights; k++)
            {
                Light * plight = parser->getLight(k);

                if (plight == NULL)
                    continue;

                Vec3f lightDir;
                Vec3f lightColor;
                float distance;
                plight->getIllumination(point, lightDir, lightColor, distance);
                float d = lightDir.Dot3(normal);

                if (d < 0)
                    d = 0.0;

                Vec3f temp(lightColor[0] * diffuseColor[0],
                           lightColor[1] * diffuseColor[1],
                           lightColor[2] * diffuseColor[2]);

                temp = d * temp;

                delata += temp;
                delata += pM->Shade(r, h, lightDir, lightColor);
            }

            pixelColor += delata;
            pixelColor.Clamp();
            outImg.SetPixel(i, j, pixelColor);
        }
	}

	outImg.SaveTGA(output_file);
}

void RenderDepth()
{

    if (depth_file == NULL)
    {
        fprintf(stderr, "Depth file is null, no need to render.\n");
    }
    else
    {
        Vec3f backColor = parser->getBackgroundColor();
        Image depthtImg(width, height);
        depthtImg.SetAllPixels(backColor);

        Camera * pCamera = parser->getCamera();
        Group * objGroups = parser->getGroup();

        if (pCamera == NULL)
        {
            fprintf(stderr, "The camera is not defined in config file, so quit.");
            return;
        }

        int i;
        int j;
        float tmin = pCamera->getTMin();

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
}

void RenderNormal()
{
    if (normal_file == NULL )
    {
        fprintf(stderr, "The normal file is null, no need to render.\n");
    }
    else
    {
        Image normalImg(width, height);

        Camera * pCamera = parser->getCamera();
        Group * objGroups = parser->getGroup();

        int i;
        int j;
        float tmin = pCamera->getTMin();

        if (pCamera == NULL)
        {
            fprintf(stderr, "The camera is not defined in config file, so quit.");
            return;
        }

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
                    Vec3f n = h.getNormal();

                    if (n.Length() != 1)
                        n.Normalize();

                    normalImg.SetPixel(i, j, n);
                }
            }

        normalImg.SaveTGA(normal_file);
    }
}

void debugTraceRay(float x, float y)
{
    if (pTracer == NULL )
        pTracer = new RayTracer(parser, bounces, weight, renderShadow, false,
            NULL, NULL);
    
    fprintf(stderr, "debugTraceRay, x=%f, y=%f \n", x, y);
	Camera * pCamera = parser->getCamera();
    
    Vec2f p(x, y);
    Ray	ray = pCamera->generateRay(p);
   
    Hit hit;
    
    pTracer->traceRay(ray, 1e-2, 0, 0.01f, 1.0f, hit);

    delete pTracer;
    pTracer = NULL;
}

void RenderAll()
{
    //RenderScene();
    RenderSceneV2();
    RenderDepth();
    RenderNormal();
}

int main(int argc, char ** argv)
{

    parseArgs(argc, argv);
    //Now we start to parse the scene file
    parser = new SceneParser(input_file);

    if (malloc_grid)
        pGrid = new Grid(parser->getGroup()->getBoundingBox(), nx, ny, nz);

    if (pTracer == NULL)
        pTracer = new RayTracer(parser, bounces, weight, renderShadow,
            needShadeBack, pGrid, NULL);
    //OpenGL ui
    if (guiMode)
    {
        Sphere::setTesselationSize(theta_steps, phi_steps);
        glutInit(&argc, argv);
        GLCanvas canvas;
        canvas.initialize(parser, RenderAll, debugTraceRay, pGrid,
            visualize_grid);
    }
    else
    {
        RenderAll();
    }

    delete parser;
    return 0;
}
