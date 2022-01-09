#pragma once
#include "tenzors.h"
#include "point.h"

typedef struct stick {
    point* p1 = 0, * p2 = 0;
    float length = 10, k = 1500;
    void update(float dt, coco2(2, 2) g);
    void draw(DB& db, float wldToRect);
}stick;