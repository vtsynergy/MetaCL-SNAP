
#pragma once

#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif


/** \file
* \brief OpenCL routines
*/

/**
\brief Structure to contain OpenCL context, command queue, device and program objects
*/
struct kernels
{
    cl_kernel calc_velocity_delta;
    cl_kernel calc_dd_coeff;
    cl_kernel calc_denominator;
    cl_kernel zero_buffer;
    cl_kernel zero_buffer_3D;
    cl_kernel outer_source;
    cl_kernel inner_source;
    cl_kernel sweep_plane;
    cl_kernel reduce_flux;
    cl_kernel reduce_flux_moments;
};
struct context
{
    cl_platform_id platform;
    cl_context context;
    cl_device_id device;
    cl_command_queue queue;
    cl_program program;
    struct kernels kernels;
};


void check_ocl(const cl_int err, const char *msg);

/** \brief Initilise the OpenCL device, context, command queue and program */
void init_ocl(struct context * context);

/** \brief Release the OpenCL objects held in the context structure */
void release_context(struct context * context);
