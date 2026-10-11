#include <complex.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "my_rand.h"

#define LOWER_COORD ((double)-1.5)
#define UPPER_COORD ((double)1.5)
#define MAX_N 10000

pthread_mutex_t mutex;

size_t nThreads, nPoints;
size_t foundMandelbrotPoint = 0;
double complex* mandelbrotSet;

bool isMandelbrotPoint(const double complex point);
double complex genRandCmplx(unsigned* pSeed, double lower, double upper);
int saveMandelbrotSetToCSV(const char* filename);

void* routine(void* pRank)
{
    size_t myRank = (size_t)pRank;
    size_t pointToFindCnt = nPoints / nThreads;
    if (myRank == nThreads - 1)
        pointToFindCnt = nPoints / nThreads + nPoints % nThreads;
    size_t foundPoints = 0;
    unsigned seed = (unsigned)myRank + 1;  // +1 to protect from zero seed

    while (foundPoints < pointToFindCnt)
    {
        double complex c = genRandCmplx(&seed, LOWER_COORD, UPPER_COORD);
        if (isMandelbrotPoint(c))
        {
            pthread_mutex_lock(&mutex);
            mandelbrotSet[foundMandelbrotPoint++] = c;
            pthread_mutex_unlock(&mutex);
            foundPoints++;
        }
    }
    return NULL;
}

int main(int argc, const char* argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s nthreads npoints\n", argv[0]);
        return EXIT_FAILURE;
    }

    nThreads = strtoul(argv[1], NULL, 10);
    nPoints = strtoul(argv[2], NULL, 10);
    pthread_t* threadHandles = malloc(nThreads * sizeof(pthread_t));
    mandelbrotSet = malloc(nPoints * sizeof(double complex));

    pthread_mutex_init(&mutex, NULL);

    for (size_t i = 0; i < nThreads; i++)
    {
        pthread_create(&threadHandles[i], NULL, routine, (void*)i);
    }

    for (size_t i = 0; i < nThreads; i++)
    {
        pthread_join(threadHandles[i], NULL);
    }

    pthread_mutex_destroy(&mutex);

    saveMandelbrotSetToCSV("mandelbrot.csv");

    free(threadHandles);
    free(mandelbrotSet);

    return EXIT_SUCCESS;
}

bool isMandelbrotPoint(const double complex point)
{
    double complex z_n = 0;
    double complex c = point;
    for (size_t i = 0; i < MAX_N; i++)
    {
        z_n = z_n * z_n + c;
        if (cabs(z_n) >= 2.0)
            return false;
    }
    return true;
}

double complex genRandCmplx(unsigned* pSeed, double lower, double upper)
{
    double re = lower + (upper - lower) * my_drand(pSeed);
    double im = lower + (upper - lower) * my_drand(pSeed);
    double complex randCmplx = CMPLX(re, im);
    return randCmplx;
}

int saveMandelbrotSetToCSV(const char* filename)
{
    FILE* file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("Error: couldn't open a file to write");
        return -1;
    }

    fprintf(file, "real,imag\n");

    for (size_t i = 0; i < nPoints; ++i)
    {
        fprintf(file, "%.17g,%.17g\n", creal(mandelbrotSet[i]), cimag(mandelbrotSet[i]));
    }

    fclose(file);
    return 0;
}
