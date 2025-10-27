#pragma once

#include "cmplx/cmplx.h"
#include "plot/plot.h"

#define nline(keys, src) line(#src, keys, src)

typedef std::vector<GnuplotLine> GnuplotLineCmplx;
inline GnuplotLineCmplx line(const std::string &name, const std::vector<double> &keys, const std::vector<cmplx> &src)
{
    return GnuplotLineCmplx{
        {name + " (re)", merge(keys, cmplx_re(src))},
        {name + " (im)", merge(keys, cmplx_im(src))}};
}
inline GnuplotLine line(const std::string &name, const std::vector<double> &keys, const std::vector<double> &src)
{
    return GnuplotLine(name, merge(keys, src));
}