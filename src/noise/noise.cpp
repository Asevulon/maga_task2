#include "noise/noise.h"

#include <cstdint>

std::vector<double> generate_white_noise(size_t size)
{
    std::vector<double> res;
    res.reserve(size);
    for (uint64_t i = 0; i < size; ++i)
    {
        res.emplace_back(single_white_noise());
    }
    return res;
}

std::vector<cmplx> apply_white_noise(std::vector<cmplx> &target, const NoiseParams &p)
{
    auto size = target.size();
    auto n_re = generate_white_noise(size);
    auto n_im = generate_white_noise(size);
    auto n = merge_cmplx(n_re, n_im);
    auto en = energy_cmplx(n);
    auto es = energy_cmplx(target);
    double noise = 1. / convert_db(p.noise);
    double betta = sqrt(noise * es / en);
    std::vector<cmplx> res;
    for (uint64_t i = 0; i < size; ++i)
        res.emplace_back(target[i] + (n[i] * betta));
    return res;
}

void apply_white_noise_raw(std::vector<cmplx> &target, const double &pn, const size_t &size)
{
    std::vector<cmplx> n;
    n.reserve(size);
    for (size_t i = 0; i < size; ++i)
    {
        n.emplace_back(single_white_noise(), single_white_noise());
    }
    auto en = energy_cmplx(n);
    auto es = energy_cmplx(target);
    double noise = 1. / convert_db(pn);
    double betta = sqrt(noise * es / en);
    for (size_t i = 0; i < size; ++i)
        target[i] = target[i] + n[i] * betta;
}