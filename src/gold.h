#pragma once

#include <algorithm>
#include <iostream>
#include <string>
#include <cstdint>

#include "config.h"
#include "cmplx/cmplx.h"
#include "modulation.h"

using bitseq = std::vector<int>;

class GoldParams
{
public:
    GoldParams() {}
    GoldParams(const Config &c)
    {
        auto g = c["Коды Голда"];
        len = g["Длина регистра"];
        taps1 = g["Тапы первой последовательности"].get<std::vector<int>>();
        taps2 = g["Тапы второй последовательности"].get<std::vector<int>>();
        num = g["Количество кодов Голда"].get<uint64_t>();
    }

    int len = 0;
    std::vector<int> taps1;
    std::vector<int> taps2;
    uint64_t num;
};

struct GoldHandler
{
    bitseq gold;
    std::vector<cmplx> filter;
    std::vector<double> r;
    GoldHandler() {}
    GoldHandler(const bitseq &gold, const std::vector<cmplx> &filter)
        : gold(gold), filter(filter) {}
};

typedef std::pair<int, int> BitPair;
typedef std::pair<BitPair, GoldHandler> GoldMapperPair;

inline bitseq generate_mseq(int n, std::vector<int> taps, uint32_t seed = 1)
{
    if (seed == 0)
        seed = 1;
    size_t L = (1ULL << n) - 1;
    bitseq seq;
    uint32_t state = seed & ((1U << n) - 1);

    for (size_t i = 0; i < L; ++i)
    {
        int out = state & 1;
        seq.push_back(out);

        int fb = 0;
        for (int t : taps)
            fb ^= (state >> t) & 1;

        state = (state >> 1) | (static_cast<uint32_t>(fb) << (n - 1));
    }
    return seq;
}

inline bitseq make_gold(const bitseq &a, const bitseq &b, size_t tau)
{
    size_t L = a.size();
    bitseq b_shift;
    b_shift.resize(L);
    for (size_t i = 0; i < L; ++i)
        b_shift[i] = b[(i + tau) % L];

    bitseq gold;
    gold.resize(L);
    for (size_t i = 0; i < L; ++i)
        gold[i] = a[i] ^ b_shift[i];
    return gold;
}

inline std::vector<GoldHandler> create_gold(const GoldParams &p)
{
    auto m1 = generate_mseq(p.len, p.taps1);
    auto m2 = generate_mseq(p.len, p.taps2);

    std::vector<GoldHandler> res;
    for (uint64_t i = 0; i < p.num; ++i)
    {
        res.emplace_back(make_gold(m1, m2, i), std::vector<cmplx>());
        for (auto &g : res.back().gold)
            if (!g)
                g = -1;
        res.back().gold.pop_back();
    }

    return res;
}

inline void create_filter(GoldHandler &h, SourceParams p)
{
    p.bits = h.gold;
    auto fm4 = generate_fm4(p);
    // for (auto &x : fm4)
    //     x = conj(x);
    h.filter.clear();
    // h.filter.insert(h.filter.end(), fm4.rbegin(), fm4.rend());
    h.filter.insert(h.filter.end(), fm4.begin(), fm4.end());
}

inline std::map<BitPair, GoldHandler> create_mapper(
    const GoldParams &gp,
    const SourceParams &sp,
    const std::vector<BitPair> &keys)
{
    auto hs = create_gold(gp);
    for (auto &h : hs)
        create_filter(h, sp);

    std::map<BitPair, GoldHandler> res;
    for (size_t i = 0; i < keys.size(); ++i)
        res[keys[i]] = hs[i];
    return res;
}

inline bitseq to_gold(const bitseq &src, const std::map<BitPair, GoldHandler> &mapper)
{
    bitseq res;

    size_t size = src.size();

    for (size_t i = 0; i < size; i += 2)
    {
        auto &bits = mapper.at({src[i], src[i + 1]}).gold;
        res.insert(res.end(), bits.begin(), bits.end());
    }
    return res;
}

inline void correlate_gold(
    const std::vector<cmplx> &_s,
    std::map<BitPair, GoldHandler> &gm)
{
    for (auto &g : gm)
    {
        auto s = _s;
        auto f = g.second.filter;
        size_t size = std::max(_s.size(), f.size());
        size = next_power_of_two(size);
        std::vector<cmplx> r(size);
        zero_extention(s, size);
        zero_extention(f, size);
        correlation_fft(s, f, r);
        g.second.r = abs(r);
    }
}

inline bitseq find_bits(const std::map<BitPair, GoldHandler> &m, const SourceParams &p, size_t real_size)
{
    // bits per sample
    size_t bps = (*m.begin()).second.gold.size() * size_t(p.fs / p.Tb) / 2;
    bitseq res;
    for (size_t i = 0; i < real_size; i += bps)
    {
        double max = 0;
        BitPair max_key;
        for (const auto &[key, val] : m)
        {
            double local_max = *std::max_element(
                val.r.begin() + i, val.r.begin() + i + bps);
            if (local_max > max)
            {
                max = local_max;
                max_key = key;
            }
        }

        res.emplace_back(max_key.first);
        res.emplace_back(max_key.second);
    }
    return res;
}