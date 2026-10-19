#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "timer.h"

#define G 6.67430e-11
#define DT 0.01  // 200 for solar system

typedef struct
{
    double x;
    double y;
} vec2_t;

typedef struct
{
    double m;
    vec2_t pos;
    vec2_t vel;
} point_t;

// Simulation variables
double tEnd = 0;
size_t nPoints = 0;
point_t* points = NULL;

// Multithreading variables
size_t nThreads = 0;
pthread_barrier_t barrier;
vec2_t** localForces = NULL;
FILE* outputFile = NULL;

int parseInput(const char* inputPath);
void* routine(void* pRank);

int main(int argc, const char* argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s nThreads t_end input_path\n", argv[0]);
        return EXIT_FAILURE;
    }

    nThreads = strtoul(argv[1], NULL, 10);
    nThreads = (nThreads == 0) ? 1 : nThreads;

    tEnd = strtod(argv[2], NULL);
    const char* inputPath = argv[3];

    printf("Threads: %zu | tEnd: %.2f | Input: %s\n", nThreads, tEnd, inputPath);

    int status = -1;
    if (parseInput(inputPath) != 0)
    {
        fprintf(stderr, "[Error] parsing error\n");
        return EXIT_FAILURE;
    }
    printf("Points loaded: %zu\n", nPoints);

    // Open file for logging
    outputFile = fopen("nbody_output.csv", "w");
    if (!outputFile)
    {
        perror("[Error] couldn't open output file");
        return EXIT_FAILURE;
    }

    // Initialize resources for threads
    pthread_t* threadHandles = malloc(nThreads * sizeof(pthread_t));
    localForces = calloc(nThreads, sizeof(vec2_t*));
    if (!threadHandles || !localForces)
    {
        perror("Failed to allocate threads/forces arrays");
        goto main_cleanup;
    }
    for (size_t i = 0; i < nThreads; i++)
    {
        localForces[i] = calloc(nPoints, sizeof(vec2_t));
    }
    pthread_barrier_init(&barrier, NULL, nThreads);

    // START BENCHMARKING
    double startTime, endTime;
    GET_TIME(startTime);

    for (size_t i = 0; i < nThreads; i++)
    {
        pthread_create(&threadHandles[i], NULL, routine, (void*)i);
    }

    for (size_t i = 0; i < nThreads; i++)
    {
        pthread_join(threadHandles[i], NULL);
    }

    GET_TIME(endTime);
    // END BENCHMARKING

    printf("\n[SUCCESS] Simulation finished.\n");
    printf("Time: %lf seconds\n", endTime - startTime);

    status = 0;

main_cleanup:
    if (outputFile)
        fclose(outputFile);
    if (points)
        free(points);
    if (threadHandles)
        free(threadHandles);
    if (localForces)
    {
        for (size_t i = 0; i < nThreads; i++)
        {
            if (localForces[i])
                free(localForces[i]);
        }
        free(localForces);
    }
    pthread_barrier_destroy(&barrier);

    return (status == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

void* routine(void* pRank)
{
    size_t rank = (size_t)pRank;
    vec2_t* myForces = localForces[rank];

    for (double t = 0; t <= tEnd; t += DT)
    {
        // Log current positions of points
        if (rank == 0 && outputFile != NULL)
        {
            fprintf(outputFile, "%.5f", t);
            for (size_t i = 0; i < nPoints; i++)
            {
                fprintf(outputFile, ",%.5f,%.5f", points[i].pos.x, points[i].pos.y);
            }
            fprintf(outputFile, "\n");
        }

        // Calculate new forces between points
        // Thread number rank processes points with indexes rank, rank + nThreads, rank + 2*nThreads ...
        // Such approach balances the workload across threads
        memset(myForces, 0, nPoints * sizeof(vec2_t));  // Reset forces to zero
        for (size_t q = rank; q < nPoints; q += nThreads)
        {
            for (size_t k = q + 1; k < nPoints; k++)
            {
                double m_k = points[k].m;
                double m_q = points[q].m;

                double rx_k = points[k].pos.x;
                double ry_k = points[k].pos.y;
                double rx_q = points[q].pos.x;
                double ry_q = points[q].pos.y;

                double dx = rx_k - rx_q;
                double dy = ry_k - ry_q;

                double dist = dx * dx + dy * dy;  // dist^2
                if (dist < 1e-10)                 // protection from division by zero
                    continue;

                double denom = dist * sqrt(dist);  // (dist^2) * dist = dist^3, faster than pow(dist^2, 1.5)
                double forceScaler = G * m_q * m_k / denom;

                // Update forces in local buffer
                myForces[q].x += forceScaler * dx;
                myForces[q].y += forceScaler * dy;

                // Newton's Third Law
                myForces[k].x -= forceScaler * dx;
                myForces[k].y -= forceScaler * dy;
            }
        }

        // Wait for all threads to finish calculating their local forces
        pthread_barrier_wait(&barrier);

        // Find new position and velocity for points
        for (size_t q = rank; q < nPoints; q += nThreads)
        {
            double F_x = 0;
            double F_y = 0;

            // Calculate final force for the current point
            for (size_t thread = 0; thread < nThreads; thread++)
            {
                F_x += localForces[thread][q].x;
                F_y += localForces[thread][q].y;
            }

            double m = points[q].m;
            double v_x = points[q].vel.x;
            double v_y = points[q].vel.y;

            // Update the position
            points[q].pos.x += v_x * DT;
            points[q].pos.y += v_y * DT;

            // Update the velocity
            points[q].vel.x += (F_x / m) * DT;
            points[q].vel.y += (F_y / m) * DT;
        }

        // Wait for all threads to finish updating positions and velocities for points
        pthread_barrier_wait(&barrier);
    }

    return NULL;
}

int parseInput(const char* inputPath)
{
    FILE* file = fopen(inputPath, "r");
    if (file == NULL)
    {
        perror("[Error] couldn't open a file to read");
        return -1;
    }

    char* line = NULL;
    size_t size = 0;
    int status = -1;  // error status

    if (getline(&line, &size, file) == -1)
    {
        perror("Couldn't read the first line from the file");
        goto cleanup;
    }

    if (sscanf(line, "%zu", &nPoints) != 1)
    {
        fprintf(stderr, "[Error] couldn't parse the number of points\n");
        goto cleanup;
    }

    points = malloc(nPoints * sizeof(point_t));
    if (!points)
    {
        perror("[Error] couldn't allocate memory for points buffer");
        goto cleanup;
    }

    for (size_t i = 0; i < nPoints; i++)
    {
        if (getline(&line, &size, file) == -1)
        {
            fprintf(stderr, "[Error] unexpected EOF at point %zu\n", i);
            goto cleanup;
        }

        point_t* p = &points[i];
        if (sscanf(line, "%lf %lf %lf %lf %lf", &p->m, &p->pos.x, &p->pos.y, &p->vel.x, &p->vel.y) != 5)
        {
            fprintf(stderr, "[Error] couldn't parse point data\n");
            goto cleanup;
        }
    }

    status = 0;

cleanup:
    if (line)
        free(line);
    if (file)
        fclose(file);

    if (status != 0 && points != NULL)
    {
        free(points);
        points = NULL;
    }

    return status;
}
