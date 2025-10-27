#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <random>
#include <fstream>

inline int exec(const std::string cmd)
{
    std::cerr << "exec: " << cmd << std::endl;
    return system(cmd.c_str());
}

inline double rand(double left, double right)
{
    return left + (right - left) * (double)std::rand() / (double)RAND_MAX;
}

inline std::vector<std::mt19937> create_random(uint64_t n)
{
    std::vector<std::mt19937> generators;
    std::random_device rd;
    std::seed_seq base_seed{rd(), rd(), rd(), rd()};
    std::vector<unsigned> seeds(n);
    base_seed.generate(seeds.begin(), seeds.end());
    for (int i = 0; i < n; ++i)
    {
        generators.emplace_back(seeds[i]);
    }

    return generators;
}

inline std::string random_bits(std::mt19937 &gen, const size_t &size)
{
    std::uniform_int_distribution<uint8_t> dist('0', '1');
    std::string bits;
    bits.resize(size);
    for (uint64_t i = 0; i < size; ++i)
    {
        bits[i] = dist(gen);
    }
    return bits;
}

template <typename T>
inline std::vector<double> autokeys(T &&src)
{
    std::vector<double> keys;
    keys.resize(src.size());
    for (uint64_t i = 0; i < keys.size(); ++i)
        keys[i] = i;
    return keys;
}

inline std::vector<double> ranged_keys(const size_t &steps, const double &begin, const double &end)
{
    std::vector<double> res;
    res.resize(steps);
    double step = (end - begin) / double(steps - 1);
    for (size_t i = 0; i < steps; ++i)
    {
        res[i] = begin + step * i;
    }
    return res;
}

inline uint64_t next_power_of_two(uint64_t v)
{
    if (v == 0)
        return 1;
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v |= v >> 32;
    return v + 1;
}

template <typename T>
inline void zero_extention(std::vector<T> &target, size_t size)
{
    size_t cur = target.size();
    target.reserve(size);
    for (size_t i{cur}; i < size; ++i)
        target.emplace_back(T{});
}

#define nprint_vector(src) print_vector(src, #src)
template <typename T>
inline void print_vector(const std::vector<T> &src, const std::string &path)
{
    std::ofstream file(path);
    for (auto &item : src)
        file << item << std::endl;
    file.close();
}