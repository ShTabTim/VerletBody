#pragma once
#include "tenzors.h"
#include "point.h"
#include "stick.h"
#include "mas.h"

template <int count>
class box {
private:
    mas<point, count* count> boxPoints;
    mas<stick, count* (count - 1) * 2 + 2 * (count - 1) * (count - 1)> boxSticks;

    float m;
public:
    void init(float m = 0.1, float k = 300, float stl = 200, float pointRad = 1, bool topIsStatic = false, float friction = 2, float bounce = 0.6) {
        this->m = m;

        foreach(boxPoints, i, {
            point & pt = boxPoints[i];
            pt.pos[0] = (i / count) * stl + 20;
            pt.pos[1] = (i % count) * stl + 200;

            pt.m = m / (count * count);

            pt.prevPos[0] = boxPoints[i].pos[0] + 2 * (rand() % 2 - 0.5);
            pt.prevPos[1] = boxPoints[i].pos[1] + 2 * (rand() % 2 - 0.5);

            pt.rad = pointRad;
            pt.mu = bounce;
            pt.friction = friction;
            });

        float kx = k * 2;
        float kplus = k;
        float fx = friction;
        float fplus = friction;

        for (int i = 0; i < count; i++)
            for (int j = 0; j < count; j++) {

                point& ptoo = boxPoints[i * count + j];
                ptoo.friction = fplus;
                if (i < count - 1) {
                    point& ptox = boxPoints[i * count + count + j];
                    ptox.friction = fplus;
                    boxSticks[(i * count + j) * 2].p1 = &ptoo;
                    boxSticks[(i * count + j) * 2].p2 = &ptox;
                    boxSticks[(i * count + j) * 2].length = stl;
                    boxSticks[(i * count + j) * 2].k = kplus;
                }
                if (j < count - 1) {
                    point& ptoy = boxPoints[i * count + j + 1];
                    ptoy.friction = fplus;
                    boxSticks[(i * count - i + j) * 2 + 1].p1 = &ptoo;
                    boxSticks[(i * count - i + j) * 2 + 1].p2 = &ptoy;
                    boxSticks[(i * count - i + j) * 2 + 1].length = stl;
                    boxSticks[(i * count - i + j) * 2 + 1].k = kplus;
                }
            }
        for (int i = 0; i < count - 1; i++)
            for (int j = 0; j < count - 1; j++) {
                stick& stvn = boxSticks[count * (count - 1) * 2 + 2 * (i * count - i + j)];
                stvn.p1 = &(boxPoints[i * count + j]);
                stvn.p2 = &(boxPoints[i * count + count + j + 1]);
                stvn.length = stl * sqrt(2);
                stvn.k = kx;

                (boxPoints[i * count + j]).friction = fx;
                (boxPoints[i * count + count + j + 1]).friction = fx;

                stick& stnv = boxSticks[count * (count - 1) * 2 + 1 + 2 * (i * count - i + j)];
                stnv.p1 = &(boxPoints[i * count + count + j]);
                stnv.p2 = &(boxPoints[i * count + j + 1]);
                stnv.length = stl * sqrt(2);
                stvn.k = kx;

                (boxPoints[i * count + count + j]).friction = fx;
                (boxPoints[i * count + j + 1]).friction = fx;
            }
        if (topIsStatic) sum(i, count, boxPoints[i * count + count - 1].isStatic = true);
    }

    point& getPtCenter() {
        return boxPoints[count * ((int)(count / 2)) + ((int)(count / 2))];
    }

    point& operator[](int i) { return boxPoints[i]; }
    mas<point, count* count> getPoints() { return boxPoints; }

    void update(float dt, vec(2) gravity, coco2(2,2) g) {
        foreach(boxSticks, i, boxSticks[i].update(dt, g););
        foreach(boxPoints, i, boxPoints[i].update(dt, gravity););
    }

    void draw(DB& db, float wldToRect, bool sticks, bool points) {
        if (sticks) foreach(boxSticks, i, boxSticks[i].draw(db, wldToRect););
        if (points) foreach(boxPoints, i, boxPoints[i].draw(db, wldToRect););
    }
};
