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
        Object3D * obj =dynamic_cast<Object3D *>(mObjectArray[i]);
        delete obj;
    }
}

bool Group::intersect(const Ray &r, Hit &h, float tmin)
{
    vector<Object3D*>::iterator itr_end = mObjectArray.end();
    vector<Object3D*>::iterator itr;

	bool ret = false;
	
	for (itr = mObjectArray.begin(); itr != itr_end; ++itr)
	{
        ret = (*itr)->intersect(r, h, tmin);
		if (ret == true)
			return true;
	}

	return false;
}

void Group::addObject(int index, Object3D *obj)
{
    mObjectArray.push_back(obj);
}
