
#include "scalar_flux.h"
extern double ker_launch_over[9];
extern double ker_exec_time[9];
extern int ker_call_nums[9];
extern cl_ulong start_time, end_time;extern size_t return_bytes;
extern struct timespec start, end;
void compute_scalar_flux(
    struct problem * problem,
    struct rankinfo * rankinfo,
    struct context * context,
    struct buffers * buffers
    )
{

    // get closest power of 2 to nang
    size_t power = 1 << (unsigned int)ceil(log2((double)problem->nang));

    const size_t global[] = {power * problem->ng, rankinfo->nx*rankinfo->ny*rankinfo->nz};
    const size_t local[] = {power, 1};

    cl_int err;
	err=clFinish(context->queue);
    err=clFinish(context->copy_queue);
    clock_gettime(CLOCK_REALTIME, &start);
    err  = clSetKernelArg(context->kernels.reduce_flux,  0, sizeof(unsigned int), &rankinfo->nx);
    err |= clSetKernelArg(context->kernels.reduce_flux,  1, sizeof(unsigned int), &rankinfo->ny);
    err |= clSetKernelArg(context->kernels.reduce_flux,  2, sizeof(unsigned int), &rankinfo->nz);
    err |= clSetKernelArg(context->kernels.reduce_flux,  3, sizeof(unsigned int), &problem->nang);
    err |= clSetKernelArg(context->kernels.reduce_flux,  4, sizeof(unsigned int), &problem->ng);
    err |= clSetKernelArg(context->kernels.reduce_flux,  5, sizeof(cl_mem), &buffers->angular_flux_in[0]);
    err |= clSetKernelArg(context->kernels.reduce_flux,  6, sizeof(cl_mem), &buffers->angular_flux_in[1]);
    err |= clSetKernelArg(context->kernels.reduce_flux,  7, sizeof(cl_mem), &buffers->angular_flux_in[2]);
    err |= clSetKernelArg(context->kernels.reduce_flux,  8, sizeof(cl_mem), &buffers->angular_flux_in[3]);
    err |= clSetKernelArg(context->kernels.reduce_flux,  9, sizeof(cl_mem), &buffers->angular_flux_in[4]);
    err |= clSetKernelArg(context->kernels.reduce_flux, 10, sizeof(cl_mem), &buffers->angular_flux_in[5]);
    err |= clSetKernelArg(context->kernels.reduce_flux, 11, sizeof(cl_mem), &buffers->angular_flux_in[6]);
    err |= clSetKernelArg(context->kernels.reduce_flux, 12, sizeof(cl_mem), &buffers->angular_flux_in[7]);
    err |= clSetKernelArg(context->kernels.reduce_flux, 13, sizeof(cl_mem), &buffers->angular_flux_out[0]);
    err |= clSetKernelArg(context->kernels.reduce_flux, 14, sizeof(cl_mem), &buffers->angular_flux_out[1]);
    err |= clSetKernelArg(context->kernels.reduce_flux, 15, sizeof(cl_mem), &buffers->angular_flux_out[2]);
    err |= clSetKernelArg(context->kernels.reduce_flux, 16, sizeof(cl_mem), &buffers->angular_flux_out[3]);
    err |= clSetKernelArg(context->kernels.reduce_flux, 17, sizeof(cl_mem), &buffers->angular_flux_out[4]);
    err |= clSetKernelArg(context->kernels.reduce_flux, 18, sizeof(cl_mem), &buffers->angular_flux_out[5]);
    err |= clSetKernelArg(context->kernels.reduce_flux, 19, sizeof(cl_mem), &buffers->angular_flux_out[6]);
    err |= clSetKernelArg(context->kernels.reduce_flux, 20, sizeof(cl_mem), &buffers->angular_flux_out[7]);
    err |= clSetKernelArg(context->kernels.reduce_flux, 21, sizeof(cl_mem), &buffers->velocity_delta);
    err |= clSetKernelArg(context->kernels.reduce_flux, 22, sizeof(cl_mem), &buffers->quad_weights);
    err |= clSetKernelArg(context->kernels.reduce_flux, 23, sizeof(cl_mem), &buffers->scalar_flux);
    err |= clSetKernelArg(context->kernels.reduce_flux, 24, sizeof(double)*local[0], NULL);
    check_ocl(err, "Setting scalar flux reduction kernel arguments");

    err = clEnqueueNDRangeKernel(context->queue,
        context->kernels.reduce_flux,
        2, 0, global, local,
        0, NULL, &scalar_flux_event);
    clFinish(context->queue);
    clock_gettime(CLOCK_REALTIME, &end);
    ker_launch_over[5]+=( end.tv_sec - start.tv_sec ) + ( end.tv_nsec - start.tv_nsec )/ BILLION;
    err = clGetEventProfilingInfo(scalar_flux_event,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),  &start_time,&return_bytes);
    err = clGetEventProfilingInfo(scalar_flux_event,CL_PROFILING_COMMAND_END,sizeof(cl_ulong), &end_time,&return_bytes);
    ker_exec_time[5]+=(double)(end_time-start_time)/BILLION;
    check_ocl(err, "Enqueueing scalar flux reduction kernel");
    ker_call_nums[5]++;

}

void compute_scalar_flux_moments(
    struct problem * problem,
    struct rankinfo * rankinfo,
    struct context * context,
    struct buffers * buffers
    )
{

    // get closest power of 2 to nang
    size_t power = 1 << (unsigned int)ceil(log2((double)problem->nang));

    const size_t global[] = {power * problem->ng, rankinfo->nx*rankinfo->ny*rankinfo->nz};
    const size_t local[] = {power, 1};

    cl_int err;
	err=clFinish(context->queue);
    err=clFinish(context->copy_queue);
    clock_gettime(CLOCK_REALTIME, &start);
    err  = clSetKernelArg(context->kernels.reduce_flux_moments,  0, sizeof(unsigned int), &rankinfo->nx);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments,  1, sizeof(unsigned int), &rankinfo->ny);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments,  2, sizeof(unsigned int), &rankinfo->nz);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments,  3, sizeof(unsigned int), &problem->nang);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments,  4, sizeof(unsigned int), &problem->ng);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments,  5, sizeof(unsigned int), &problem->cmom);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments,  6, sizeof(cl_mem), &buffers->angular_flux_in[0]);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments,  7, sizeof(cl_mem), &buffers->angular_flux_in[1]);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments,  8, sizeof(cl_mem), &buffers->angular_flux_in[2]);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments,  9, sizeof(cl_mem), &buffers->angular_flux_in[3]);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments, 10, sizeof(cl_mem), &buffers->angular_flux_in[4]);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments, 11, sizeof(cl_mem), &buffers->angular_flux_in[5]);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments, 12, sizeof(cl_mem), &buffers->angular_flux_in[6]);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments, 13, sizeof(cl_mem), &buffers->angular_flux_in[7]);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments, 14, sizeof(cl_mem), &buffers->angular_flux_out[0]);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments, 15, sizeof(cl_mem), &buffers->angular_flux_out[1]);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments, 16, sizeof(cl_mem), &buffers->angular_flux_out[2]);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments, 17, sizeof(cl_mem), &buffers->angular_flux_out[3]);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments, 18, sizeof(cl_mem), &buffers->angular_flux_out[4]);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments, 19, sizeof(cl_mem), &buffers->angular_flux_out[5]);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments, 20, sizeof(cl_mem), &buffers->angular_flux_out[6]);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments, 21, sizeof(cl_mem), &buffers->angular_flux_out[7]);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments, 22, sizeof(cl_mem), &buffers->velocity_delta);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments, 23, sizeof(cl_mem), &buffers->quad_weights);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments, 24, sizeof(cl_mem), &buffers->scat_coeff);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments, 25, sizeof(cl_mem), &buffers->scalar_flux_moments);
    err |= clSetKernelArg(context->kernels.reduce_flux_moments, 26, sizeof(double)*local[0], NULL);
    check_ocl(err, "Setting scalar flux moments reduction kernel arguments");

    err = clEnqueueNDRangeKernel(context->queue,
        context->kernels.reduce_flux_moments,
        2, 0, global, local,
        0, NULL, &scalar_flux_moments_event);
     clFinish(context->queue);
    clock_gettime(CLOCK_REALTIME, &end);
    ker_launch_over[6]+=( end.tv_sec - start.tv_sec ) + ( end.tv_nsec - start.tv_nsec )/ BILLION;
    err = clGetEventProfilingInfo(scalar_flux_moments_event,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),  &start_time,&return_bytes);
    err = clGetEventProfilingInfo(scalar_flux_moments_event,CL_PROFILING_COMMAND_END,sizeof(cl_ulong), &end_time,&return_bytes);
    ker_exec_time[6]+=(double)(end_time-start_time)/BILLION;
    check_ocl(err, "Enqueueing scalar flux moments reduction kernel");
    ker_call_nums[6]++;

}


void copy_back_scalar_flux(
    struct problem *problem,
    struct rankinfo * rankinfo,
    struct context * context,
    struct buffers * buffers,
    double * scalar_flux,
    cl_bool blocking
    )
{
    cl_int err;
    err = clEnqueueReadBuffer(context->copy_queue, buffers->scalar_flux, blocking,
        0, sizeof(double)*problem->ng*rankinfo->nx*rankinfo->ny*rankinfo->nz, scalar_flux,
        0, NULL, NULL);
    check_ocl(err, "Copying back scalar flux");
}
