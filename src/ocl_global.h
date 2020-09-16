
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include "metamorph.h"
#include "metacl_module.h"

/** \file
* \brief Basic OpenCL routines and global structures
*/

/**
\brief Structure to contain OpenCL context, command queue, device and program objects
*/
struct context
{
    /** \brief The platform */
    cl_platform_id platform;

    /** \brief The context */
    cl_context context;

    /** \brief The device */
    cl_device_id device;

    /** \brief The (compute) command queue */
    cl_command_queue queue;

    /** \brief The (copy) command queue used for copying back scalar flux only */
    cl_command_queue copy_queue;
};

/**
@{
\brief Check for OpenCL errors */
void check_ocl_error(const cl_int err, const char *msg, const int line, const char * file);
#define check_ocl(err, msg) check_ocl_error((err),(msg),__LINE__,__FILE__)
/** @}*/

/** \brief Initilise the OpenCL device, context, command queue and program */
void init_ocl(struct context * context, const bool multigpu, const int rank);

/** \brief Release the OpenCL objects held in the context structure */
void release_context(struct context * context);
