/*
 * The Group class derived from Object3D.
 * Implemented by Mohism Research
 */

#include "Group.h"


Group::Group(int numObj)
    : mNumberObj(numObj)
{
}

Group::~Group()
{
    int i;

    for (i = 0; i < mNumberObj; i++)
    {
        Object3D * obj =dynamic_cast<Object *>(mObjectArray[i]);
        delete obj;
    }
}

bool Group::intersect(const Ray &r, Hit &h, float tmin);
{
    vector<Object3D*>::iterator itr_end = mObjectArray.end();
    vector<Object3D*>::iterator itr;
    for (itr = vAAA.begin(); itr != itr_end; ++itr)
        (*itr)->intersect(r, h, tmin);

void Group::addObject(int index, Object3D *obj)
{
    mObjectArray.push_back(obj);
}
