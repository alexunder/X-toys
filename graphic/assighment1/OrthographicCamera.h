/*
 * The OrthographicCamera  class derived from Camera
 * Implemented by Mohism Research
 */

#ifndef __H_ORTHOGRAPHICCAMERA
#define __H_ORTHOGRAPHICCAMERA

class OrthographicCamera : public Camera
{
public:
	Ray generateRay(Vec2f point);
	float getTMin() const;
};

#endif
