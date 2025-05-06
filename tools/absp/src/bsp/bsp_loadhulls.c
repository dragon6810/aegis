#include <bsp/bsp.h>

#include <cli/cli.h>

void bsp_loadhulls(void)
{
    int i,h;
    
    FILE *ptr;
    int imodel;
    unsigned long int before;
    vec3_t v;
    bsp_texinfo_t texinfo;
    bsp_model_t *model;
    bsp_face_t *face;
    float scale;
    int npoints;
    int nvertices[MAX_MAP_HULLS] = {}, nfaces[MAX_MAP_HULLS] = {};

    for(h=0; h<MAX_MAP_HULLS; h++)
    {
        ptr = fopen(cli_infiles[h], "r");
        if(!ptr)
            continue;

        while(fscanf(ptr, "*%d", &imodel) == 1)
        {
            fseek(ptr, 1, SEEK_CUR);

            if(!h)
                bsp_nmodels++;

            model = &bsp_models[imodel];
            model->firstface[h] = bsp_nfaces[h];
            model->nfaces[h] = 0;

            while(fgetc(ptr) == '(')
            {
                fseek(ptr, -1, SEEK_CUR);

                face = &bsp_faces[h][bsp_nfaces[h]++];
                model->nfaces[h]++;

                npoints = 0;
                before = ftell(ptr);
                while(fscanf(ptr, "( %f %f %f ) ", &v[0], &v[1], &v[2]))
                    npoints++;
                
                fseek(ptr, before, SEEK_SET);
                face->poly = AllocPoly(npoints);
                for(i=0; i<npoints; i++)
                    fscanf(ptr, "( %f %f %f ) ", 
                        &face->poly->points[i][0], 
                        &face->poly->points[i][1], 
                        &face->poly->points[i][2]);
                
                if(!fscanf(ptr, "%15s ", texinfo.texname))
                    cli_error(true, "parsing hull \"%s\"\n", cli_infiles[h]);
                
                for(i=0; i<2; i++)
                    if(!fscanf(ptr, "[ %f %f %f %f ] ", 
                            &texinfo.basis[i][0], &texinfo.basis[i][1], &texinfo.basis[i][2],
                            &texinfo.shift[i]))
                        cli_error(true, "parsing hull \"%s\"\n", cli_infiles[h]);

                for(i=0; i<2; i++)
                {
                    if(!fscanf(ptr, "%f ", &scale))
                        cli_error(true, "parsing hull \"%s\"\n", cli_infiles[h]);
                    VectorScale(texinfo.basis[i], texinfo.basis[i], scale);
                }

                nvertices[h] += npoints;
                nfaces[h]++;
            }
        }
    }

    if(cli_verbose)
    {
        printf(" | input | vertices | faces | \n");
        for(h=0; h<MAX_MAP_HULLS; h++)
        {
            printf("-+-------+----------+-------+-\n");
            printf(" | %5d | %8d | %5d | \n", h, nvertices[h], nfaces[h]);
        }
        printf("-+-------+----------+-------+-\n");
    }
}