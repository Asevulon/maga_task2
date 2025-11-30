#pragma once
#include <cmath>
#include <vector>
#include <cstdint>

class cmplx
{
public:
    double re;
    double im;

    cmplx() { re = im = 0.; }
    cmplx(const cmplx &src)
    {
        re = src.re;
        im = src.im;
    }
    cmplx(double re_, double im_)
    {
        re = re_;
        im = im_;
    }
    cmplx &operator=(const cmplx &c)
    {
        re = c.re;
        im = c.im;
        return *this;
    }
    cmplx &operator=(const cmplx &&c)
    {
        re = c.re;
        im = c.im;
        return *this;
    }
};

inline cmplx operator*(const cmplx &x, const cmplx &y)
{
    return cmplx(x.re * y.re - x.im * y.im,
                 x.re * y.im + y.re * x.im);
}
inline cmplx operator*(const cmplx &x, const double y)
{
    return cmplx(x.re * y, x.im * y);
}
inline cmplx operator/(const cmplx &x, const cmplx &y)
{
    cmplx z;
    double y2 = y.re * y.re + y.im * y.im;
    if (y2 < 10e-40)
        return z;
    z.re = (x.re * y.re + x.im * y.im) / y2;
    z.im = (y.re * x.im - x.re * y.im) / y2;
    return z;
}
inline cmplx operator/(const cmplx &x, double y)
{
    cmplx z;
    if (y < 10e-40)
        return z;
    z.re = x.re / y;
    z.im = x.im / y;
    return z;
}
inline cmplx operator+(const cmplx &x, const cmplx &y)
{
    cmplx z;
    z.re = x.re + y.re;
    z.im = x.im + y.im;
    return z;
}
inline cmplx operator-(const cmplx &x, const cmplx &y)
{
    cmplx z;
    z.re = x.re - y.re;
    z.im = x.im - y.im;
    return z;
}
inline double abs(const cmplx &x)
{
    return sqrt(x.re * x.re + x.im * x.im);
}
inline cmplx conj(const cmplx &x)
{
    return cmplx(x.re, -x.im);
}

void fourea(std::vector<cmplx> &F, double is);

template <template <typename...> typename container_type>
container_type<double> cmplx_re(const container_type<cmplx> &src)
{
    container_type<double> res;
    for (auto &item : src)
        res.emplace_back(item.re);
    return res;
}
template <template <typename...> typename container_type>
container_type<double> cmplx_im(const container_type<cmplx> &src)
{
    container_type<double> res;
    for (auto &item : src)
        res.emplace_back(item.im);
    return res;
}

inline std::vector<cmplx> merge_cmplx(const std::vector<double> &re, const std::vector<double> &im)
{
    std::vector<cmplx> res;
    size_t size = std::min(re.size(), im.size());
    res.reserve(size);
    for (uint64_t i = 0; i < size; ++i)
        res.emplace_back(re[i], im[i]);
    return res;
}

inline double energy_cmplx(const std::vector<cmplx> &target)
{
    double res = 0;
    for (const auto &x : target)
        res += x.re * x.re + x.im * x.im;
    return res;
}

inline std::vector<double> abs(const std::vector<cmplx> &src)
{
    std::vector<double> res;
    res.reserve(src.size());
    for (const auto &x : src)
        res.emplace_back(abs(x));
    return res;
}
inline void apply_abs(const std::vector<cmplx> &src, std::vector<double> &res)
{
    size_t size = src.size();
    if (res.size() < size)
        res.resize(size);
    for (size_t i = 0; i < size; ++i)
    {
        res[i] = abs(src[i]);
    }
}

inline void correlation_fft(std::vector<cmplx> &x, std::vector<cmplx> &y, std::vector<cmplx> &r)
{
    fourea(x, -1);
    fourea(y, -1);
    size_t size = x.size();
    for (size_t i = 0; i < size; ++i)
    {
        r[i] = x[i] * conj(y[i]);
    }
    fourea(r, 1);
}