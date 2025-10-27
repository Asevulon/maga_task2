#pragma once

#include <vector>
#include <string>

#include "cmplx/cmplx.h"
#include "config.h"

class Fm4Params
{
public:
    Fm4Params() {}
    Fm4Params(const Config &c)
    {
        bits = c["bits"];
        if (bits.size() % 2)
        {
            bits += "0";
            std::cout << "Предупреждение: нечетное количество бит в FM4. Добавлен один дополнительный бит." << std::endl;
        }
        Tb = c["Tb"];
        fs = c["fs"];
    }

    std::string bits;
    double Tb = 0;
    double fs = 0;
};

inline std::vector<cmplx> &
apply_fm4(
    std::vector<cmplx> &target,
    const std::string &bits,
    double Tb,
    double fs)
{
    size_t size_per_bit = fs / Tb;
    size_t size = size_per_bit * bits.size() / 2;

    if (target.size() < size)
        target.resize(size);

    for (size_t i = 0; i < bits.size(); i += 2)
    {
        double A1 = bits[i] - '0';
        double A2 = bits[i + 1] - '0';
        size_t offset = i * size_per_bit / 2;
        for (size_t j = 0; j < size_per_bit; ++j)
            target[j + offset] = cmplx(A1, A2);
    }

    if (target.size() > size)
        for (size_t i = size; i < target.size(); ++i)
            target[i] = cmplx{};
    return target;
}

inline std::vector<cmplx> generate_fm4(
    const std::string &bits,
    double Tb,
    double fs)
{
    std::vector<cmplx> res(bits.size() * fs / Tb);
    return apply_fm4(res, bits, Tb, fs);
}

inline std::vector<cmplx> generate_fm4(const Fm4Params &p)
{
    return generate_fm4(p.bits, p.Tb, p.fs);
}

inline std::vector<double> generate_keys(
    const std::string &bits,
    double Tb,
    double fs)
{
    size_t size = bits.size() * fs / Tb / 2;
    std::vector<double> res(size);
    for (size_t i = 0; i < size; ++i)
        res[i] = i / fs;
    return res;
}
inline auto generate_keys(const Fm4Params &p) { return generate_keys(p.bits, p.Tb, p.fs); }