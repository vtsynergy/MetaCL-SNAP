
#include "source.h"


void compute_outer_source(
    const struct problem * problem,
    const struct rankinfo * rankinfo,
    struct context * context,
    struct buffers * buffers
    )
{
    cl_int err;

    size_t global[3] = {rankinfo->nx, rankinfo->ny, rankinfo->nz};
    size_t local[3] = {0,0,0};
    err = metacl_outer_zero_and_others_calc_outer_source(context->queue, global, local, NULL, 1, &outer_source_event,rankinfo->nx, rankinfo->ny, rankinfo->nz, problem->ng, problem->cmom, problem->nmom, &buffers->fixed_source, &buffers->scattering_matrix, &buffers->scalar_flux, &buffers->scalar_flux_moments, &buffers->outer_source);
    check_ocl(err, "Enqueue outer source kernel");
}


void compute_inner_source(
    const struct problem * problem,
    const struct rankinfo * rankinfo,
    struct context * context,
    struct buffers * buffers
    )
{
    cl_int err;

    size_t global[3] = {rankinfo->nx, rankinfo->ny, rankinfo->nz};
    size_t local[3] = {0,0,0};
    err = metacl_sweep_zero_inner_reducef_calc_inner_source(context->queue, global, local, NULL, 1, &inner_source_event, rankinfo->nx, rankinfo->ny, rankinfo->nz, problem->ng, problem->cmom, problem->nmom, &buffers->outer_source, &buffers->scattering_matrix, &buffers->scalar_flux, &buffers->scalar_flux_moments, &buffers->inner_source);
    check_ocl(err, "Enqueue inner source kernel");
}
