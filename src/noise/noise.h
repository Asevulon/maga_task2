#pragma once

#include <vector>

#include "general/general.h"
#include "cmplx/cmplx.h"
#include "modulation.h"

class NoiseParams : public SourceParams
{
public:
    NoiseParams() {}
    NoiseParams(const Config &c) : SourceParams(c) {}
    NoiseParams(const SourceParams &s) : SourceParams(s) {}
};

std::vector<double> generate_white_noise(size_t size);
std::vector<cmplx> apply_white_noise(std::vector<cmplx> &target, const NoiseParams &p);
void apply_white_noise_raw(std::vector<cmplx> &target, const double &noise, const size_t &size);

inline double single_white_noise()
{
    double res = 0;
    for (int i = 0; i < 12; i++)
        res += rand(-1, 1);
    res /= 12;
    return res;
}

inline double convert_db(double val)
{
    return pow(10, val / 10);
}
