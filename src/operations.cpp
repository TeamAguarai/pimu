#ifdef USING_VSCODE_AS_EDITOR
    #include "operations.h"
#endif

namespace pimu {

template <typename T>
T remap(T value, T in_min, T in_max, T out_min, T out_max) {
    return out_min + (value - in_min) * (out_max - out_min) / (in_max - in_min);
}

// Redondeo para float
float round(float num, int decimals) {
    float factor = std::pow(10.0f, decimals);
    return std::round(std::round(num * (factor * 10.0f)) / 10.0f) / factor;
}

// Redondeo para double
double round(double num, int decimals) {
    double factor = std::pow(10.0, decimals);
    return std::round(std::round(num * (factor * 10.0)) / 10.0) / factor;
}

// Redondeo para enteros (No afecta valores enteros)
int round(int num, int) {
    return num;
}

}

