#pragma once
#include "FundLibs/ShRendCPU/DrawBuffer.h"
#include "tenzors.h"

typedef struct point {
    vec(2) pos, prevPos, dpdt;
    float m = 0.03, mu = 0.6, friction = 10;
    bool isStatic = false;
    float rad = 10;
    point();
    point(float x, float y);
    void update(float dt, vec(2) gravity);
    void draw(DB& db, float wldToRect);
}point;