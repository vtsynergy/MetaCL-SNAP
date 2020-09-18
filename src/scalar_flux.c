
#include "scalar_flux.h"
#include "string.h"
extern cl_device_type devType;
extern char * platName;

#ifdef KERNEL_PROFILE
extern double ker_launch_over[9];
extern double ker_exec_time[9];
extern int ker_call_nums[9];
extern cl_ulong start_time, end_time;extern size_t return_bytes;
extern struct timespec start, end;
extern size_t null_offset[3];
#endif

void compute_scalar_flux(
    struct problem * problem,
    struct rankinfo * rankinfo,
    struct context * context,
    struct buffers * buffers
    )
{

    // get closest power of 2 to nang
    size_t power = 1 << (unsigned int)ceil(log2((double)problem->nang));
    if (devType & CL_DEVICE_TYPE_ACCELERATOR && (strstr(platName, "Intel(R) FPGA")!=NULL || strstr(platName, "Altera")!=NULL)) {
	if (power > 128) power = 128;
    }

    a_dim3 global ={power * problem->ng, rankinfo->nx*rankinfo->ny*rankinfo->nz,1};
    a_dim3 local ={power, 1, 1};

    cl_int err;
#ifdef KERNEL_PROFILE
    err=clFinish(context->queue);
    err=clFinish(context->copy_queue);

    clock_gettime(CLOCK_REALTIME, &start);
    err = metacl_sweep_zero_inner_reducef_reduce_flux(context->queue, global, local, NULL, 0,&scalar_flux_event,rankinfo->nx,  rankinfo->ny,  rankinfo->nz,  problem->nang,  problem->ng, &buffers->angular_flux_in[0],  &buffers->angular_flux_in[1],&buffers->angular_flux_in[2], &buffers->angular_flux_in[3], &buffers->angular_flux_in[4], &buffers->angular_flux_in[5], &buffers->angular_flux_in[6], &buffers->angular_flux_in[7], &buffers->angular_flux_out[0], &buffers->angular_flux_out[1], &buffers->angular_flux_out[2], &buffers->angular_flux_out[3], &buffers->angular_flux_out[4], &buffers->angular_flux_out[5], &buffers->angular_flux_out[6], &buffers->angular_flux_out[7], &buffers->velocity_delta, &buffers->quad_weights, &buffers->scalar_flux, (size_t) local[0]);
    clock_gettime(CLOCK_REALTIME, &end);
    ker_launch_over[5]+=( end.tv_sec - start.tv_sec ) + ( end.tv_nsec - start.tv_nsec )/ BILLION;
    err = clGetEventProfilingInfo(scalar_flux_event,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),  &start_time,&return_bytes);
    err = clGetEventProfilingInfo(scalar_flux_event,CL_PROFILING_COMMAND_END,sizeof(cl_ulong), &end_time,&return_bytes);
    ker_exec_time[5]+=(double)(end_time-start_time)/BILLION;
    ker_call_nums[5]++;
#else
    err = metacl_sweep_zero_inner_reducef_reduce_flux(context->queue, global, local, NULL, 1,&scalar_flux_event, rankinfo->nx,  rankinfo->ny,  rankinfo->nz,  problem->nang,  problem->ng, &buffers->angular_flux_in[0],  &buffers->angular_flux_in[1],&buffers->angular_flux_in[2], &buffers->angular_flux_in[3], &buffers->angular_flux_in[4], &buffers->angular_flux_in[5], &buffers->angular_flux_in[6], &buffers->angular_flux_in[7], &buffers->angular_flux_out[0], &buffers->angular_flux_out[1], &buffers->angular_flux_out[2], &buffers->angular_flux_out[3], &buffers->angular_flux_out[4], &buffers->angular_flux_out[5], &buffers->angular_flux_out[6], &buffers->angular_flux_out[7], &buffers->velocity_delta, &buffers->quad_weights, &buffers->scalar_flux, (size_t) local[0]);
#endif //KERNEL_PROFILE
    check_ocl(err, "Enqueueing scalar flux reduction kernel");
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
    if (devType & CL_DEVICE_TYPE_ACCELERATOR && (strstr(platName, "Intel(R) FPGA")!=NULL || strstr(platName, "Altera")!=NULL)) {
	if (power > 128) power = 128;
    }

    a_dim3 global ={power*problem->ng, rankinfo->nx*rankinfo->ny*rankinfo->nz,1};
    a_dim3 local ={power, 1, 1};
    cl_int err;
#ifdef KERNEL_PROFILE
    err=clFinish(context->queue);
    err=clFinish(context->copy_queue);

    clock_gettime(CLOCK_REALTIME, &start);
    err = metacl_outer_zero_and_others_reduce_flux_moments(context->queue, global, local, NULL,0,&scalar_flux_moments_event,rankinfo->nx,  rankinfo->ny,  rankinfo->nz,  problem->nang,  problem->ng, problem->cmom, &buffers->angular_flux_in[0],  &buffers->angular_flux_in[1],&buffers->angular_flux_in[2], &buffers->angular_flux_in[3], &buffers->angular_flux_in[4], &buffers->angular_flux_in[5], &buffers->angular_flux_in[6], &buffers->angular_flux_in[7], &buffers->angular_flux_out[0], &buffers->angular_flux_out[1], &buffers->angular_flux_out[2], &buffers->angular_flux_out[3], &buffers->angular_flux_out[4], &buffers->angular_flux_out[5], &buffers->angular_flux_out[6], &buffers->angular_flux_out[7], &buffers->velocity_delta,&buffers->quad_weights, &buffers->scat_coeff, &buffers->scalar_flux_moments, (size_t) local[0]);
clock_gettime(CLOCK_REALTIME, &end);
    ker_launch_over[6]+=( end.tv_sec - start.tv_sec ) + ( end.tv_nsec - start.tv_nsec )/ BILLION;
    err = clGetEventProfilingInfo(scalar_flux_moments_event,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),  &start_time,&return_bytes);
    err = clGetEventProfilingInfo(scalar_flux_moments_event,CL_PROFILING_COMMAND_END,sizeof(cl_ulong), &end_time,&return_bytes);
    ker_exec_time[6]+=(double)(end_time-start_time)/BILLION;
    ker_call_nums[6]++;
#else
    err = metacl_outer_zero_and_others_reduce_flux_moments(context->queue, global, local, NULL, 1,&scalar_flux_moments_event,rankinfo->nx,  rankinfo->ny,  rankinfo->nz,  problem->nang,  problem->ng, problem->cmom, &buffers->angular_flux_in[0],  &buffers->angular_flux_in[1],&buffers->angular_flux_in[2], &buffers->angular_flux_in[3], &buffers->angular_flux_in[4], &buffers->angular_flux_in[5], &buffers->angular_flux_in[6], &buffers->angular_flux_in[7], &buffers->angular_flux_out[0], &buffers->angular_flux_out[1], &buffers->angular_flux_out[2], &buffers->angular_flux_out[3], &buffers->angular_flux_out[4], &buffers->angular_flux_out[5], &buffers->angular_flux_out[6], &buffers->angular_flux_out[7], &buffers->velocity_delta,&buffers->quad_weights, &buffers->scat_coeff, &buffers->scalar_flux_moments, (size_t) local[0]);
#endif //KERNEL_PROFILE
    check_ocl(err, "Enqueueing scalar flux moments reduction kernel");
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
