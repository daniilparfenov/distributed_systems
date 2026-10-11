#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define G 6.67430e-11
#define DT 0.01

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

int parseInput(const char* inputPath);
int simulate();

double tEnd = 0;
size_t nPoints = 0;
point_t* points = NULL;

int main(int argc, const char* argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s t_end input_path\n", argv[0]);
        return EXIT_FAILURE;
    }
    tEnd = strtod(argv[1], NULL);
    const char* inputPath = argv[2];
    printf("tEnd = %lf\n", tEnd);
    printf("inputPath = %s\n", inputPath);

    int status = -1;
    if (parseInput(inputPath) != 0)
    {
        fprintf(stderr, "[Error] parsing error");
        goto main_cleanup;
    }

    printf("nPoints = %zu\n", nPoints);
    for (size_t i = 0; i < nPoints; i++)
    {
        printf("%lf %lf %lf %lf %lf\n", points[i].m, points[i].pos.x, points[i].pos.y, points[i].vel.x,
               points[i].vel.y);
    }

    if (simulate() != 0)
    {
        fprintf(stderr, "[Error] simulation error");
        goto main_cleanup;
    }
    status = 0;

main_cleanup:
    if (points)
        free(points);
    if (status != 0)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
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
        perror("Couldn't read a first line from the file");
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

int simulate()
{
    vec2_t* forces = calloc(nPoints, sizeof(vec2_t));
    if (!forces)
    {
        perror("[Error] couldn't allocate memory for forces buffer");
        return -1;
    }

    for (double t = 0; t <= tEnd; t += DT)
    {
        memset(forces, 0, nPoints * sizeof(vec2_t));  // Reset forces to zero
        for (size_t q = 0; q < nPoints; q++)
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

                forces[q].x += forceScaler * dx;
                forces[q].y += forceScaler * dy;

                // Newton's Third Law
                forces[k].x -= forceScaler * dx;
                forces[k].y -= forceScaler * dy;
            }
        }

        // Find new position and velocity for points
        for (size_t q = 0; q < nPoints; q++)
        {
            double m = points[q].m;
            double x = points[q].pos.x;
            double y = points[q].pos.y;
            double v_x = points[q].vel.x;
            double v_y = points[q].vel.y;
            double F_x = forces[q].x;
            double F_y = forces[q].y;

            points[q].pos.x = x + v_x * DT;
            points[q].pos.y = y + v_y * DT;
            points[q].vel.x = v_x + (F_x / m) * DT;
            points[q].vel.y = v_y + (F_y / m) * DT;
        }
    }
    free(forces);

    return 0;
}
