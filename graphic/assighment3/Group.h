/*
 * The Group class derived from Object3D.
 * Implemented by Mohism Research
 */

#ifndef __H_GROUP
#define __H_GROUP

#include "Object3D.h"
#include <vector>

using namespace std;

class Group : public Object3D
{
public:
    Group(int numObj);
    ~Group();
    
    bool intersect(const Ray &r, Hit &h, float tmin);
    void addObject(int index, Object3D *obj);
    void paint(void);
private:
    vector<Object3D*> mObjectArray;
    int mNumberObj;
};

#endif
