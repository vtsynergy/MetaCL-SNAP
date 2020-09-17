
#include "scalar_flux.h"
#include "string.h"
extern cl_device_type devType;
extern char * platName;

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

    err = metacl_sweep_zero_inner_reducef_reduce_flux(context->queue, global, local, NULL, 1,&scalar_flux_event, rankinfo->nx,  rankinfo->ny,  rankinfo->nz,  problem->nang,  problem->ng, &buffers->angular_flux_in[0],  &buffers->angular_flux_in[1],&buffers->angular_flux_in[2], &buffers->angular_flux_in[3], &buffers->angular_flux_in[4], &buffers->angular_flux_in[5], &buffers->angular_flux_in[6], &buffers->angular_flux_in[7], &buffers->angular_flux_out[0], &buffers->angular_flux_out[1], &buffers->angular_flux_out[2], &buffers->angular_flux_out[3], &buffers->angular_flux_out[4], &buffers->angular_flux_out[5], &buffers->angular_flux_out[6], &buffers->angular_flux_out[7], &buffers->velocity_delta, &buffers->quad_weights, &buffers->scalar_flux, (size_t) local[0]);
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
    err = metacl_outer_zero_and_others_reduce_flux_moments(context->queue, global, local, NULL, 1,&scalar_flux_moments_event,rankinfo->nx,  rankinfo->ny,  rankinfo->nz,  problem->nang,  problem->ng, problem->cmom, &buffers->angular_flux_in[0],  &buffers->angular_flux_in[1],&buffers->angular_flux_in[2], &buffers->angular_flux_in[3], &buffers->angular_flux_in[4], &buffers->angular_flux_in[5], &buffers->angular_flux_in[6], &buffers->angular_flux_in[7], &buffers->angular_flux_out[0], &buffers->angular_flux_out[1], &buffers->angular_flux_out[2], &buffers->angular_flux_out[3], &buffers->angular_flux_out[4], &buffers->angular_flux_out[5], &buffers->angular_flux_out[6], &buffers->angular_flux_out[7], &buffers->velocity_delta,&buffers->quad_weights, &buffers->scat_coeff, &buffers->scalar_flux_moments, (size_t) local[0]);
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
