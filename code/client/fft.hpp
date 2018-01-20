
#ifndef FFT_HEADER
#define FFT_HEADER

#include <complex>
#include <valarray>

typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;
typedef std::valarray<int> IArray;

void fft(CArray &x);
void ifft(CArray& x);

#endif
