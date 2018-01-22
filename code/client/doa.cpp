
#include "audiotype.hpp"

#include "fft.hpp"
#include "doa.hpp"

#include <cmath>
#include <iostream>

const double SOUND_SPEED = 343.2;
const double MIC_DISTANCE_4 = 0.08127;
const double MAX_TDOA_4 = MIC_DISTANCE_4 / SOUND_SPEED;
const double PI = 3.14159265358979323846264338328;

double gcc_phat(const CArray &sig, const CArray &refsig, intptr_t fs, double max_tau, size_t interp)
{
    size_t n = sig.size() + refsig.size();

    CArray R;

    {
        CArray SIG;
        SIG.resize(n, 0);
        for (size_t i = 0; i < sig.size(); ++i) {
            SIG[i] = sig[i];
        }
        fft(SIG);
        CArray REFSIG;
        REFSIG.resize(n, 0);
        for (size_t i = 0; i < refsig.size(); ++i) {
            REFSIG[i] = refsig[i];
        }
        fft(REFSIG);
        R = SIG * REFSIG.apply(std::conj);
        R = R.apply([](Complex x) {return x/std::abs(x);});

        CArray tmp_R = R;
        R.resize(n*interp);
        for (size_t i = 0; i < tmp_R.size(); ++i) {
            R[i] = tmp_R[i];
        }
        ifft(R);

    }

    intptr_t max_shift = interp * n / 2;
    max_shift = std::min(max_shift, (intptr_t)(interp * fs * max_tau));

    size_t shift = 0;
    double maxabs = 0;

    for(size_t i = 0; i < max_shift; ++i)
    {
        double curabs = std::abs(R[i]);
        if(curabs > maxabs)
        {
            maxabs = curabs;
            shift = i;
        }
    }

    for(intptr_t i = 0; i < max_shift; ++i)
    {
        double curabs = std::abs(R[R.size() - i]);
        if(curabs > maxabs)
        {
            maxabs = curabs;
            shift = R.size() - i - 2*max_shift;
        }
    }

    return (double)shift / interp / fs;
}

double doa_chat(const AudioInputArray &l1, const AudioInputArray &l2)
{
    CArray c1, c2;
    c1.resize(l1.size());
    for(intptr_t i = 0; i < (intptr_t)l1.size(); ++i)
        c1[i] = l1[i];
    c2.resize(l2.size());
    for(intptr_t i = 0; i < (intptr_t)l2.size(); ++i)
        c2[i] = l2[i];

    return gcc_phat(c1, c2, INPUT_SAMPLERATE, MAX_TDOA_4, 1);
}

double doa(const AudioInputArray buffer[4])
{
    double theta0, theta1;
    theta0 = asin(doa_chat(buffer[0], buffer[2]) / MAX_TDOA_4) * 180.0 / PI;
    theta1 = asin(doa_chat(buffer[1], buffer[3]) / MAX_TDOA_4) * 180.0 / PI;

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
