#pragma once

#define foreach(arr, index, doing) for(size_t index = 0; index < arr.length(); index++) doing

template <typename T, size_t amount>
class mas {
private:
    T x[amount];
public:
    size_t length() { return amount; }
    T& operator[](size_t num) { return x[num]; }
};