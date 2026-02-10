// === defs.h ===
#pragma once

const int WIDTH = 1200;
const int HEIGHT = 1000;
const float MAXSPEED = 20.0;

const int histogramHeight = 100;
const int binCount = 80;

template<typename T>
const T& clamp(const T& v, const T& lo, const T& hi) {
    return (v < lo) ? lo : (hi < v) ? hi : v;
}