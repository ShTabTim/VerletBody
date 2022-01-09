#include <Windows.h>
#include "FundLibs/ShRendCPU/ShWin.h"
#include "tenzors.h"
#include "point.h"
#include "stick.h"
#include "box.h"

#define ScrWW 700
#define ScrHH 700
#define wldToRect 0.5

float bounce = 0.8;
float friction = 2;
float lll = 200;
vec(2) gravity;
ket<ket<float, -2>, -2> g;

class MainRenderer : public ShWinRendCPU {
    box<5> b;
    mas<point, 20> points;
    mas<stick, 20> sticks;
    float nuRT = 1;

    vec(2) centOfPoints() {
        vec(2) center(0);
        foreach(points, i, {
            sum(j, 2, center[j] += points[i].pos[j] / points.length() );
        });
        return center;
    }

    float volTri(vec(2) p0, vec(2) p1, vec(2) p2) {

        sum(i, 2, p0[i] = abs(p0[i])/1000 );
        sum(i, 2, p1[i] = abs(p1[i])/1000 );
        sum(i, 2, p2[i] = abs(p2[i])/1000 );

        float xmax = max(max(p0[0], p1[0]), p2[0]);
        float ymax = max(max(p0[1], p1[1]), p2[1]);

        float vol = xmax * ymax;

        float S0 = 0.5 * p0[0] * p0[1];
        float S1 = 0.5 * p1[0] * p1[1];
        float S2 = 0.5 * p2[0] * p2[1];

        //wchar_t s[64];
        //swprintf_s(s, 64, L"%10.2f, \n%10.2f, \n%10.2f, \n%10.2f, \n\n%10.2f", vol, S0, S1, S2, (vol - S0 - S1 - S2));
        //MessageBox(NULL, s, L"sdkfg", MB_OK);

        vol -= S0 + S1 + S2;

        return vol;
    }

    float volumeOfPoints() {
        vec(2) p0, p1, pn;
        float volume = 0;
        p0 = points[0].pos;
        for (int i = 2; i < points.length(); i++) {
            pn = points[i  ].pos;
            p1 = points[i-1].pos;
            volume += volTri(p0, p1, pn);
        }
        return volume;
    }

    void upd(float dt) {
        float p = nuRT / volumeOfPoints();

        vec(2) center = centOfPoints();

        vec(2) n(0);
        foreach(points, i, {

            float ll = 0;
            sum(j, 2, n[j] = (points[i% points.length()].pos[j]+points[(i+1)% points.length()].pos[j]+points[(i+points.length()-1)% points.length()].pos[j])/3 - center[j]);
            sum(j, 2, sum(k, 2, ll += g[-j][-k] * n[j] * n[k]));
            ll = sqrt(ll);

            sum(j ,2, points[i].dpdt[j] += p * n[j] / ll );
        });
    }

    bool initSim() {
        AppName = L"VB";
        gravity[0] = 0;
        gravity[1] = -1000;
        sum(i, 2, sum(j, 2, g[-i][-j] = d_kron(i, j) ));

        b.init(0.5, 1000, 100, 1, false, 7, 0.8);
        //cen.pos = vec(2)(300);
        //cen.prevPos = vec(2)(300);
        //cen.friction = 0.1;
        //cen.mu = 0.1;
        for (int i = 0; i < points.length(); i++) {

            points[i].pos[1] = lll/2/sin(3.1415/points.length())*cos(2*3.1415*i/points.length()) + 300;
            points[i].pos[0] = lll/2/sin(3.1415/points.length())*sin(2*3.1415*i/ points.length()) + 300;

            points[i].prevPos[0] = points[i].pos[0];
            points[i].prevPos[1] = points[i].pos[1];

            points[i].friction = 0.1;
            points[i].mu = 0.1;

            sticks[i].p1 = &(points[i]);
            sticks[i].p2 = &(points[(i+1)%points.length()]);
            sticks[i].k = 5000;
            sticks[i].length = lll;

            //cents[i].p1 = &(points[i]);
            //cents[i].p2 = &cen;
            //sticks[i].k = 2000;
            //sticks[i].length = lll/2/sin(3.1415/8);
        }
        return true;
    }
    bool loopSim(float dt) {
        db.Fill(0, 0, db.GetW(), db.GetH(), { 30, 57, 48 });

        if (GetKey(VK_LBUTTON).bHeld) {
            point& pt = b.getPtCenter();
        
            vec(2) mpow;
            mpow[0] = pt.pos[0] - GetMouseX() / wldToRect;
            mpow[1] = pt.pos[1] - GetMouseY() / wldToRect;
            //vec(2) cent = centOfPoints();
            //mpow[0] = cent[0] - GetMouseX() / wldToRect;
            //mpow[1] = cent[1] - GetMouseY() / wldToRect;
        
            //float len = 0;
            //sum(i, 2, sum(j, 2, len += g[-i][-j]*mpow[i]*mpow[j]));
        
            //sum(i, 2, mpow[i] =  1000*mpow[i]/sqrt(len));
        
            //sum(i, 2, points[0].dpdt[i] -= mpow[i]);
            //foreach(points, j, sum(i, 2, points[j].dpdt[i] -= mpow[i]));
            foreach(b.getPoints(), j, sum(i, 2, b[j].dpdt[i] -= mpow[i]));
        }
        b.update(dt, gravity, g);
        b.draw(db, wldToRect, true, false);
        //upd(dt);
        //foreach(sticks, i, sticks[i].update(dt, g););
        //foreach(cents, i, cents[i].update(dt, g););
        //foreach(points, i, points[i].update(dt, gravity););
        //cen.update(dt, gravity);

        //foreach(sticks, i, sticks[i].draw(db, wldToRect););
        //foreach(cents, i, cents[i].draw(db, wldToRect););
        //foreach(points, i, points[i].draw(db, wldToRect););
        //cen.draw(db, wldToRect);

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