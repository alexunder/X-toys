/*
 * The Group class derived from Object3D.
 * Implemented by Mohism Research
 */

#include "Group.h"
#include "hit.h"

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

    float t0 = tmin;
    float t1 = 100000.0f;

	bool is_hit = false;
	bool ret = false;

	for (itr = mObjectArray.begin(); itr != itr_end; ++itr)
	{
        ret = (*itr)->intersect(r, h, tmin);
		if (ret == true)
		{
			float t = h.getT();

			if (t <= t1 && t >= t0)
			{
				is_hit = true;
				t1 = t;
			}
		}
	}

	if (is_hit)
		return true;
	else
		return false;
}

void Group::addObject(int index, Object3D *obj)
{
    mObjectArray.push_back(obj);
}
