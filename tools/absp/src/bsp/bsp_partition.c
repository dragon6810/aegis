#include <bsp/bsp.h>

#include <std/assert/assert.h>
#include <std/profiler/profiler.h>

void bsp_partition_boundmodel(bsp_model_t* model)
{
    int h, i, j, k;

    bsp_face_t *f;

    assert(model);

    for(h=0; h<MAX_MAP_HULLS; h++)
    {
        model->bounds[h][0][0] = model->bounds[h][0][1] = model->bounds[h][0][2] = -MAX_MAP_RANGE;
        model->bounds[h][1][0] = model->bounds[h][1][1] = model->bounds[h][1][2] =  MAX_MAP_RANGE;

        for(i=model->firstface[h]; i<model->firstface[h]+model->nfaces[h]; i++)
        {
            f = &bsp_faces[h][i];
            for(j=0; j<f->poly->npoints; j++)
            {
                for(k=0; k<3; k++)
                {
                    if(f->poly->points[j][k] < model->bounds[h][0][k])
                        model->bounds[h][0][k] = f->poly->points[j][k];
                    if(f->poly->points[j][k] > model->bounds[h][1][k])
                        model->bounds[h][1][k] = f->poly->points[j][k];
                }
            }
        }
    }
}

/* TODO: Currently approximating with monte carlo. make more sophisticated in the future. */
int bsp_partition_spaceratio(vec3_t n, float d, vec3_t bounds[2])
{
    const float epsilon = 0.01;

    int s[3], i;

    vec3_t p;
    float dist;
    int counts[2] = { 0, 0 };

    const int nsamples = 4;

    for(s[0]=0; s[0]<nsamples; s[0]++)
    {
        for(s[1]=0; s[1]<nsamples; s[1]++)
        {
            for(s[2]=0; s[2]<nsamples; s[2]++)
            {
                for(i=0; i<3; i++)
                    p[i] = bounds[0][i] + ((float) s[i] / (float) nsamples) * (bounds[1][i] - bounds[0][i]);
                dist = VectorDot(p, n) - d;

                if(dist < -epsilon)
                    counts[0]++;
                if(dist > epsilon)
                    counts[1]++;
            }
        }
    }

    return counts[1] - counts[0];
}

int bsp_partition_chooseplane(list_int_t faces, int hull)
{
    int i, j, k;
    bsp_face_t *f;

    vec3_t bounds[2];
    vec3_t p;
    vec3_t n, a, b;
    float d;
    int score;

    int bestscore, bestface;

    if(!faces.data)
        return -1;

    for(i=0; i<3; i++)
    {
        bounds[0][i] = MAX_MAP_RANGE + 8;
        bounds[1][i] = -(MAX_MAP_RANGE + 8);
    }

    for(i=0; i<faces.size; i++)
    {
        f = &bsp_faces[hull][faces.data[i]];
        for(j=0; j<f->poly->npoints; j++)
        {
            VectorCopy(p, f->poly->points[j]);
            for(k=0; k<3; k++)
            {
                if(p[k] < bounds[0][k])
                    bounds[0][k] = p[k];
                if(p[k] > bounds[1][k])
                    bounds[1][k] = p[k];
            }
        }
    }

    bestface = -1;
    bestscore = 0;
    for(i=0; i<faces.size; i++)
    {
        f = &bsp_faces[hull][faces.data[i]];
        if(f->poly->npoints < 3)
            continue;

        VectorSubtract(a, f->poly->points[1], f->poly->points[0]);
        VectorSubtract(b, f->poly->points[2], f->poly->points[0]);
        VectorCross(n, a, b);
        VectorNormalize(n, n);
        d = VectorDot(n, f->poly->points[0]);
        score = bsp_partition_spaceratio(n, d, bounds);

        if(score < bestscore || !i)
        {
            bestface = i;
            bestscore = score;
        }
    }

    if(bestface == -1)
        return -1;

    return bestface;
}

void bsp_partition_processmodel(bsp_model_t* model)
{
    assert(model);

    bsp_partition_boundmodel(model);
}

void bsp_partition(void)
{
    int i;

    profiler_push("Partition World");

    for(i=0; i<bsp_nmodels; i++)
        bsp_partition_processmodel(&bsp_models[i]);

    profiler_pop();
}