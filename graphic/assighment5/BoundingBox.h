#ifndef _BOUNDING_BOX_H_
#define _BOUNDING_BOX_H_

#include "vectors.h"
#include "ray.h"

#include <assert.h>

#define min2(a,b) (((a)<(b))?(a):(b))
#define max2(a,b) (((a)>(b))?(a):(b))

// ====================================================================
// ====================================================================

class BoundingBox {
public:

    // CONSTRUCTOR & DESTRUCTOR
    BoundingBox(Vec3f _min, Vec3f _max) {
        Set(_min,_max); }

    BoundingBox()
    {
        min = Vec3f(  INFINITY,  INFINITY,  INFINITY);
        max = Vec3f( -INFINITY, -INFINITY, -INFINITY);
    }
    ~BoundingBox() {}

    friend BoundingBox Union(const BoundingBox &b, const Vec3f &p);
    friend BoundingBox Union(const BoundingBox &b, const BoundingBox &b2);

    // ACCESSORS
    void Get(Vec3f &_min, Vec3f &_max) const {
        _min = min;
        _max = max; }

    Vec3f getMin() const { return min; }
    Vec3f getMax() const { return max; }

    // MODIFIERS
    void Set(BoundingBox *bb) {
        assert(bb != NULL);
        min = bb->min;
        max = bb->max; }
    
    void Set(Vec3f _min, Vec3f _max) {
        assert (_min.x() <= _max.x() &&
                _min.y() <= _max.y() &&
                _min.z() <= _max.z());
        min = _min;
        max = _max; }
    
    void Extend(const Vec3f v) {
        min = Vec3f(min2(min.x(),v.x()),
                min2(min.y(),v.y()),
                min2(min.z(),v.z()));
        max = Vec3f(max2(max.x(),v.x()),
                max2(max.y(),v.y()),
                max2(max.z(),v.z())); }
    void Extend(BoundingBox *bb) {
        assert (bb != NULL);
        Extend(bb->min);
        Extend(bb->max); }

    bool Inside(const Vec3f &pt) const {
        return (pt.x() >= min.x() && pt.x() <= max.x() &&
                pt.y() >= min.y() && pt.y() <= max.y() &&
                pt.z() >= min.z() && pt.z() <= max.z() );
    }

    bool IntersectP(const Ray &ray, float *hitt0 = NULL,
        float *hitt1 = NULL) const;

        // DEBUGGING 
    void Print() const {
        printf ("%f %f %f  -> %f %f %f\n", min.x(),min.y(),min.z(),
                max.x(),max.y(),max.z()); }
    void paint() const;
private:
    //BoundingBox() { assert(0); } // don't use this constructor
    // REPRESENTATION
    Vec3f min;
    Vec3f max;
};

// ====================================================================
// ====================================================================

#endif
