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
	int cnt = 0;
	Hit temphit;

	for (itr = mObjectArray.begin(); itr != itr_end; ++itr)
	{
        ret = (*itr)->intersect(r, temphit, tmin);
		if (ret == true)
		{
			float t = temphit.getT();
#ifdef DEBUG
//			printf("obj index=%d, t=%f, t0=%f, t1=%f\n", cnt, t, t0, t1);
#endif
			if (t <= t1 && t >= t0)
			{
				is_hit = true;
				t1 = t;
				h.set(t, temphit.getMaterial(), temphit.getNormal(), temphit.getIntersectionPoint());
			}
		}

		cnt++;
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
