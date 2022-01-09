#pragma once

#define sum(index, maxIndex, operation) for (int index = 0; index < maxIndex; index++) operation

#define vec(dem) bra<float, (dem)>
#define covec(dem) ket<float, (-dem)>
#define coco2(dem1, dem2) ket<ket<float, (-dem1)>, (-dem2)>
#define d_kron(i, j) ((i == j) ? 1 : 0)

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
