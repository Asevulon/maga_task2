#pragma once

#include <vector>

#include "cmplx/cmplx.h"
#include "config.h"
#include "general/general.h"
#include "gold.h"

class SourceParams
{
public:
    double Tb = 0;
    double fs = 0;
    double noise = 0;
    bitseq bits;

    SourceParams()
    {
    }
    SourceParams(const Config &c)
    {
        auto s = c["Исходный сигнал"];
        Tb = s["Скорость передачи данных, бит/с"];
        fs = 1000. * s["Частота дискретизации сигнала, кГц"].get<double>();
        noise = s["Отношение сигнал/шум, дБ"];

        auto rnd = create_random(1);
        bits = random_bits(rnd.front(), s["Число бит"]);
    }
};

inline std::vector<cmplx> &
apply_fm4(
    std::vector<cmplx> &target,
    const bitseq &bits,
    double Tb,
    double fs)
{
    size_t size_per_bit = fs / Tb;
    size_t size = size_per_bit * bits.size() / 2;

    if (target.size() < size)
        target.resize(size);

    for (size_t i = 0; i < bits.size(); i += 2)
    {
        double A1 = bits[i];
        double A2 = bits[i + 1];
        size_t offset = (i / 2) * size_per_bit;
        for (size_t j = 0; j < size_per_bit; ++j)
            target[j + offset] = cmplx(A1, A2);
    }

    if (target.size() > size)
        for (size_t i = size; i < target.size(); ++i)
            target[i] = cmplx{};
    return target;
}

inline std::vector<cmplx> generate_fm4(
    const bitseq &bits,
    double Tb,
    double fs)
{
    std::vector<cmplx> res(bits.size() * fs / Tb / 2);
    return apply_fm4(res, bits, Tb, fs);
}

inline std::vector<cmplx> generate_fm4(const SourceParams &p)
{
    return generate_fm4(p.bits, p.Tb, p.fs);
}

inline std::vector<double> generate_keys(
    const bitseq &bits,
    double Tb,
    double fs)
{
    size_t size = bits.size() * fs / Tb / 2;
    std::vector<double> res(size);
    for (size_t i = 0; i < size; ++i)
        res[i] = i / fs;
    return res;
}
inline auto generate_keys(const SourceParams &p)
{
    return generate_keys(p.bits, p.Tb, p.fs);
}