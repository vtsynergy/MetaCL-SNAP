
#include "source.h"

extern double ker_launch_over[9];
extern double ker_exec_time[9];
extern int ker_call_nums[9];
extern cl_ulong start_time, end_time;extern size_t return_bytes;
extern struct timespec start, end;
extern size_t null_offset[3];
void compute_outer_source(
    const struct problem * problem,
    const struct rankinfo * rankinfo,
    struct context * context,
    struct buffers * buffers
    )
{
    cl_int err;
    
//    err = clSetKernelArg(context->kernels.outer_source, 0, sizeof(unsigned int), &rankinfo->nx);
//    err |= clSetKernelArg(context->kernels.outer_source, 1, sizeof(unsigned int), &rankinfo->ny);
//    err |= clSetKernelArg(context->kernels.outer_source, 2, sizeof(unsigned int), &rankinfo->nz);
//    err |= clSetKernelArg(context->kernels.outer_source, 3, sizeof(unsigned int), &problem->ng);
//    err |= clSetKernelArg(context->kernels.outer_source, 4, sizeof(unsigned int), &problem->cmom);
//    err |= clSetKernelArg(context->kernels.outer_source, 5, sizeof(unsigned int), &problem->nmom);
//    err |= clSetKernelArg(context->kernels.outer_source, 6, sizeof(cl_mem), &buffers->fixed_source);
//    err |= clSetKernelArg(context->kernels.outer_source, 7, sizeof(cl_mem), &buffers->scattering_matrix);
//    err |= clSetKernelArg(context->kernels.outer_source, 8, sizeof(cl_mem), &buffers->scalar_flux);
//    err |= clSetKernelArg(context->kernels.outer_source, 9, sizeof(cl_mem), &buffers->scalar_flux_moments);
//    err |= clSetKernelArg(context->kernels.outer_source, 10, sizeof(cl_mem), &buffers->outer_source);
//    check_ocl(err, "Setting outer source kernel arguments");
//
//    //size_t global[] = {rankinfo->nx, rankinfo->ny, rankinfo->nz};
//    size_t global[] = {rankinfo->nx, rankinfo->ny, rankinfo->nz};
//    size_t local[] = {0,0,0};
//    err = clEnqueueNDRangeKernel(context->queue,
//        context->kernels.outer_source,
//        3, 0, global, NULL,
//        0, NULL, &outer_source_event);
  
   size_t global[3] = {rankinfo->nx, rankinfo->ny, rankinfo->nz};
    size_t local[3] = {0,0,0};clock_gettime(CLOCK_REALTIME, &start);
    err = meta_gen_opencl_outer_source_calc_outer_source(context->queue, global, local, null_offset,rankinfo->nx, rankinfo->ny, rankinfo->nz, problem->ng, problem->cmom, problem->nmom, &buffers->fixed_source, &buffers->scattering_matrix, &buffers->scalar_flux, &buffers->scalar_flux_moments, &buffers->outer_source, 0, &outer_source_event);
clock_gettime(CLOCK_REALTIME, &end);
    ker_launch_over[0]+=( end.tv_sec - start.tv_sec ) + ( end.tv_nsec - start.tv_nsec )/ BILLION;
    err = clGetEventProfilingInfo(outer_source_event,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),  &start_time,&return_bytes);
    err = clGetEventProfilingInfo(outer_source_event,CL_PROFILING_COMMAND_END,sizeof(cl_ulong), &end_time,&return_bytes);
    ker_exec_time[0]+=(double)(end_time-start_time)/BILLION;
    check_ocl(err, "Enqueue outer source kernel");
    ker_call_nums[0]++;
}


void compute_inner_source(
    const struct problem * problem,
    const struct rankinfo * rankinfo,
    struct context * context,
    struct buffers * buffers
    )
{
    cl_int err;
    
//    err = clSetKernelArg(context->kernels.inner_source, 0, sizeof(unsigned int), &rankinfo->nx);
//    err |= clSetKernelArg(context->kernels.inner_source, 1, sizeof(unsigned int), &rankinfo->ny);
//    err |= clSetKernelArg(context->kernels.inner_source, 2, sizeof(unsigned int), &rankinfo->nz);
//    err |= clSetKernelArg(context->kernels.inner_source, 3, sizeof(unsigned int), &problem->ng);
//    err |= clSetKernelArg(context->kernels.inner_source, 4, sizeof(unsigned int), &problem->cmom);
//    err |= clSetKernelArg(context->kernels.inner_source, 5, sizeof(unsigned int), &problem->nmom);
//    err |= clSetKernelArg(context->kernels.inner_source, 6, sizeof(cl_mem), &buffers->outer_source);
//    err |= clSetKernelArg(context->kernels.inner_source, 7, sizeof(cl_mem), &buffers->scattering_matrix);
//    err |= clSetKernelArg(context->kernels.inner_source, 8, sizeof(cl_mem), &buffers->scalar_flux);
//    err |= clSetKernelArg(context->kernels.inner_source, 9, sizeof(cl_mem), &buffers->scalar_flux_moments);
//    err |= clSetKernelArg(context->kernels.inner_source, 10, sizeof(cl_mem), &buffers->inner_source);
//    check_ocl(err, "Setting inner source kernel arguments");
//
//    size_t global[] = {rankinfo->nx, rankinfo->ny, rankinfo->nz};
//    err = clEnqueueNDRangeKernel(context->queue,
//        context->kernels.inner_source,
//        3, 0, global, NULL,
//        0, NULL, &inner_source_event);
    
   size_t global[3] = {rankinfo->nx, rankinfo->ny, rankinfo->nz};
    size_t local[3] = {0,0,0};clock_gettime(CLOCK_REALTIME, &start);
    err = meta_gen_opencl_inner_source_calc_inner_source(context->queue, global, local, null_offset,rankinfo->nx, rankinfo->ny, rankinfo->nz, problem->ng, problem->cmom, problem->nmom, &buffers->outer_source, &buffers->scattering_matrix, &buffers->scalar_flux, &buffers->scalar_flux_moments, &buffers->inner_source, 0, &inner_source_event);
    clock_gettime(CLOCK_REALTIME, &end);
    ker_launch_over[1]+=( end.tv_sec - start.tv_sec ) + ( end.tv_nsec - start.tv_nsec )/ BILLION;
    err = clGetEventProfilingInfo(inner_source_event,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),  &start_time,&return_bytes);
    err = clGetEventProfilingInfo(inner_source_event,CL_PROFILING_COMMAND_END,sizeof(cl_ulong), &end_time,&return_bytes);
    ker_exec_time[1]+=(double)(end_time-start_time)/BILLION;
    check_ocl(err, "Enqueue inner source kernel");
    ker_call_nums[1]++;
}
