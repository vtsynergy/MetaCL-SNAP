
#include "sweep.h"

void init_planes(struct plane** planes, unsigned int *num_planes, struct problem * problem, struct rankinfo * rankinfo)
{
    *num_planes = rankinfo->nx + rankinfo->ny + problem->chunk - 2;
    *planes = malloc(sizeof(struct plane) * *num_planes);

    for (unsigned int p = 0; p < *num_planes; p++)
        (*planes)[p].num_cells = 0;

    for (unsigned int k = 0; k < problem->chunk; k++)
        for (unsigned int j = 0; j < rankinfo->ny; j++)
            for (unsigned int i = 0; i < rankinfo->nx; i++)
            {
                unsigned int p = i + j + k;
                (*planes)[p].num_cells += 1;
            }

    for (unsigned int p = 0; p < *num_planes; p++)
    {
        (*planes)[p].cell_ids = malloc(sizeof(struct cell_id) * (*planes)[p].num_cells);
    }

    unsigned int index[*num_planes];
    for (unsigned int p = 0; p < *num_planes; p++)
        index[p] = 0;

    for (unsigned int k = 0; k < problem->chunk; k++)
        for (unsigned int j = 0; j < rankinfo->ny; j++)
            for (unsigned int i = 0; i < rankinfo->nx; i++)
            {
                unsigned int p = i + j + k;
                (*planes)[p].cell_ids[index[p]].i = i;
                (*planes)[p].cell_ids[index[p]].j = j;
                (*planes)[p].cell_ids[index[p]].k = k;
                index[p] += 1;
            }
}

void copy_planes(const struct plane * planes, const unsigned int num_planes, struct context * context, struct buffers * buffers)
{
    buffers->planes = malloc(sizeof(cl_mem)*num_planes);

    cl_int err;
    for (unsigned int p = 0; p < num_planes; p++)
    {
        buffers->planes[p] =
            clCreateBuffer(context->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof(struct cell_id)*planes[p].num_cells, planes[p].cell_ids, &err);
        check_ocl(err, "Creating and copying a plane cell indicies buffer");
    }
}

void sweep_plane(
    const unsigned int z_pos,
    const int octant,
    const int istep,
    const int jstep,
    const int kstep,
    const unsigned int plane,
    const struct plane * planes,
    struct problem * problem,
    struct rankinfo * rankinfo,
    struct context * context,
    struct buffers * buffers
    )
{
    cl_int err;

    size_t global[3] = {problem->nang*problem->ng, planes[plane].num_cells,1};
    size_t local[3] = {0,0,0};
    // 2 dimensional kernel
    // First dimension: number of angles * number of groups
    // Second dimension: number of cells in plane
   
    // Actually enqueue
    size_t m_offset[3]={0,0,0};
    err = metacl_sweep_zero_inner_reducef_sweep_plane(context->queue, global, local, NULL, 1, NULL,rankinfo->nx, rankinfo->ny, rankinfo->nz, problem->nang, problem->ng, problem->cmom,istep, jstep, kstep, octant, z_pos, &buffers->planes[plane], &buffers->inner_source, &buffers->scat_coeff, &buffers-> dd_i, &buffers->dd_j, &buffers->dd_k, &buffers->mu, &buffers->velocity_delta, &buffers->mat_cross_section, &buffers->denominator, &buffers->angular_flux_in[octant], &buffers->flux_i, &buffers->flux_j, &buffers->flux_k, &buffers->angular_flux_out[octant]);
    check_ocl(err, "Enqueue plane sweep kernel");
}

