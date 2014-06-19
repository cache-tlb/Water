#pragma once
#include "StdHeader.h"
#include "vec3.h"
#include "GLHeader.h"

struct HitTest {
    HitTest (float _t = FLT_MAX, const Vector &_hit = Vector(), const Vector &_normal = Vector()): t(_t), hit(_hit), normal(_normal) {}

    void mergeWith(const HitTest &other){
        if (other.t > 0 && other.t < t) {
            t = other.t;
            hit = other.hit;
            normal = other.normal;
        }
    }

    float t;
    Vector hit, normal;
};

class RayTracer {
public:
    RayTracer();

    Vector getRayForPixel(float x, float y);

    //static bool hitTestBox(const Vector &origin, const Vector &ray, const Vector &min, const Vector &max, HitTest &result);
    // only hitTestSphere is used, for picking the sphere
    static bool hitTestSphere(const Vector &origin, const Vector &ray, const Vector &center, const float radius, HitTest &result);
    //static bool hitTestTriangle(const Vector &origin, const Vector &ray, const Vector &a, const Vector &b, const Vector &c, HitTest &result);

public:
    Vector ray00, ray10, ray01, ray11, eye;
    GLint viewport[4];
};

