#include <Windows.h>
#include "FundLibs/ShRendCPU/ShWin.h"
#include <vector>

#define ScrWW 700
#define ScrHH 700
#define wldToRect 0.5

#define sum(index, maxIndex, operation) for (int index = 0; index < maxIndex; index++) operation
template<typename T, int dem>
class bra {
private:
    T x[dem];
public:
    bra<T, dem>() {}
    bra<T, dem>(T el) { sum(i, dem, x[i] = el); }

    T& operator[](int num) { return x[num]; }
};
template<typename T, int dem>
class ket {
private:
    T _x[-dem];
public:
    ket<T, dem>() {}
    ket<T, dem>(T el) { sum(i, dem, _x[i] = el); }

    T& operator[](int num) { return _x[-num]; }
};

#define vec(dem) bra<float, (dem)>
#define covec(dem) ket<float, (-dem)>
#define coco2(dem1, dem2) ket<ket<float, (-dem1)>, (-dem2)>
#define d_kron(i, j) ((i == j) ? 1 : 0)

float bounce = 0.1;
float friction = 10;
vec(2) gravity;
ket<ket<float, -2>, -2> g;

typedef struct point {
    vec(2) pos, prevPos, dpdt;
    float m = 0.03;
    bool isStatic = false;
    point() { sum(i, 2, dpdt[i] = 0); }
    point(float x, float y) { pos[0] = x; pos[1] = y; sum(i, 2, dpdt[i] = 0); }
    void update(float dt) {
        if (isStatic) return;
        vec(2) v;
        sum(i, 2, v[i] = max(-100, min(100, pos[i] - prevPos[i])) );

        sum(i, 2, dpdt[i] += gravity[i] * m - v[i]*friction);

        prevPos = pos;
        sum(i, 2, pos[i] += v[i] + max(-1, min(1, dpdt[i] / m * dt * dt)));
        sum(i, 2, dpdt[i] = 0);

        if (pos[1] < 0) { dpdt[1] -= gravity[1] * m + v[0]*bounce;  pos[1] = 0; prevPos = pos; }
    }
    void draw(DB& db) {
        db.DrawCircle(pos[0]*wldToRect, pos[1]*wldToRect, 2, { 200, 240, 20 });
    }
}point;
typedef struct stick {
    point *p1 = 0, *p2 = 0;
    float length = 10, k = 1000;
    void update(float dt) {
        if (p1 == 0 || p2 == 0) return;
        if (p1->isStatic && p2->isStatic) return;
        vec(2) dr;
        sum(i, 2, dr[i] = p2->pos[i] - p1->pos[i]);
        float lNow = 0;
        sum(i, 2, sum(j, 2, lNow += g[-i][-j]*dr[i]*dr[j] ));
        lNow = sqrt(lNow);

        float epsilon = (length - lNow) / length;

        if (!p1->isStatic) sum(i, 2, p1->dpdt[i] -= dr[i] / lNow * max(-1, min(1, epsilon)) * k);
        if (!p2->isStatic) sum(i, 2, p2->dpdt[i] += dr[i] / lNow * max(-1, min(1, epsilon)) * k);
    }
    void draw(DB& db) {
        if (p1 != 0 && p2 != 0) 
            db.DrawLine(p1->pos[0]*wldToRect, p1->pos[1]*wldToRect, p2->pos[0]*wldToRect, p2->pos[1]*wldToRect, { 200, 240, 20 });
    }
}stick;
template <typename T, size_t amount>
class mas {
private:
    T x[amount];
public:
    size_t length() { return amount; }
    T& operator[](size_t num) { return x[num]; }
};
#define foreach(arr, index, doing) for(size_t index = 0; index < arr.length(); index++) doing

//void createBox(point& p1, )
float h = 40;
#define nns 11
class MainRenderer : public ShWinRendCPU {
    mas<point, nns*nns> ragPoints;
    mas<stick, nns*(nns-1)*2 + 2*(nns-1)*(nns-1)> rag;

    bool initSim() {
        AppName = L"VB";
        gravity[0] = 0;
        gravity[1] = -1000;
        foreach(ragPoints, i,{
            point &pt = ragPoints[i];
            pt.pos[0] = (i/nns)*h + 20;
            pt.pos[1] = (i%nns)*h + 200;

            pt.m = 0.3f/nns;

            pt.prevPos[0] = ragPoints[i].pos[0] + 2*(rand()%2-0.5);
            pt.prevPos[1] = ragPoints[i].pos[1] + 2*(rand()%2-0.5);
        });

        for (int i = 0; i < nns; i++)
            for(int j = 0; j < nns; j++) {

                point &ptoo = ragPoints[i* nns + j];

                if (i < nns-1) {
                    point& ptox = ragPoints[i * nns + nns + j];
                    rag[(i * nns + j) * 2].p1 = &ptoo;
                    rag[(i * nns + j) * 2].p2 = &ptox;
                    rag[(i * nns + j) * 2].length = h;
                }
                if (j < nns-1) {
                    point& ptoy = ragPoints[i * nns + j + 1];
                    rag[(i * nns - i + j) * 2 + 1].p1 = &ptoo;
                    rag[(i * nns - i + j) * 2 + 1].p2 = &ptoy;
                    rag[(i * nns - i + j) * 2 + 1].length = h;
                }
            }
        for (int i = 0; i < nns-1; i++)
            for (int j = 0; j < nns-1; j++) {
                stick& stvn = rag[nns *(nns -1)*2 + 2*(i*nns-i + j)];
                stvn.p1 = &(ragPoints[i * nns + j]);
                stvn.p2 = &(ragPoints[i * nns + nns + j + 1]);
                stvn.length = h * sqrt(2);
                
                stick& stnv = rag[nns *(nns -1)*2 +1  + 2*(i* nns-i + j)];
                stnv.p1 = &(ragPoints[i * nns + nns + j]);
                stnv.p2 = &(ragPoints[i * nns + j + 1]);
                stnv.length = h * sqrt(2);
            }
        //rag[rand() % rag.length()].p1 = 0;
        //rag[rand() % rag.length()].p1 = 0;
        //rag[rand() % rag.length()].p1 = 0;
        //rag[rand() % rag.length()].p1 = 0;
        //rag[rand() % rag.length()].p1 = 0;
        //rag[rand() % rag.length()].p1 = 0;
        //sum(i, nns, ragPoints[i* nns + nns-1].isStatic = true);

        sum(i, 2, sum(j, 2, g[-i][-j] = d_kron(i, j) ));
        return true;
    }
    bool loopSim(float dt) {
        db.Fill(0, 0, db.GetW(), db.GetH(), { 30, 57, 48 });

        if (GetKey(VK_LBUTTON).bHeld) {
            point& pt = ragPoints[nns*((int)(nns/2))+((int)(nns/2))];// rand() % ragPoints.length()];

            vec(2) mpow;
            mpow[0] = pt.pos[0] - GetMouseX() / wldToRect;
            mpow[1] = pt.pos[1] - GetMouseY() / wldToRect;

            sum(i, 2, mpow[i] = mpow[i] * mpow[i] * mpow[i]);

            sum(i, 2, pt.dpdt[i] -= mpow[i]);
        }
        foreach(rag, i, rag[i].update(dt);)
        foreach(ragPoints, i, ragPoints[i].update(dt);)
        foreach(rag, i, rag[i].draw(db);)
        foreach(ragPoints, i, ragPoints[i].draw(db);)

        return true;
    }
    bool finitSim() {
        return true;
    }
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) {
    if (uiMsg == WM_CLOSE)
        PostQuitMessage(0);
    else
        return DefWindowProc(hWnd, uiMsg, wParam, lParam);
    return 0;
}
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR sCmdLine, int iShow) {
    MainRenderer simulation;
    if (simulation.init(hInst, ScrWW, ScrHH, WndProc))
        simulation.Start();
    return 0;
}