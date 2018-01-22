
#include "audiotype.hpp"

#include "fft.hpp"
#include "doa.hpp"


#include "xcorr.h"

#include <cmath>

const double SOUND_SPEED = 343.2;
const double MIC_DISTANCE_4 = 0.08127;
const double MAX_TDOA_4 = MIC_DISTANCE_4 / SOUND_SPEED;
const double PI = 3.14159265358979323846264338328;

// extern "C"
// {
// double max_gcc_chat(const double *a1, const double *a2, int N);
// }

double gcc_phat(const CArray &sig, const CArray &refsig, intptr_t fs, double max_tau, size_t interp)
{
    intptr_t n = sig.size() + refsig.size();

    CArray R;

    {
        CArray SIG;
        SIG.resize(n);
        for(int i = 0; i < sig.size(); ++i)
            SIG[i] = sig[i];
        for(int i = sig.size(); i < SIG.size(); ++i)
            SIG[i] = 0;
        fft(SIG);

        CArray REFSIG;
        REFSIG.resize(n);
        for(int i = 0; i < sig.size(); ++i)
            REFSIG[i] = refsig[i];
        for(int i = sig.size(); i < SIG.size(); ++i)
            REFSIG[i] = 0;
        fft(REFSIG);

        R = SIG * REFSIG.apply(std::conj);

        // R = R.apply([](Complex x) {return x/(std::abs(x)+1.0E-15);});
        // for(int i = 0; i < 10; ++i)
            // printf("%lf+%lfi,", std::abs(R[i].real()), std::abs(R[i].imag()));
        // R.resize(n*interp);
        ifft(R);
    }

    intptr_t max_shift = interp * n / 2;
    max_shift = std::min(max_shift, (intptr_t)(interp * fs * max_tau));

    intptr_t shift = 0;
    double maxabs = 0;

    for(intptr_t i = 0; i <= max_shift; ++i)
    {
        printf("%lf,", std::abs(R[i]));
        double curabs = std::abs(R[i]);
        if(curabs > maxabs)
        {
            maxabs = curabs;
            shift = i;
        }
    }

    for(intptr_t i = R.size() - max_shift; i < R.size(); ++i)
    {
        printf("%lf,", std::abs(R[i]));
        double curabs = std::abs(R[i]);
        if(curabs > maxabs)
        {
            maxabs = curabs;
            shift = i - R.size();
        }
    }

    printf("shift: %d\n", shift);

    return (double)shift / interp / fs;
}

double doa_chat(const AudioInputArray &l1, const AudioInputArray &l2)
{
    int n = (int)(log2(l1.size()));
    n = (int)pow(2, n);
    CArray c1, c2;
    c1.resize(n);
    for(intptr_t i = 0; i < (intptr_t)n; ++i)
        if(i < l1.size())
            c1[i] = l1[i];
        else
            c1[i] = 0;
    c2.resize(n);
    for(intptr_t i = 0; i < (intptr_t)n; ++i)
        if(i < l1.size())
            c2[i] = l2[i];
        else
            c2[i] = 0;

    return gcc_phat(c1, c2, INPUT_SAMPLERATE, MAX_TDOA_4, 1);
}



double doa(const AudioInputArray buffer[4])
{
    double theta0, theta1;
    double t1 = doa_chat(buffer[0], buffer[2]);
    double t2 = doa_chat(buffer[1], buffer[3]);
    intptr_t N = buffer[0].size();
    std::vector<double> tmp1, tmp2;
    tmp1.resize(N);
    tmp2.resize(N);

    // for(intptr_t i = 0; i < N; ++i)
    // {
    //     tmp1[i] = buffer[0][i];
    //     tmp2[i] = buffer[2][i];
    // }
    // double t1 = max_gcc_chat(tmp1.data(), tmp2.data(), N);

    // for(intptr_t i = 0; i < N; ++i)
    // {
    //     tmp1[i] = buffer[1][i];
    //     tmp2[i] = buffer[3][i];
    // }
    // double t2 = max_gcc_chat(tmp1.data(), tmp2.data(), N);

    theta0 = asin(t1 / MAX_TDOA_4) * 180.0 / PI;
    theta1 = asin(t2 / MAX_TDOA_4) * 180.0 / PI;

    // printf("%lf, %lf\n", t1, t2);

    double best_guess;

    if(std::abs(theta0) < std::abs(theta1))
    {
        if(theta1>0)
            best_guess = theta0 > 0 ? theta0 : theta0 + 360;
        else
            best_guess = 180 - theta0;
    }
    else
    {
        if(theta0<0)
            best_guess = theta1 > 0 ? theta1 : theta1 + 360;
        else
            best_guess = 180 - theta1;
    }

    best_guess = 120 - best_guess;
    best_guess = best_guess > 0 ? best_guess : best_guess + 360;

    return best_guess;
}
