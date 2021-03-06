
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

#ifdef METACL
    meta_set_acc(deviceIndex, metaModePreferOpenCL); //Must be set to OpenCL, don't need a device since we will override
    meta_get_state_OpenCL(&context->platform ,&context->device, &context->context, &context->queue);
    context->copy_queue = clCreateCommandQueue(context->context, context->device, CL_QUEUE_PROFILING_ENABLE, &err);
    check_ocl(err, "Creating copy command queue");
#else
    // Get list of platforms
    cl_uint num_platforms;
    err = clGetPlatformIDs(0, NULL, &num_platforms);
    check_ocl(err, "Getting number of platforms");
    cl_platform_id *platforms = malloc(num_platforms*sizeof(cl_platform_id));
    err = clGetPlatformIDs(num_platforms, platforms, &num_platforms);
    check_ocl(err, "Getting platforms");

    // Get a GPU device
    cl_device_type type = CL_DEVICE_TYPE_ALL;
    cl_uint num_devices = 0;
    cl_device_id devices[MAX_DEVICES];
    for (unsigned int i = 0; i < num_platforms; i++)
    {
        cl_uint num;
        err = clGetDeviceIDs(platforms[i], type, MAX_DEVICES-num_devices, devices+num_devices, &num);
        check_ocl(err, "Getting devices");
        num_devices += num;
    }
    free(platforms);
    if (num_devices == 0)
        check_ocl(CL_DEVICE_NOT_FOUND, "Cannot find a GPU device");
    if (!multigpu)
    {
        // Just pick the first GPU device
        context->device = devices[deviceIndex];
    #ifdef __APPLE__
        // If we on my MacBook we need the second GPU (the discrete one)
        context->device = devices[1];
    #endif
    }
    else
    {
        // NOTE THIS ONLY WORKS FOR CRAY CS-STORM NODES
        context->device = devices[rank];
    }

    // Create a context and command queue for the device
    context->context = clCreateContext(0, 1, &context->device, NULL, NULL, &err);
    check_ocl(err, "Creating context");

    if (profiling)
    {
        context->queue = clCreateCommandQueue(context->context, context->device, CL_QUEUE_PROFILING_ENABLE, &err);
        check_ocl(err, "Creating command queue");
        context->copy_queue = clCreateCommandQueue(context->context, context->device, CL_QUEUE_PROFILING_ENABLE, &err);
        check_ocl(err, "Creating copy command queue");
    }
    else
    {
        context->queue = clCreateCommandQueue(context->context, context->device, 0, &err);
        check_ocl(err, "Creating command queue");
        context->copy_queue = clCreateCommandQueue(context->context, context->device, 0, &err);
        check_ocl(err, "Creating copy command queue");
    }

    err = clGetDeviceInfo(context->device, CL_DEVICE_PLATFORM, sizeof(cl_platform_id), &context->platform, NULL);
    check_ocl(err, "Querying device platform");
#endif //METACL

    // Detect device type and vendor
    err = clGetDeviceInfo(context->device, CL_DEVICE_TYPE, sizeof(cl_device_type), &devType, NULL);
    check_ocl(err, "Querying device type");
    size_t name_size;
    err = clGetPlatformInfo(context->platform, CL_PLATFORM_NAME, 0, NULL, &name_size);
    check_ocl(err, "Querying platform name length");
    platName = (char *)calloc(sizeof(char), name_size + 1);
    err = clGetPlatformInfo(context->platform, CL_PLATFORM_NAME, name_size + 1, platName, NULL);
    check_ocl(err, "Querying platform name");

    char *options = "-cl-mad-enable -cl-fast-relaxed-math";
#ifdef METACL
    __metacl_sweep_zero_inner_reducef_custom_args=options;
    __metacl_outer_zero_and_others_custom_args=options;
     meta_register_module(&metacl_metacl_module_registry);
#else
    // Create program
    #define clBinaryProg(name) {\
       printf("Loading "#name".aocx\n"); \
FILE * f = fopen(#name".aocx", "r"); \
       fseek(f, 0, SEEK_END); \
       size_t len = (size_t) ftell(f); \
       const unsigned char * progSrc = (const unsigned char *) malloc(sizeof(char) * len); \
       rewind(f); \
       fread((void *) progSrc, len, 1, f); \
       fclose(f); \
       cl_int err; \
       name = clCreateProgramWithBinary(context->context, 1, &context->device , &len, &progSrc, NULL, &err);}

    cl_program outer_zero_and_others;
    cl_program sweep_zero_inner_reducef;
    cl_int build_err;
    if (devType & CL_DEVICE_TYPE_ACCELERATOR && (strstr(platName, "Intel(R) FPGA")!=NULL || strstr(platName, "Altera")!=NULL)) {
        clBinaryProg(outer_zero_and_others);
        build_err = clBuildProgram(outer_zero_and_others, 1, &context->device, options, NULL, NULL);
    } else {
        context->program = clCreateProgramWithSource(context->context, sizeof(ocl_kernels)/sizeof(char*), ocl_kernels, NULL, &err);
        check_ocl(err, "Creating program");
        // Build program
        build_err = clBuildProgram(context->program, 1, &context->device, options, NULL, NULL);
        outer_zero_and_others = context->program;
    }

    if (build_err == CL_BUILD_PROGRAM_FAILURE)
    {
        size_t log_size;
        err = clGetProgramBuildInfo(outer_zero_and_others, context->device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        check_ocl(err, "Getting build log size");
        char *build_log = malloc(log_size);
        err = clGetProgramBuildInfo(outer_zero_and_others, context->device, CL_PROGRAM_BUILD_LOG, log_size, build_log, NULL);
        check_ocl(err, "Getting build log");
        fprintf(stderr, "OpenCL Build log: %s\n", build_log);
        free(build_log);
    }
    check_ocl(build_err, "Building program");

    // Create the kernels
    context->kernels.calc_velocity_delta = clCreateKernel(outer_zero_and_others, "calc_velocity_delta", &err);
    check_ocl(err, "Creating velocity delta kernel");

    context->kernels.calc_dd_coeff = clCreateKernel(outer_zero_and_others, "calc_dd_coeff", &err);
    check_ocl(err, "Creating diamond difference constants kernel");

    context->kernels.calc_denominator = clCreateKernel(outer_zero_and_others, "calc_denominator", &err);
    check_ocl(err, "Creating denominator kernel");

    context->kernels.zero_buffer = clCreateKernel(outer_zero_and_others, "zero_buffer", &err);
    check_ocl(err, "Creating buffer zeroing kernel");

    context->kernels.reduce_flux_moments = clCreateKernel(outer_zero_and_others, "reduce_flux_moments", &err);
    check_ocl(err, "Creating scalar flux moments reduction kernel");

    context->kernels.outer_source = clCreateKernel(outer_zero_and_others, "calc_outer_source", &err);
    check_ocl(err, "Creating outer source kernel");

    if (devType & CL_DEVICE_TYPE_ACCELERATOR && (strstr(platName, "Intel(R) FPGA")!=NULL || strstr(platName, "Altera")!=NULL)) {
        clBinaryProg(sweep_zero_inner_reducef);
        build_err = clBuildProgram(sweep_zero_inner_reducef, 1, &context->device, options, NULL, NULL);
        if (build_err == CL_BUILD_PROGRAM_FAILURE)
        {
            size_t log_size;
            err = clGetProgramBuildInfo(sweep_zero_inner_reducef, context->device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
            check_ocl(err, "Getting build log size");
            char *build_log = malloc(log_size);
            err = clGetProgramBuildInfo(sweep_zero_inner_reducef, context->device, CL_PROGRAM_BUILD_LOG, log_size, build_log, NULL);
            check_ocl(err, "Getting build log");
            fprintf(stderr, "OpenCL Build log: %s\n", build_log);
            free(build_log);
        }
        check_ocl(build_err, "Building program");
    } else {
        sweep_zero_inner_reducef = context->program;
    }
    context->kernels.inner_source = clCreateKernel(sweep_zero_inner_reducef, "calc_inner_source", &err);
    check_ocl(err, "Creating inner source kernel");

    context->kernels.sweep_plane = clCreateKernel(sweep_zero_inner_reducef, "sweep_plane", &err);
    check_ocl(err, "Creating sweep plane kernel");

    context->kernels.reduce_flux = clCreateKernel(sweep_zero_inner_reducef, "reduce_flux", &err);
    check_ocl(err, "Creating scalar flux reduction kernel");

    context->kernels.zero_buffer_inner = clCreateKernel(sweep_zero_inner_reducef, "zero_buffer", &err);
    check_ocl(err, "Creating buffer zeroing kernel");
#endif //METACL
}

void release_context(struct context * context)
{
#ifdef METACL
    meta_deregister_module(&metacl_metacl_module_registry);
#else
    cl_int err;
    err = clReleaseProgram(context->program);
    check_ocl(err, "Releasing program");

#ifdef CL_VERSION_1_2
    err = clReleaseDevice(context->device);
    check_ocl(err, "Releasing device");
#endif

    err = clReleaseCommandQueue(context->queue);
    check_ocl(err, "Releasing command queue");

    err = clReleaseContext(context->context);
    check_ocl(err, "Releasing context");
#endif //METACL

    free(platName);
}
