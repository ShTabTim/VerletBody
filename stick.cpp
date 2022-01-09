#include "stick.h"

void stick::update(float dt, coco2(2, 2) g) {
    if (p1 == 0 || p2 == 0) return;
    if (p1->isStatic && p2->isStatic) return;
    vec(2) dr;
    sum(i, 2, dr[i] = p2->pos[i] - p1->pos[i]);
    float lNow = 0;
    sum(i, 2, sum(j, 2, lNow += g[-i][-j] * dr[i] * dr[j]));
    lNow = sqrt(lNow);

    float epsilon = (length - lNow) / length;

    sum(i, 2, p1->dpdt[i] -= dr[i] / lNow * min(1, max(-1, epsilon)) * k);
    sum(i, 2, p2->dpdt[i] += dr[i] / lNow * min(1, max(-1, epsilon)) * k);
}
void stick::draw(DB& db, float wldToRect) {
    if (p1 != 0 && p2 != 0)
        db.DrawLine(p1->pos[0] * wldToRect, p1->pos[1] * wldToRect, p2->pos[0] * wldToRect, p2->pos[1] * wldToRect, { 240, 200, 20 });
}