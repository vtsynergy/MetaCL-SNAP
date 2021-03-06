
#include "ocl_buffers.h"
#ifdef KERNEL_PROFILE
#include <time.h>
extern double ker_launch_over[9];
extern double ker_exec_time[9];
extern int ker_call_nums[9];
extern cl_ulong start_time, end_time;extern size_t return_bytes;
extern struct timespec start, end;
#endif //KERNEL_PROFILE

void check_device_memory_requirements(
    struct problem * problem, struct rankinfo * rankinfo,
    struct context * context)
{
    cl_int err;
    cl_ulong global;
    err = clGetDeviceInfo(context->device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &global, NULL);
    check_ocl(err, "Getting device memory size");

    cl_ulong total = 0;
    // Add up the memory requirements, in bytes.
    total += problem->nang*problem->ng*rankinfo->nx*rankinfo->ny*rankinfo->nz*8;
    total += problem->nang*problem->ng*rankinfo->nx*rankinfo->ny*rankinfo->nz*8;
    total += problem->nang*problem->ng*rankinfo->ny*rankinfo->nz;
    total += problem->nang*problem->ng*rankinfo->nx*rankinfo->nz;
    total += problem->nang*problem->ng*rankinfo->nx*rankinfo->ny;
    total += problem->ng*rankinfo->nx*rankinfo->ny*rankinfo->nz;
    if (problem->cmom-1 == 0)
        total += problem->ng*rankinfo->nx*rankinfo->ny*rankinfo->nz;
    else
        total += 1;
    total += problem->nang;
    total += problem->nang;
    total += problem->nang;
    total += problem->nang;
    total += problem->nang*problem->cmom*8;
    total += problem->ng;
    total += problem->ng*rankinfo->nx*rankinfo->ny*rankinfo->nz;
    total += problem->cmom*problem->ng*rankinfo->nx*rankinfo->ny*rankinfo->nz;
    total += problem->cmom*problem->ng*rankinfo->nx*rankinfo->ny*rankinfo->nz;
    total += problem->nmom*problem->ng*problem->ng;
    total += 1;
    total += problem->nang;
    total += problem->nang;
    total += problem->ng;
    total += problem->ng;
    total += problem->nang*problem->ng*rankinfo->nx*rankinfo->ny*rankinfo->nz;
    total *= sizeof(double);

    if (global < total)
    {
        fprintf(stderr,"Error: Device does not have enough global memory.\n");
        fprintf(stderr, "Required: %.1f GB\n", (double)total/(1024.0*1024.0*1024.0));
        fprintf(stderr, "Available: %.1f GB\n", (double)global/(1024.0*1024.0*1024.0));
        exit(EXIT_FAILURE);
    }
}

void allocate_buffers(
    struct problem * problem, struct rankinfo * rankinfo,
    struct context * context, struct buffers * buffers)
{
    cl_int err;

    // Angular flux
    for (int i = 0; i < 8; i++)
    {
        buffers->angular_flux_in[i] = clCreateBuffer(
            context->context,
            CL_MEM_READ_WRITE,
            sizeof(double)*problem->nang*problem->ng*rankinfo->nx*rankinfo->ny*rankinfo->nz,
            NULL, &err);
        check_ocl(err, "Creating an angular flux in buffer");

        buffers->angular_flux_out[i] = clCreateBuffer(
            context->context,
            CL_MEM_READ_WRITE,
            sizeof(double)*problem->nang*problem->ng*rankinfo->nx*rankinfo->ny*rankinfo->nz,
            NULL, &err);
        check_ocl(err, "Creating an angular flux out buffer");
    }

    // Edge fluxes
    buffers->flux_i = clCreateBuffer(context->context, CL_MEM_READ_WRITE,
        sizeof(double)*problem->nang*problem->ng*rankinfo->ny*rankinfo->nz,
        NULL, &err);
    check_ocl(err, "Creating flux_i buffer");

    buffers->flux_j = clCreateBuffer(context->context, CL_MEM_READ_WRITE,
        sizeof(double)*problem->nang*problem->ng*rankinfo->nx*rankinfo->nz,
        NULL, &err);
    check_ocl(err, "Creating flux_j buffer");

    buffers->flux_k = clCreateBuffer(context->context, CL_MEM_READ_WRITE,
        sizeof(double)*problem->nang*problem->ng*rankinfo->nx*rankinfo->ny,
        NULL, &err);
    check_ocl(err, "Creating flux_k buffer");

    // Scalar flux
    buffers->scalar_flux = clCreateBuffer(context->context, CL_MEM_READ_WRITE,
        sizeof(double)*problem->ng*rankinfo->nx*rankinfo->ny*rankinfo->nz,
        NULL, &err);
    check_ocl(err, "Creating scalar flux buffer");

    if (problem->cmom-1 > 0)
    {
        buffers->scalar_flux_moments = clCreateBuffer(context->context, CL_MEM_READ_WRITE,
            sizeof(double)*(problem->cmom-1)*problem->ng*rankinfo->nx*rankinfo->ny*rankinfo->nz,
            NULL, &err);
        check_ocl(err, "Creating scalar flux moments buffer");
    }
    else
    {
        // Still make the buffer so that the buffer object is valid, but as small as possible (1 byte)
        buffers->scalar_flux_moments = clCreateBuffer(context->context, CL_MEM_READ_WRITE, 1, NULL, &err);
        check_ocl(err, "Creating (small) scalar flux moments buffer");
    }

    // Weights and cosines
    buffers->quad_weights = clCreateBuffer(context->context, CL_MEM_READ_ONLY, sizeof(double)*problem->nang, NULL, &err);
    check_ocl(err, "Creating quadrature weights buffer");
    buffers->mu = clCreateBuffer(context->context, CL_MEM_READ_ONLY, sizeof(double)*problem->nang, NULL, &err);
    check_ocl(err, "Creating mu cosine buffer");
    buffers->eta = clCreateBuffer(context->context, CL_MEM_READ_ONLY, sizeof(double)*problem->nang, NULL, &err);
    check_ocl(err, "Creating eta cosine buffer");
    buffers->xi = clCreateBuffer(context->context, CL_MEM_READ_ONLY, sizeof(double)*problem->nang, NULL, &err);
    check_ocl(err, "Creating xi cosine buffer");

    // Scattering coefficient
    buffers->scat_coeff = clCreateBuffer(context->context, CL_MEM_READ_ONLY,
        sizeof(double)*problem->nang*problem->cmom*8, NULL, &err);
    check_ocl(err, "Creating scattering coefficient buffer");

    // Material cross section
    buffers->mat_cross_section = clCreateBuffer(context->context, CL_MEM_READ_ONLY,
        sizeof(double)*problem->ng, NULL, &err);
    check_ocl(err, "Creating material cross section buffer");

    // Source terms
    buffers->fixed_source = clCreateBuffer(context->context, CL_MEM_READ_ONLY,
        sizeof(double)*problem->ng*rankinfo->nx*rankinfo->ny*rankinfo->nz, NULL, &err);
    check_ocl(err, "Creating fixed source buffer");
    buffers->outer_source = clCreateBuffer(context->context, CL_MEM_READ_WRITE,
        sizeof(double)*problem->cmom*problem->ng*rankinfo->nx*rankinfo->ny*rankinfo->nz, NULL, &err);
    check_ocl(err, "Creating outer source buffer");
    buffers->inner_source = clCreateBuffer(context->context, CL_MEM_READ_WRITE,
        sizeof(double)*problem->cmom*problem->ng*rankinfo->nx*rankinfo->ny*rankinfo->nz, NULL, &err);
    check_ocl(err, "Creating inner source buffer");

    // Scattering terms
    buffers->scattering_matrix = clCreateBuffer(context->context, CL_MEM_READ_ONLY,
        sizeof(double)*problem->nmom*problem->ng*problem->ng, NULL, &err);
    check_ocl(err, "Creating scattering matrix buffer");

    // Diamond diference co-efficients
    buffers->dd_i = clCreateBuffer(context->context, CL_MEM_READ_WRITE,
        sizeof(double), NULL, &err);
    check_ocl(err, "Creating i diamond difference coefficient");
    buffers->dd_j = clCreateBuffer(context->context, CL_MEM_READ_WRITE,
        sizeof(double)*problem->nang, NULL, &err);
    check_ocl(err, "Creating j diamond difference coefficient");
    buffers->dd_k = clCreateBuffer(context->context, CL_MEM_READ_WRITE,
        sizeof(double)*problem->nang, NULL, &err);
    check_ocl(err, "Creating k diamond difference coefficient");

    // Velocities
    buffers->velocities = clCreateBuffer(context->context, CL_MEM_READ_ONLY, sizeof(double)*problem->ng, NULL, &err);
    check_ocl(err, "Creating velocity buffer");
    buffers->velocity_delta = clCreateBuffer(context->context, CL_MEM_READ_WRITE, sizeof(double)*problem->ng, NULL, &err);
    check_ocl(err, "Creating velocity delta buffer");

    // Denominator array
    buffers->denominator = clCreateBuffer(context->context, CL_MEM_READ_WRITE,
        sizeof(double)*problem->nang*problem->ng*rankinfo->nx*rankinfo->ny*rankinfo->nz, NULL, &err);
    check_ocl(err, "Creating denominator buffer");

}


void zero_buffer_inner(struct context * context, cl_mem buffer, size_t offset, size_t size)
{
    cl_int err;
    size_t enq_off[3]={offset,0,0};
    size_t global[3] = {size,1,1};
    size_t local[3] = {0,0,0};
    cl_event temp2;
    int async = 1;
#ifdef KERNEL_PROFILE
    err=clFinish(context->queue);
    err=clFinish(context->copy_queue);
    async = 0;

    clock_gettime(CLOCK_REALTIME, &start);
#endif //KERNEL_PROFILE
#ifdef METACL
    err = metacl_sweep_zero_inner_reducef_zero_buffer(context->queue, global, local, enq_off, async, &temp2, &buffer);
#else
    err = clSetKernelArg(context->kernels.zero_buffer_inner, 0, sizeof(cl_mem), &buffer);
    check_ocl(err, "Setting buffer zero kernel argument");
    err = clEnqueueNDRangeKernel(context->queue,
        context->kernels.zero_buffer_inner,
        1, &offset, &size, NULL, 0, NULL, &temp2);
    if (async == 0) clFinish(context->queue);
#endif //METACL
#ifdef KERNEL_PROFILE
    clock_gettime(CLOCK_REALTIME, &end);
    ker_launch_over[8]+=( end.tv_sec - start.tv_sec ) + ( end.tv_nsec - start.tv_nsec )/ BILLION;
    err = clGetEventProfilingInfo(temp2,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),  &start_time,&return_bytes);
    err = clGetEventProfilingInfo(temp2,CL_PROFILING_COMMAND_END,sizeof(cl_ulong), &end_time,&return_bytes);
    ker_exec_time[8]+=(double)(end_time-start_time)/BILLION;
    temp2=NULL;
    ker_call_nums[8]++;
#endif //KERNEL_PROFILE
    check_ocl(err, "Enqueueing buffer zero inner kernel");
}

void zero_buffer(struct context * context, cl_mem buffer, size_t offset, size_t size)
{
    cl_int err;
    size_t global[3] = {size,1,1};
    size_t local[3] = {0,0,0};
    size_t enq_off[3]={offset,0,0};
    cl_event temp2;
    int async = 1;
#ifdef KERNEL_PROFILE
    err=clFinish(context->queue);
    err=clFinish(context->copy_queue);
    async = 0;

    clock_gettime(CLOCK_REALTIME, &start);
#endif //KERNEL_PROFILE
#ifdef METACL
    err = metacl_outer_zero_and_others_zero_buffer(context->queue, global, local, enq_off, async, &temp2, &buffer);
#else
    err = clSetKernelArg(context->kernels.zero_buffer, 0, sizeof(cl_mem), &buffer);
    check_ocl(err, "Setting buffer zero kernel argument");
    err = clEnqueueNDRangeKernel(context->queue,
        context->kernels.zero_buffer,
        1, &offset, &size, NULL, 0, NULL, &temp2);
    if (async == 0) clFinish(context->queue);
#endif //METACL
#ifdef KERNEL_PROFILE
    clock_gettime(CLOCK_REALTIME, &end);
    ker_launch_over[8]+=( end.tv_sec - start.tv_sec ) + ( end.tv_nsec - start.tv_nsec )/ BILLION;
    err = clGetEventProfilingInfo(temp2,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),  &start_time,&return_bytes);
    err = clGetEventProfilingInfo(temp2,CL_PROFILING_COMMAND_END,sizeof(cl_ulong), &end_time,&return_bytes);
    ker_exec_time[8]+=(double)(end_time-start_time)/BILLION;
    temp2=NULL;
    ker_call_nums[8]++;
#endif //KERNEL_PROFILE
    check_ocl(err, "Enqueueing buffer zero kernel");
}

void swap_angular_flux_buffers(struct buffers * buffers)
{
    for (int i = 0; i < 8; i++)
    {
        cl_mem tmp;
        tmp = buffers->angular_flux_in[i];
        buffers->angular_flux_in[i] = buffers->angular_flux_out[i];
        buffers->angular_flux_out[i] = tmp;
    }
}
