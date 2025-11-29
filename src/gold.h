#pragma once

#include <string>
#include <cstdint>

#include "config.h"
#include <iostream>

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

inline std::vector<bitseq> create_gold(const GoldParams &p)
{
    auto m1 = generate_mseq(p.len, p.taps1);
    auto m2 = generate_mseq(p.len, p.taps2);

    std::vector<bitseq> res;
    for (uint64_t i = 0; i < p.num; ++i)
        res.emplace_back(make_gold(m1, m2, i));
    return res;
}

inline std::map<std::pair<int, int>, bitseq> create_mapper(const std::vector<std::pair<int, int>> &keys, const std::vector<bitseq> &gold)
{
    assert(keys.size() == gold.size());

    std::map<std::pair<int, int>, bitseq> res;
    size_t size = keys.size();
    for (uint64_t i = 0; i < size; ++i)
        res[keys[i]] = gold[i];
    return res;
}

inline bitseq to_gold(const bitseq &src, const std::map<std::pair<int, int>, bitseq> &mapper)
{
    bitseq res;

    size_t size = src.size();

    for (size_t i = 0; i < size; i += 2)
    {
        auto &bits = mapper.at({src[i], src[i + 1]});
        res.insert(res.end(), bits.begin(), bits.end());
    }
    return res;
}