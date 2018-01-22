#include "xcorr.h"
#include <string.h>

void xcorr(fftw_complex * signala, fftw_complex * signalb, fftw_complex * result, int N)
{
    fftw_complex * signala_ext = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * (2 * N - 1));
    fftw_complex * signalb_ext = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * (2 * N - 1));
    fftw_complex * out_shifted = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * (2 * N - 1));
    fftw_complex * outa = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * (2 * N - 1));
    fftw_complex * outb = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * (2 * N - 1));
    fftw_complex * out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * (2 * N - 1));

    fftw_plan pa = fftw_plan_dft_1d(2 * N - 1, signala_ext, outa, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_plan pb = fftw_plan_dft_1d(2 * N - 1, signalb_ext, outb, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_plan px = fftw_plan_dft_1d(2 * N - 1, out, result, FFTW_BACKWARD, FFTW_ESTIMATE);

    //zeropadding
    memset (signala_ext, 0, sizeof(fftw_complex) * (N - 1));
    memcpy (signala_ext + (N - 1), signala, sizeof(fftw_complex) * N);
    memcpy (signalb_ext, signalb, sizeof(fftw_complex) * N);
    memset (signalb_ext + N, 0, sizeof(fftw_complex) * (N - 1));

    fftw_execute(pa);
    fftw_execute(pb);

    // fftw_complex scale = 1.0/(2 * N -1);
    fftw_complex scale;
    scale[0] = 1.0/(2 * N -1);
    scale[1] = 0;
    for (int i = 0; i < 2 * N - 1; i++)
    {
        out[i][0] = (outa[i][0] * outb[i][0] + outa[i][1] * outb[i][1]) * scale[0];
        out[i][1] = (outa[i][1] * outb[i][0] - outa[i][0] * outb[i][1]) * scale[0];
    }
        // out[i] = outa[i] * conj(outb[i]) * scale;

    fftw_execute(px);

    fftw_destroy_plan(pa);
    fftw_destroy_plan(pb);
    fftw_destroy_plan(px);

    fftw_free(signala_ext);
    fftw_free(signalb_ext);
    fftw_free(out_shifted);
    fftw_free(out);
    fftw_free(outa);
    fftw_free(outb);

    fftw_cleanup();

    return;
}

#define SOUND_SPEED 343.2
#define MIC_DISTANCE_4 0.08127
#define MAX_TDOA_4 (MIC_DISTANCE_4 / SOUND_SPEED)
#define PI 3.14159265358979323846264338328
#define INPUT_SAMPLERATE 16000

double max_gcc_chat(const double *a1, const double *a2, int N)
{
    fftw_complex * a = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * N);
    fftw_complex * b = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * N);
    fftw_complex * o = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * (2 * N - 1));

    for(int i = 0; i < N; ++i)
    {
        a[i][0] = a1[i];
        a[i][1] = 0;
        b[i][0] = a2[i];
        b[i][1] = 0;
    }

    xcorr(a, b, o, N);

    // int max_shift = min((int)(MAX_TDOA_4*INPUT_SAMPLERATE), N-1);
    int max_shift = (int)(MAX_TDOA_4*INPUT_SAMPLERATE);
    double maxabs = 0;
    int shift = 0;

    for(int i = 0; i <= max_shift; ++i)
    {
        double curabs = o[i][0]*o[i][0]+o[i][1]*o[i][1];
        if(curabs > maxabs)
        {
            maxabs = curabs;
            shift = i;
        }
    }

    for(int i = 2*N - 1 - max_shift; i < 2*N - 1; ++i)
    {
        double curabs = o[i][0]*o[i][0]+o[i][1]*o[i][1];
        if(curabs > maxabs)
        {
            maxabs = curabs;
            shift = i - 2 * N;
        }
    }

    fftw_free(a);
    fftw_free(b);
    fftw_free(o);

    return (double)shift / (double)INPUT_SAMPLERATE;
}
