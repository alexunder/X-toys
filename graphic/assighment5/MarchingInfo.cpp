#include "marchingInfo.h"

MarchingInfo::MarchingInfo()
{
    i = -1;
    j = -1;
    k = -1;
}

MarchingInfo::~MarchingInfo()
{
}

void MarchingInfo::nextCell()
{
    if (t_next_x < t_next_y&&t_next_x < t_next_z)
    {
        i += sign_x;
        tmin = t_next_x;
        t_next_x += d_tx;
        if (sign_x == 1)
            normal.Set(-1.0f,0.0f,0.0f);
        else if (sign_x == -1)
            normal.Set(1.0f, 0.0f, 0.0f);
    }
    else if (t_next_y < t_next_x&&t_next_y < t_next_z)
    {
        j += sign_y;
        tmin = t_next_y;
        t_next_y += d_ty;
        if (sign_y == 1)
            normal.Set(0.0f, -1.0f, 0.0f);
        else if (sign_y == -1)
            normal.Set(0.0f, 1.0f, 0.0f);
    }
    else
    {
        k += sign_z;
        tmin = t_next_z;
        t_next_z += d_tz;
        if (sign_z == 1)
            normal.Set(0.0f, 0.0f, -1.0f);
        else if (sign_z == -1)
            normal.Set(0.0f, 0.0f, 1.0f);
    }
}

void MarchingInfo::setIndices(int i, int j, int k)
{
    this->i = i;
    this->j = j;
    this->k = k;
}

void MarchingInfo::getIndices(int &i, int &j, int &k)
{
    i = this->i;
    j = this->j;
    k = this->k;
}

void MarchingInfo::set_tmin(float t)
{
    this->tmin = t;
}

float MarchingInfo::get_tmin()
{
    return this->tmin;
}

void MarchingInfo::setNext(float x, float y, float z)
{
    this->t_next_x = x;
    this->t_next_y = y;
    this->t_next_z = z;
}

void MarchingInfo::setDelta(float x, float y, float z)
{
    this->d_tx = x;
    this->d_ty = y;
    this->d_tz = z;
}

void MarchingInfo::setSign(int x, int y, int z)
{
    this->sign_x = x;
    this->sign_y = y;
    this->sign_z = z;
}

Vec3f MarchingInfo::getNormal()
{
    return normal;
}

void MarchingInfo::setNormal(Vec3f normal)
{
    this->normal = normal;
}

void MarchingInfo::getVertex(Vec3f &a, Vec3f &b, Vec3f &c, Vec3f &d)
{
    a = this->a;
    b = this->b;
    c = this->c;
    d = this->d;
}

void MarchingInfo::setVertex(Vec3f a, Vec3f b, Vec3f c, Vec3f d)
{
    this->a = a;
    this->b = b;
    this->c = c;
    this->d = d;
}
