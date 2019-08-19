
#include "sweep.h"
extern double ker_launch_over[9];
extern double ker_exec_time[9];
extern int ker_call_nums[9];
extern cl_ulong start_time, end_time;extern size_t return_bytes;
extern struct timespec start, end;
extern size_t null_offset;
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
    //size_t global[3] = {problem->nang*problem->ng, planes[plane].num_cells};
   /*
    // Set the (many) kernel arguments
    err = clSetKernelArg(context->kernels.sweep_plane, 0, sizeof(unsigned int), &rankinfo->nx);
    err |= clSetKernelArg(context->kernels.sweep_plane, 1, sizeof(unsigned int), &rankinfo->ny);
    err |= clSetKernelArg(context->kernels.sweep_plane, 2, sizeof(unsigned int), &rankinfo->nz);
    err |= clSetKernelArg(context->kernels.sweep_plane, 3, sizeof(unsigned int), &problem->nang);
    err |= clSetKernelArg(context->kernels.sweep_plane, 4, sizeof(unsigned int), &problem->ng);
    err |= clSetKernelArg(context->kernels.sweep_plane, 5, sizeof(unsigned int), &problem->cmom);
    err |= clSetKernelArg(context->kernels.sweep_plane, 6, sizeof(int), &istep);
    err |= clSetKernelArg(context->kernels.sweep_plane, 7, sizeof(int), &jstep);
    err |= clSetKernelArg(context->kernels.sweep_plane, 8, sizeof(int), &kstep);
    err |= clSetKernelArg(context->kernels.sweep_plane, 9, sizeof(unsigned int), &octant);
    err |= clSetKernelArg(context->kernels.sweep_plane, 10, sizeof(unsigned int), &z_pos);
    err |= clSetKernelArg(context->kernels.sweep_plane, 11, sizeof(cl_mem), &buffers->planes[plane]);
    err |= clSetKernelArg(context->kernels.sweep_plane, 12, sizeof(cl_mem), &buffers->inner_source);
    err |= clSetKernelArg(context->kernels.sweep_plane, 13, sizeof(cl_mem), &buffers->scat_coeff);
    err |= clSetKernelArg(context->kernels.sweep_plane, 14, sizeof(cl_mem), &buffers->dd_i);
    err |= clSetKernelArg(context->kernels.sweep_plane, 15, sizeof(cl_mem), &buffers->dd_j);
    err |= clSetKernelArg(context->kernels.sweep_plane, 16, sizeof(cl_mem), &buffers->dd_k);
    err |= clSetKernelArg(context->kernels.sweep_plane, 17, sizeof(cl_mem), &buffers->mu);
    err |= clSetKernelArg(context->kernels.sweep_plane, 18, sizeof(cl_mem), &buffers->velocity_delta);
    err |= clSetKernelArg(context->kernels.sweep_plane, 19, sizeof(cl_mem), &buffers->mat_cross_section);
    err |= clSetKernelArg(context->kernels.sweep_plane, 20, sizeof(cl_mem), &buffers->denominator);
    err |= clSetKernelArg(context->kernels.sweep_plane, 21, sizeof(cl_mem), &buffers->angular_flux_in[octant]);
    err |= clSetKernelArg(context->kernels.sweep_plane, 22, sizeof(cl_mem), &buffers->flux_i);
    err |= clSetKernelArg(context->kernels.sweep_plane, 23, sizeof(cl_mem), &buffers->flux_j);
    err |= clSetKernelArg(context->kernels.sweep_plane, 24, sizeof(cl_mem), &buffers->flux_k);
    err |= clSetKernelArg(context->kernels.sweep_plane, 25, sizeof(cl_mem), &buffers->angular_flux_out[octant]);

    check_ocl(err, "Setting plane sweep kernel arguments");

    // Actually enqueue
    err = clEnqueueNDRangeKernel(context->queue,
        context->kernels.sweep_plane,
        2, 0, global, NULL,
        0, NULL, NULL);
    */cl_event temp1;
    clock_gettime(CLOCK_REALTIME, &start);
    err = meta_gen_opencl_sweep_plane_sweep_plane(context->queue, global, local, null_offset ,rankinfo->nx, rankinfo->ny, rankinfo->nz, problem->nang, problem->ng, problem->cmom,istep, jstep, kstep, octant, z_pos, &buffers->planes[plane], &buffers->inner_source, &buffers->scat_coeff, &buffers-> dd_i, &buffers->dd_j, &buffers->dd_k, &buffers->mu, &buffers->velocity_delta, &buffers->mat_cross_section, &buffers->denominator, &buffers->angular_flux_in[octant], &buffers->flux_i, &buffers->flux_j, &buffers->flux_k, &buffers->angular_flux_out[octant], 0, &temp1);
    clock_gettime(CLOCK_REALTIME, &end);
    ker_launch_over[7]+=( end.tv_sec - start.tv_sec ) + ( end.tv_nsec - start.tv_nsec )/ BILLION;
    err = clGetEventProfilingInfo(temp1,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),  &start_time,&return_bytes);
    err = clGetEventProfilingInfo(temp1,CL_PROFILING_COMMAND_END,sizeof(cl_ulong), &end_time,&return_bytes);
    ker_exec_time[7]+=(double)(end_time-start_time)/BILLION;
    temp1=NULL;
    check_ocl(err, "Enqueue plane sweep kernel");
    ker_call_nums[7]++;
}

