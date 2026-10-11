#include <stdio.h>
#include <stdlib.h>

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

    if (parseInput(inputPath) != 0)
        return EXIT_FAILURE;

    printf("nPoints = %zu\n", nPoints);
    for (size_t i = 0; i < nPoints; i++)
    {
        printf("%lf %lf %lf %lf %lf\n", points[i].m, points[i].pos.x, points[i].pos.y, points[i].vel.x,
               points[i].vel.y);
    }

    if (points)
        free(points);

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
