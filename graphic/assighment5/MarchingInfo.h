#ifndef _MARCHINGINFO_H_
#define _MARCHINGINFO_H_

#include "vectors.h"

class MarchingInfo
{
public:
    MarchingInfo();
    ~MarchingInfo();
    void nextCell();

    void getIndices(int &i, int &j, int &k);
    void setIndices(int i, int j, int k);

    float get_tmin();
    void set_tmin(float t);

    Vec3f getNormal();
    void setNormal(Vec3f normal);

    void getVertex(Vec3f &a, Vec3f &b, Vec3f &c, Vec3f &d);
    void setVertex(Vec3f a, Vec3f b, Vec3f c, Vec3f d);

    void setNext(float x, float y, float z);
    void setDelta(float x, float y, float z);
    void setSign(int x, int y, int z);

private:
    float tmin;
    int i, j, k;//gridµÄË÷Òý
    float t_next_x, t_next_y, t_next_z;
    float d_tx, d_ty, d_tz;
    int sign_x, sign_y, sign_z;
    Vec3f normal;
    Vec3f a, b, c, d;
};

#endif
