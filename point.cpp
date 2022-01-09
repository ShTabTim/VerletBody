#include "point.h"

point::point() { sum(i, 2, dpdt[i] = 0); }
point::point(float x, float y) { pos[0] = x; pos[1] = y; sum(i, 2, dpdt[i] = 0); }
void point::update(float dt, vec(2) gravity) {
    if (isStatic) return;
    vec(2) dr;
    sum(i, 2, dr[i] = max(-100, min(100, pos[i] - prevPos[i])));

    sum(i, 2, dpdt[i] += gravity[i] * m - dr[i] * friction);

    prevPos = pos;
    sum(i, 2, pos[i] += dr[i] + max(-2, min(2, dpdt[i] / m * dt * dt)));
    sum(i, 2, dpdt[i] = 0);

    if (pos[1] < rad) { dpdt[1] -= gravity[1] * m; dpdt[0] -= dr[0]/ mu;  pos[1] = rad; sum(i, 2, prevPos[i] = pos[i] + (2 * d_kron(i, 1) - 1) * dr[i]); }
}
void point::draw(DB& db, float wldToRect) {
    db.DrawCircle(pos[0] * wldToRect, pos[1] * wldToRect, rad, { 200, 240, 20 });
}