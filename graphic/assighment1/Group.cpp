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

	bool is_hit = false;
	bool ret = false;
	float small = 100000.0;
	Hit tempHit;

	for (itr = mObjectArray.begin(); itr != itr_end; ++itr)
	{
        ret = (*itr)->intersect(r, tempHit, tmin);
		if (ret == true)
		{
			float temp = h.getT();

			if (temp < small)
			{
				is_hit = true;
				small = temp;
			    h.set(tempHit.getT(), tempHit.getMaterial(), tempHit.getIntersectionPoint());
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
