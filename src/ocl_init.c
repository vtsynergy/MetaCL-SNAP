
#include <string.h>
#include "profiler.h"
#include "ocl_global.h"
#include "ocl_kernels.h"

cl_device_type devType;
char * platName;
extern int deviceIndex;
#define MAX_DEVICES 16

void check_ocl_error(const cl_int err, const char *msg, const int line, const char * file)
{
    if (err != CL_SUCCESS)
    {
        fprintf(stderr, "OpenCL Error in %s line %d: %d. %s\n", file, line, err, msg);
        exit(EXIT_FAILURE);
    }
}

void init_ocl(struct context * context, const bool multigpu, const int rank)
{
    cl_int err;

    meta_set_acc(deviceIndex, metaModePreferOpenCL); //Must be set to OpenCL, don't need a device since we will override
    meta_get_state_OpenCL(&context->platform ,&context->device, &context->context, &context->queue);
    context->copy_queue = clCreateCommandQueue(context->context, context->device, CL_QUEUE_PROFILING_ENABLE, &err);
    check_ocl(err, "Creating copy command queue");

    // Build program
    char *options = "-cl-mad-enable -cl-fast-relaxed-math";
    __metacl_sweep_zero_inner_reducef_custom_args=options;
    __metacl_outer_zero_and_others_custom_args=options;
     meta_register_module(&metacl_metacl_module_registry);

    // Detect device type and vendor
    err = clGetDeviceInfo(context->device, CL_DEVICE_TYPE, sizeof(cl_device_type), &devType, NULL);
    check_ocl(err, "Querying device type");
    size_t name_size;
    err = clGetPlatformInfo(context->platform, CL_PLATFORM_NAME, 0, NULL, &name_size);
    check_ocl(err, "Querying platform name length");
    platName = (char *)calloc(sizeof(char), name_size + 1);
    err = clGetPlatformInfo(context->platform, CL_PLATFORM_NAME, name_size + 1, platName, NULL);
    check_ocl(err, "Querying platform name");
}

void release_context(struct context * context)
{
    meta_deregister_module(&metacl_metacl_module_registry);

    free(platName);
}
