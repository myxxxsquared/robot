#ifndef XCORR
#define XCORR

// #include <complex.h>
#include <fftw3.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

// void xcorr(fftw_complex *, fftw_complex *, fftw_complex *, int);
void xcorr(fftw_complex * signala, fftw_complex * signalb, fftw_complex * result, int N);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
