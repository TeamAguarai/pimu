#include <algorithm>
#include <wiringPi.h>
#include <chrono>
#include <cmath>

namespace pimu {

// Referencia: https://stackoverflow.com/questions/8684327/c-map-number-ranges
template <typename T>
T remap(T value, T in_min, T in_max, T out_min, T out_max);

// Referencia: https://stackoverflow.com/questions/70221264/how-do-i-set-the-precision-of-a-float-variable-in-c
float round(float num, int decimals);
double round(double num, int decimals);
int round(int num, int decimals); // No afecta enteros, solo los devuelve.

} 