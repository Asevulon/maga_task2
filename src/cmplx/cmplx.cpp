#include "cmplx/cmplx.h"
#include "general/const.h"

void fourea(std::vector<cmplx> &F, double is)
{
    cmplx temp, w, c;
    long i, i1, j, j1, istep;
    long m, mmax;
    long n = F.size();
    double fn, r1, theta;
    fn = (double)n;
    double r = Pi * is;
    j = 1;
    for (i = 1; i <= n; i++)
    {
        i1 = i - 1;
        if (i < j)
        {
            j1 = j - 1;
            temp = F[j1];
            F[j1] = F[i1];
            F[i1] = temp;
        }
        m = n / 2;
        while (j > m)
        {
            j -= m;
            m = (m + 1) / 2;
        }
        j += m;
    }
    mmax = 1;
    while (mmax < n)
    {
        istep = 2 * mmax;
        r1 = r / (double)mmax;
        for (m = 1; m <= mmax; m++)
        {
            theta = r1 * (double)(m - 1);
            w = cmplx(cos(theta), sin(theta));
            for (i = m - 1; i < n; i += istep)
            {
                j = i + mmax;
                c = F[j];
                temp = w * c;
                F[j] = F[i] - temp;
                F[i] = F[i] + temp;
            }
        }
        mmax = istep;
    }
    if (is > 0)
        for (i = 0; i < n; i++)
        {
            F[i].re /= fn;
            F[i].im /= fn;
        }
    return;
}