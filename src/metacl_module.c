//Force MetaMorph to include the OpenCL code
#ifndef WITH_OPENCL
#define WITH_OPENCL
#endif
#include "metamorph.h"
#include "metacl_module.h"
extern cl_context meta_context;
extern cl_command_queue meta_queue;
extern cl_device_id meta_device;
//TODO: Expose this with a function (with safety checks) rather than a variable
const char * __meta_gen_opencl_calc_dd_coeff_custom_args = NULL;
//TODO: Expose this with a function (with safety checks) rather than a variable
const char * __meta_gen_opencl_calc_denominator_custom_args = NULL;
//TODO: Expose this with a function (with safety checks) rather than a variable
const char * __meta_gen_opencl_calc_velocity_delta_custom_args = NULL;
//TODO: Expose this with a function (with safety checks) rather than a variable
const char * __meta_gen_opencl_inner_source_custom_args = NULL;
//TODO: Expose this with a function (with safety checks) rather than a variable
const char * __meta_gen_opencl_outer_source_custom_args = NULL;
//TODO: Expose this with a function (with safety checks) rather than a variable
const char * __meta_gen_opencl_reduce_flux_custom_args = NULL;
//TODO: Expose this with a function (with safety checks) rather than a variable
const char * __meta_gen_opencl_reduce_flux_moments_custom_args = NULL;
//TODO: Expose this with a function (with safety checks) rather than a variable
const char * __meta_gen_opencl_sweep_plane_custom_args = NULL;
//TODO: Expose this with a function (with safety checks) rather than a variable
const char * __meta_gen_opencl_zero_buffer_custom_args = NULL;
struct __meta_gen_opencl_metacl_module_frame * __meta_gen_opencl_metacl_module_current_frame = NULL;

struct __meta_gen_opencl_metacl_module_frame * __meta_gen_opencl_metacl_module_lookup_frame(cl_command_queue queue) {
  struct __meta_gen_opencl_metacl_module_frame * current = __meta_gen_opencl_metacl_module_current_frame;
  while (current != NULL) {
    if (current->queue == queue) break;
    current = current->next_frame;
  }
  return current;
}
a_module_record * meta_gen_opencl_metacl_module_registration = NULL;
a_module_record * meta_gen_opencl_metacl_module_registry(a_module_record * record) {
  if (record == NULL) return meta_gen_opencl_metacl_module_registration;
  a_module_record * old_registration = meta_gen_opencl_metacl_module_registration;
  if (old_registration == NULL) {
    record->implements = module_implements_opencl;
    record->module_init = &meta_gen_opencl_metacl_module_init;
    record->module_deinit = &meta_gen_opencl_metacl_module_deinit;
    record->module_registry_func = &meta_gen_opencl_metacl_module_registry;
    meta_gen_opencl_metacl_module_registration = record;
  }
  if (old_registration != NULL && old_registration != record) return record;
  if (old_registration == record) meta_gen_opencl_metacl_module_registration = NULL;
  return old_registration;
}
void meta_gen_opencl_metacl_module_init() {
  cl_int buildError, createError;
  if (meta_gen_opencl_metacl_module_registration == NULL) {
    meta_register_module(&meta_gen_opencl_metacl_module_registry);
    return;
  }
  if (meta_context == NULL) metaOpenCLFallBack();
  struct __meta_gen_opencl_metacl_module_frame * new_frame = (struct __meta_gen_opencl_metacl_module_frame *) calloc(1, sizeof(struct __meta_gen_opencl_metacl_module_frame));
  new_frame->next_frame = __meta_gen_opencl_metacl_module_current_frame;
  new_frame->device = meta_device;
  new_frame->queue = meta_queue;
  new_frame->context = meta_context;
  __meta_gen_opencl_metacl_module_current_frame = new_frame;
  meta_cl_device_vendor vendor = metaOpenCLDetectDevice(new_frame->device);
  if ((vendor & meta_cl_device_is_accel) && ((vendor & meta_cl_device_vendor_mask) == meta_cl_device_vendor_intelfpga)) {
    __meta_gen_opencl_metacl_module_current_frame->calc_dd_coeff_progLen = metaOpenCLLoadProgramSource("calc_dd_coeff.aocx", &__meta_gen_opencl_metacl_module_current_frame->calc_dd_coeff_progSrc);
    if (__meta_gen_opencl_metacl_module_current_frame->calc_dd_coeff_progLen != -1)
      __meta_gen_opencl_metacl_module_current_frame->calc_dd_coeff_prog = clCreateProgramWithBinary(meta_context, 1, &meta_device, &__meta_gen_opencl_metacl_module_current_frame->calc_dd_coeff_progLen, (const unsigned char **)&__meta_gen_opencl_metacl_module_current_frame->calc_dd_coeff_progSrc, NULL, &buildError);
  } else {
    __meta_gen_opencl_metacl_module_current_frame->calc_dd_coeff_progLen = metaOpenCLLoadProgramSource("calc_dd_coeff.cl", &__meta_gen_opencl_metacl_module_current_frame->calc_dd_coeff_progSrc);
    if (__meta_gen_opencl_metacl_module_current_frame->calc_dd_coeff_progLen != -1)
      __meta_gen_opencl_metacl_module_current_frame->calc_dd_coeff_prog = clCreateProgramWithSource(meta_context, 1, &__meta_gen_opencl_metacl_module_current_frame->calc_dd_coeff_progSrc, &__meta_gen_opencl_metacl_module_current_frame->calc_dd_coeff_progLen, &buildError);
  }
  if (__meta_gen_opencl_metacl_module_current_frame->calc_dd_coeff_progLen != -1) {
    if (buildError != CL_SUCCESS) fprintf(stderr, "OpenCL program creation error %d at %s:%d\n", buildError, __FILE__, __LINE__);
    buildError = clBuildProgram(__meta_gen_opencl_metacl_module_current_frame->calc_dd_coeff_prog, 1, &meta_device, __meta_gen_opencl_calc_dd_coeff_custom_args ? __meta_gen_opencl_calc_dd_coeff_custom_args : "", NULL, NULL);
    if (buildError != CL_SUCCESS) {
      size_t logsize = 0;
      clGetProgramBuildInfo(__meta_gen_opencl_metacl_module_current_frame->calc_dd_coeff_prog, meta_device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logsize);
      char * buildLog = (char *) malloc(sizeof(char) * (logsize + 1));
      clGetProgramBuildInfo(__meta_gen_opencl_metacl_module_current_frame->calc_dd_coeff_prog, meta_device, CL_PROGRAM_BUILD_LOG, logsize, buildLog, NULL);
      if (buildError != CL_SUCCESS) fprintf(stderr, "OpenCL program build error %d at %s:%d\n", buildError, __FILE__, __LINE__);
      fprintf(stderr, "Build Log:\n%s\n", buildLog);
      free(buildLog);
    } else {
      __meta_gen_opencl_metacl_module_current_frame->calc_dd_coeff_init = 1;
    }
    __meta_gen_opencl_metacl_module_current_frame->calc_dd_coeff_calc_dd_coeff_kernel = clCreateKernel(__meta_gen_opencl_metacl_module_current_frame->calc_dd_coeff_prog, "calc_dd_coeff", &createError);
    if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
  }
  if ((vendor & meta_cl_device_is_accel) && ((vendor & meta_cl_device_vendor_mask) == meta_cl_device_vendor_intelfpga)) {
    __meta_gen_opencl_metacl_module_current_frame->calc_denominator_progLen = metaOpenCLLoadProgramSource("calc_denominator.aocx", &__meta_gen_opencl_metacl_module_current_frame->calc_denominator_progSrc);
    if (__meta_gen_opencl_metacl_module_current_frame->calc_denominator_progLen != -1)
      __meta_gen_opencl_metacl_module_current_frame->calc_denominator_prog = clCreateProgramWithBinary(meta_context, 1, &meta_device, &__meta_gen_opencl_metacl_module_current_frame->calc_denominator_progLen, (const unsigned char **)&__meta_gen_opencl_metacl_module_current_frame->calc_denominator_progSrc, NULL, &buildError);
  } else {
    __meta_gen_opencl_metacl_module_current_frame->calc_denominator_progLen = metaOpenCLLoadProgramSource("calc_denominator.cl", &__meta_gen_opencl_metacl_module_current_frame->calc_denominator_progSrc);
    if (__meta_gen_opencl_metacl_module_current_frame->calc_denominator_progLen != -1)
      __meta_gen_opencl_metacl_module_current_frame->calc_denominator_prog = clCreateProgramWithSource(meta_context, 1, &__meta_gen_opencl_metacl_module_current_frame->calc_denominator_progSrc, &__meta_gen_opencl_metacl_module_current_frame->calc_denominator_progLen, &buildError);
  }
  if (__meta_gen_opencl_metacl_module_current_frame->calc_denominator_progLen != -1) {
    if (buildError != CL_SUCCESS) fprintf(stderr, "OpenCL program creation error %d at %s:%d\n", buildError, __FILE__, __LINE__);
    buildError = clBuildProgram(__meta_gen_opencl_metacl_module_current_frame->calc_denominator_prog, 1, &meta_device, __meta_gen_opencl_calc_denominator_custom_args ? __meta_gen_opencl_calc_denominator_custom_args : "", NULL, NULL);
    if (buildError != CL_SUCCESS) {
      size_t logsize = 0;
      clGetProgramBuildInfo(__meta_gen_opencl_metacl_module_current_frame->calc_denominator_prog, meta_device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logsize);
      char * buildLog = (char *) malloc(sizeof(char) * (logsize + 1));
      clGetProgramBuildInfo(__meta_gen_opencl_metacl_module_current_frame->calc_denominator_prog, meta_device, CL_PROGRAM_BUILD_LOG, logsize, buildLog, NULL);
      if (buildError != CL_SUCCESS) fprintf(stderr, "OpenCL program build error %d at %s:%d\n", buildError, __FILE__, __LINE__);
      fprintf(stderr, "Build Log:\n%s\n", buildLog);
      free(buildLog);
    } else {
      __meta_gen_opencl_metacl_module_current_frame->calc_denominator_init = 1;
    }
    __meta_gen_opencl_metacl_module_current_frame->calc_denominator_calc_denominator_kernel = clCreateKernel(__meta_gen_opencl_metacl_module_current_frame->calc_denominator_prog, "calc_denominator", &createError);
    if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
  }
  if ((vendor & meta_cl_device_is_accel) && ((vendor & meta_cl_device_vendor_mask) == meta_cl_device_vendor_intelfpga)) {
    __meta_gen_opencl_metacl_module_current_frame->calc_velocity_delta_progLen = metaOpenCLLoadProgramSource("calc_velocity_delta.aocx", &__meta_gen_opencl_metacl_module_current_frame->calc_velocity_delta_progSrc);
    if (__meta_gen_opencl_metacl_module_current_frame->calc_velocity_delta_progLen != -1)
      __meta_gen_opencl_metacl_module_current_frame->calc_velocity_delta_prog = clCreateProgramWithBinary(meta_context, 1, &meta_device, &__meta_gen_opencl_metacl_module_current_frame->calc_velocity_delta_progLen, (const unsigned char **)&__meta_gen_opencl_metacl_module_current_frame->calc_velocity_delta_progSrc, NULL, &buildError);
  } else {
    __meta_gen_opencl_metacl_module_current_frame->calc_velocity_delta_progLen = metaOpenCLLoadProgramSource("calc_velocity_delta.cl", &__meta_gen_opencl_metacl_module_current_frame->calc_velocity_delta_progSrc);
    if (__meta_gen_opencl_metacl_module_current_frame->calc_velocity_delta_progLen != -1)
      __meta_gen_opencl_metacl_module_current_frame->calc_velocity_delta_prog = clCreateProgramWithSource(meta_context, 1, &__meta_gen_opencl_metacl_module_current_frame->calc_velocity_delta_progSrc, &__meta_gen_opencl_metacl_module_current_frame->calc_velocity_delta_progLen, &buildError);
  }
  if (__meta_gen_opencl_metacl_module_current_frame->calc_velocity_delta_progLen != -1) {
    if (buildError != CL_SUCCESS) fprintf(stderr, "OpenCL program creation error %d at %s:%d\n", buildError, __FILE__, __LINE__);
    buildError = clBuildProgram(__meta_gen_opencl_metacl_module_current_frame->calc_velocity_delta_prog, 1, &meta_device, __meta_gen_opencl_calc_velocity_delta_custom_args ? __meta_gen_opencl_calc_velocity_delta_custom_args : "", NULL, NULL);
    if (buildError != CL_SUCCESS) {
      size_t logsize = 0;
      clGetProgramBuildInfo(__meta_gen_opencl_metacl_module_current_frame->calc_velocity_delta_prog, meta_device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logsize);
      char * buildLog = (char *) malloc(sizeof(char) * (logsize + 1));
      clGetProgramBuildInfo(__meta_gen_opencl_metacl_module_current_frame->calc_velocity_delta_prog, meta_device, CL_PROGRAM_BUILD_LOG, logsize, buildLog, NULL);
      if (buildError != CL_SUCCESS) fprintf(stderr, "OpenCL program build error %d at %s:%d\n", buildError, __FILE__, __LINE__);
      fprintf(stderr, "Build Log:\n%s\n", buildLog);
      free(buildLog);
    } else {
      __meta_gen_opencl_metacl_module_current_frame->calc_velocity_delta_init = 1;
    }
    __meta_gen_opencl_metacl_module_current_frame->calc_velocity_delta_calc_velocity_delta_kernel = clCreateKernel(__meta_gen_opencl_metacl_module_current_frame->calc_velocity_delta_prog, "calc_velocity_delta", &createError);
    if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
  }
  if ((vendor & meta_cl_device_is_accel) && ((vendor & meta_cl_device_vendor_mask) == meta_cl_device_vendor_intelfpga)) {
    __meta_gen_opencl_metacl_module_current_frame->inner_source_progLen = metaOpenCLLoadProgramSource("inner_source.aocx", &__meta_gen_opencl_metacl_module_current_frame->inner_source_progSrc);
    if (__meta_gen_opencl_metacl_module_current_frame->inner_source_progLen != -1)
      __meta_gen_opencl_metacl_module_current_frame->inner_source_prog = clCreateProgramWithBinary(meta_context, 1, &meta_device, &__meta_gen_opencl_metacl_module_current_frame->inner_source_progLen, (const unsigned char **)&__meta_gen_opencl_metacl_module_current_frame->inner_source_progSrc, NULL, &buildError);
  } else {
    __meta_gen_opencl_metacl_module_current_frame->inner_source_progLen = metaOpenCLLoadProgramSource("inner_source.cl", &__meta_gen_opencl_metacl_module_current_frame->inner_source_progSrc);
    if (__meta_gen_opencl_metacl_module_current_frame->inner_source_progLen != -1)
      __meta_gen_opencl_metacl_module_current_frame->inner_source_prog = clCreateProgramWithSource(meta_context, 1, &__meta_gen_opencl_metacl_module_current_frame->inner_source_progSrc, &__meta_gen_opencl_metacl_module_current_frame->inner_source_progLen, &buildError);
  }
  if (__meta_gen_opencl_metacl_module_current_frame->inner_source_progLen != -1) {
    if (buildError != CL_SUCCESS) fprintf(stderr, "OpenCL program creation error %d at %s:%d\n", buildError, __FILE__, __LINE__);
    buildError = clBuildProgram(__meta_gen_opencl_metacl_module_current_frame->inner_source_prog, 1, &meta_device, __meta_gen_opencl_inner_source_custom_args ? __meta_gen_opencl_inner_source_custom_args : "", NULL, NULL);
    if (buildError != CL_SUCCESS) {
      size_t logsize = 0;
      clGetProgramBuildInfo(__meta_gen_opencl_metacl_module_current_frame->inner_source_prog, meta_device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logsize);
      char * buildLog = (char *) malloc(sizeof(char) * (logsize + 1));
      clGetProgramBuildInfo(__meta_gen_opencl_metacl_module_current_frame->inner_source_prog, meta_device, CL_PROGRAM_BUILD_LOG, logsize, buildLog, NULL);
      if (buildError != CL_SUCCESS) fprintf(stderr, "OpenCL program build error %d at %s:%d\n", buildError, __FILE__, __LINE__);
      fprintf(stderr, "Build Log:\n%s\n", buildLog);
      free(buildLog);
    } else {
      __meta_gen_opencl_metacl_module_current_frame->inner_source_init = 1;
    }
    __meta_gen_opencl_metacl_module_current_frame->inner_source_calc_inner_source_kernel = clCreateKernel(__meta_gen_opencl_metacl_module_current_frame->inner_source_prog, "calc_inner_source", &createError);
    if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
  }
  if ((vendor & meta_cl_device_is_accel) && ((vendor & meta_cl_device_vendor_mask) == meta_cl_device_vendor_intelfpga)) {
    __meta_gen_opencl_metacl_module_current_frame->outer_source_progLen = metaOpenCLLoadProgramSource("outer_source.aocx", &__meta_gen_opencl_metacl_module_current_frame->outer_source_progSrc);
    if (__meta_gen_opencl_metacl_module_current_frame->outer_source_progLen != -1)
      __meta_gen_opencl_metacl_module_current_frame->outer_source_prog = clCreateProgramWithBinary(meta_context, 1, &meta_device, &__meta_gen_opencl_metacl_module_current_frame->outer_source_progLen, (const unsigned char **)&__meta_gen_opencl_metacl_module_current_frame->outer_source_progSrc, NULL, &buildError);
  } else {
    __meta_gen_opencl_metacl_module_current_frame->outer_source_progLen = metaOpenCLLoadProgramSource("outer_source.cl", &__meta_gen_opencl_metacl_module_current_frame->outer_source_progSrc);
    if (__meta_gen_opencl_metacl_module_current_frame->outer_source_progLen != -1)
      __meta_gen_opencl_metacl_module_current_frame->outer_source_prog = clCreateProgramWithSource(meta_context, 1, &__meta_gen_opencl_metacl_module_current_frame->outer_source_progSrc, &__meta_gen_opencl_metacl_module_current_frame->outer_source_progLen, &buildError);
  }
  if (__meta_gen_opencl_metacl_module_current_frame->outer_source_progLen != -1) {
    if (buildError != CL_SUCCESS) fprintf(stderr, "OpenCL program creation error %d at %s:%d\n", buildError, __FILE__, __LINE__);
    buildError = clBuildProgram(__meta_gen_opencl_metacl_module_current_frame->outer_source_prog, 1, &meta_device, __meta_gen_opencl_outer_source_custom_args ? __meta_gen_opencl_outer_source_custom_args : "", NULL, NULL);
    if (buildError != CL_SUCCESS) {
      size_t logsize = 0;
      clGetProgramBuildInfo(__meta_gen_opencl_metacl_module_current_frame->outer_source_prog, meta_device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logsize);
      char * buildLog = (char *) malloc(sizeof(char) * (logsize + 1));
      clGetProgramBuildInfo(__meta_gen_opencl_metacl_module_current_frame->outer_source_prog, meta_device, CL_PROGRAM_BUILD_LOG, logsize, buildLog, NULL);
      if (buildError != CL_SUCCESS) fprintf(stderr, "OpenCL program build error %d at %s:%d\n", buildError, __FILE__, __LINE__);
      fprintf(stderr, "Build Log:\n%s\n", buildLog);
      free(buildLog);
    } else {
      __meta_gen_opencl_metacl_module_current_frame->outer_source_init = 1;
    }
    __meta_gen_opencl_metacl_module_current_frame->outer_source_calc_outer_source_kernel = clCreateKernel(__meta_gen_opencl_metacl_module_current_frame->outer_source_prog, "calc_outer_source", &createError);
    if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
  }
  if ((vendor & meta_cl_device_is_accel) && ((vendor & meta_cl_device_vendor_mask) == meta_cl_device_vendor_intelfpga)) {
    __meta_gen_opencl_metacl_module_current_frame->reduce_flux_progLen = metaOpenCLLoadProgramSource("reduce_flux.aocx", &__meta_gen_opencl_metacl_module_current_frame->reduce_flux_progSrc);
    if (__meta_gen_opencl_metacl_module_current_frame->reduce_flux_progLen != -1)
      __meta_gen_opencl_metacl_module_current_frame->reduce_flux_prog = clCreateProgramWithBinary(meta_context, 1, &meta_device, &__meta_gen_opencl_metacl_module_current_frame->reduce_flux_progLen, (const unsigned char **)&__meta_gen_opencl_metacl_module_current_frame->reduce_flux_progSrc, NULL, &buildError);
  } else {
    __meta_gen_opencl_metacl_module_current_frame->reduce_flux_progLen = metaOpenCLLoadProgramSource("reduce_flux.cl", &__meta_gen_opencl_metacl_module_current_frame->reduce_flux_progSrc);
    if (__meta_gen_opencl_metacl_module_current_frame->reduce_flux_progLen != -1)
      __meta_gen_opencl_metacl_module_current_frame->reduce_flux_prog = clCreateProgramWithSource(meta_context, 1, &__meta_gen_opencl_metacl_module_current_frame->reduce_flux_progSrc, &__meta_gen_opencl_metacl_module_current_frame->reduce_flux_progLen, &buildError);
  }
  if (__meta_gen_opencl_metacl_module_current_frame->reduce_flux_progLen != -1) {
    if (buildError != CL_SUCCESS) fprintf(stderr, "OpenCL program creation error %d at %s:%d\n", buildError, __FILE__, __LINE__);
    buildError = clBuildProgram(__meta_gen_opencl_metacl_module_current_frame->reduce_flux_prog, 1, &meta_device, __meta_gen_opencl_reduce_flux_custom_args ? __meta_gen_opencl_reduce_flux_custom_args : "", NULL, NULL);
    if (buildError != CL_SUCCESS) {
      size_t logsize = 0;
      clGetProgramBuildInfo(__meta_gen_opencl_metacl_module_current_frame->reduce_flux_prog, meta_device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logsize);
      char * buildLog = (char *) malloc(sizeof(char) * (logsize + 1));
      clGetProgramBuildInfo(__meta_gen_opencl_metacl_module_current_frame->reduce_flux_prog, meta_device, CL_PROGRAM_BUILD_LOG, logsize, buildLog, NULL);
      if (buildError != CL_SUCCESS) fprintf(stderr, "OpenCL program build error %d at %s:%d\n", buildError, __FILE__, __LINE__);
      fprintf(stderr, "Build Log:\n%s\n", buildLog);
      free(buildLog);
    } else {
      __meta_gen_opencl_metacl_module_current_frame->reduce_flux_init = 1;
    }
    __meta_gen_opencl_metacl_module_current_frame->reduce_flux_reduce_flux_kernel = clCreateKernel(__meta_gen_opencl_metacl_module_current_frame->reduce_flux_prog, "reduce_flux", &createError);
    if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
  }
  if ((vendor & meta_cl_device_is_accel) && ((vendor & meta_cl_device_vendor_mask) == meta_cl_device_vendor_intelfpga)) {
    __meta_gen_opencl_metacl_module_current_frame->reduce_flux_moments_progLen = metaOpenCLLoadProgramSource("reduce_flux_moments.aocx", &__meta_gen_opencl_metacl_module_current_frame->reduce_flux_moments_progSrc);
    if (__meta_gen_opencl_metacl_module_current_frame->reduce_flux_moments_progLen != -1)
      __meta_gen_opencl_metacl_module_current_frame->reduce_flux_moments_prog = clCreateProgramWithBinary(meta_context, 1, &meta_device, &__meta_gen_opencl_metacl_module_current_frame->reduce_flux_moments_progLen, (const unsigned char **)&__meta_gen_opencl_metacl_module_current_frame->reduce_flux_moments_progSrc, NULL, &buildError);
  } else {
    __meta_gen_opencl_metacl_module_current_frame->reduce_flux_moments_progLen = metaOpenCLLoadProgramSource("reduce_flux_moments.cl", &__meta_gen_opencl_metacl_module_current_frame->reduce_flux_moments_progSrc);
    if (__meta_gen_opencl_metacl_module_current_frame->reduce_flux_moments_progLen != -1)
      __meta_gen_opencl_metacl_module_current_frame->reduce_flux_moments_prog = clCreateProgramWithSource(meta_context, 1, &__meta_gen_opencl_metacl_module_current_frame->reduce_flux_moments_progSrc, &__meta_gen_opencl_metacl_module_current_frame->reduce_flux_moments_progLen, &buildError);
  }
  if (__meta_gen_opencl_metacl_module_current_frame->reduce_flux_moments_progLen != -1) {
    if (buildError != CL_SUCCESS) fprintf(stderr, "OpenCL program creation error %d at %s:%d\n", buildError, __FILE__, __LINE__);
    buildError = clBuildProgram(__meta_gen_opencl_metacl_module_current_frame->reduce_flux_moments_prog, 1, &meta_device, __meta_gen_opencl_reduce_flux_moments_custom_args ? __meta_gen_opencl_reduce_flux_moments_custom_args : "", NULL, NULL);
    if (buildError != CL_SUCCESS) {
      size_t logsize = 0;
      clGetProgramBuildInfo(__meta_gen_opencl_metacl_module_current_frame->reduce_flux_moments_prog, meta_device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logsize);
      char * buildLog = (char *) malloc(sizeof(char) * (logsize + 1));
      clGetProgramBuildInfo(__meta_gen_opencl_metacl_module_current_frame->reduce_flux_moments_prog, meta_device, CL_PROGRAM_BUILD_LOG, logsize, buildLog, NULL);
      if (buildError != CL_SUCCESS) fprintf(stderr, "OpenCL program build error %d at %s:%d\n", buildError, __FILE__, __LINE__);
      fprintf(stderr, "Build Log:\n%s\n", buildLog);
      free(buildLog);
    } else {
      __meta_gen_opencl_metacl_module_current_frame->reduce_flux_moments_init = 1;
    }
    __meta_gen_opencl_metacl_module_current_frame->reduce_flux_moments_reduce_flux_moments_kernel = clCreateKernel(__meta_gen_opencl_metacl_module_current_frame->reduce_flux_moments_prog, "reduce_flux_moments", &createError);
    if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
  }
  if ((vendor & meta_cl_device_is_accel) && ((vendor & meta_cl_device_vendor_mask) == meta_cl_device_vendor_intelfpga)) {
    __meta_gen_opencl_metacl_module_current_frame->sweep_plane_progLen = metaOpenCLLoadProgramSource("sweep_plane.aocx", &__meta_gen_opencl_metacl_module_current_frame->sweep_plane_progSrc);
    if (__meta_gen_opencl_metacl_module_current_frame->sweep_plane_progLen != -1)
      __meta_gen_opencl_metacl_module_current_frame->sweep_plane_prog = clCreateProgramWithBinary(meta_context, 1, &meta_device, &__meta_gen_opencl_metacl_module_current_frame->sweep_plane_progLen, (const unsigned char **)&__meta_gen_opencl_metacl_module_current_frame->sweep_plane_progSrc, NULL, &buildError);
  } else {
    __meta_gen_opencl_metacl_module_current_frame->sweep_plane_progLen = metaOpenCLLoadProgramSource("sweep_plane.cl", &__meta_gen_opencl_metacl_module_current_frame->sweep_plane_progSrc);
    if (__meta_gen_opencl_metacl_module_current_frame->sweep_plane_progLen != -1)
      __meta_gen_opencl_metacl_module_current_frame->sweep_plane_prog = clCreateProgramWithSource(meta_context, 1, &__meta_gen_opencl_metacl_module_current_frame->sweep_plane_progSrc, &__meta_gen_opencl_metacl_module_current_frame->sweep_plane_progLen, &buildError);
  }
  if (__meta_gen_opencl_metacl_module_current_frame->sweep_plane_progLen != -1) {
    if (buildError != CL_SUCCESS) fprintf(stderr, "OpenCL program creation error %d at %s:%d\n", buildError, __FILE__, __LINE__);
    buildError = clBuildProgram(__meta_gen_opencl_metacl_module_current_frame->sweep_plane_prog, 1, &meta_device, __meta_gen_opencl_sweep_plane_custom_args ? __meta_gen_opencl_sweep_plane_custom_args : "", NULL, NULL);
    if (buildError != CL_SUCCESS) {
      size_t logsize = 0;
      clGetProgramBuildInfo(__meta_gen_opencl_metacl_module_current_frame->sweep_plane_prog, meta_device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logsize);
      char * buildLog = (char *) malloc(sizeof(char) * (logsize + 1));
      clGetProgramBuildInfo(__meta_gen_opencl_metacl_module_current_frame->sweep_plane_prog, meta_device, CL_PROGRAM_BUILD_LOG, logsize, buildLog, NULL);
      if (buildError != CL_SUCCESS) fprintf(stderr, "OpenCL program build error %d at %s:%d\n", buildError, __FILE__, __LINE__);
      fprintf(stderr, "Build Log:\n%s\n", buildLog);
      free(buildLog);
    } else {
      __meta_gen_opencl_metacl_module_current_frame->sweep_plane_init = 1;
    }
    __meta_gen_opencl_metacl_module_current_frame->sweep_plane_sweep_plane_kernel = clCreateKernel(__meta_gen_opencl_metacl_module_current_frame->sweep_plane_prog, "sweep_plane", &createError);
    if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
  }
  if ((vendor & meta_cl_device_is_accel) && ((vendor & meta_cl_device_vendor_mask) == meta_cl_device_vendor_intelfpga)) {
    __meta_gen_opencl_metacl_module_current_frame->zero_buffer_progLen = metaOpenCLLoadProgramSource("zero_buffer.aocx", &__meta_gen_opencl_metacl_module_current_frame->zero_buffer_progSrc);
    if (__meta_gen_opencl_metacl_module_current_frame->zero_buffer_progLen != -1)
      __meta_gen_opencl_metacl_module_current_frame->zero_buffer_prog = clCreateProgramWithBinary(meta_context, 1, &meta_device, &__meta_gen_opencl_metacl_module_current_frame->zero_buffer_progLen, (const unsigned char **)&__meta_gen_opencl_metacl_module_current_frame->zero_buffer_progSrc, NULL, &buildError);
  } else {
    __meta_gen_opencl_metacl_module_current_frame->zero_buffer_progLen = metaOpenCLLoadProgramSource("zero_buffer.cl", &__meta_gen_opencl_metacl_module_current_frame->zero_buffer_progSrc);
    if (__meta_gen_opencl_metacl_module_current_frame->zero_buffer_progLen != -1)
      __meta_gen_opencl_metacl_module_current_frame->zero_buffer_prog = clCreateProgramWithSource(meta_context, 1, &__meta_gen_opencl_metacl_module_current_frame->zero_buffer_progSrc, &__meta_gen_opencl_metacl_module_current_frame->zero_buffer_progLen, &buildError);
  }
  if (__meta_gen_opencl_metacl_module_current_frame->zero_buffer_progLen != -1) {
    if (buildError != CL_SUCCESS) fprintf(stderr, "OpenCL program creation error %d at %s:%d\n", buildError, __FILE__, __LINE__);
    buildError = clBuildProgram(__meta_gen_opencl_metacl_module_current_frame->zero_buffer_prog, 1, &meta_device, __meta_gen_opencl_zero_buffer_custom_args ? __meta_gen_opencl_zero_buffer_custom_args : "", NULL, NULL);
    if (buildError != CL_SUCCESS) {
      size_t logsize = 0;
      clGetProgramBuildInfo(__meta_gen_opencl_metacl_module_current_frame->zero_buffer_prog, meta_device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logsize);
      char * buildLog = (char *) malloc(sizeof(char) * (logsize + 1));
      clGetProgramBuildInfo(__meta_gen_opencl_metacl_module_current_frame->zero_buffer_prog, meta_device, CL_PROGRAM_BUILD_LOG, logsize, buildLog, NULL);
      if (buildError != CL_SUCCESS) fprintf(stderr, "OpenCL program build error %d at %s:%d\n", buildError, __FILE__, __LINE__);
      fprintf(stderr, "Build Log:\n%s\n", buildLog);
      free(buildLog);
    } else {
      __meta_gen_opencl_metacl_module_current_frame->zero_buffer_init = 1;
    }
    __meta_gen_opencl_metacl_module_current_frame->zero_buffer_zero_buffer_kernel = clCreateKernel(__meta_gen_opencl_metacl_module_current_frame->zero_buffer_prog, "zero_buffer", &createError);
    if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
  }
  meta_gen_opencl_metacl_module_registration->initialized = 1;
}

void meta_gen_opencl_metacl_module_deinit() {
  cl_int releaseError;
  if (__meta_gen_opencl_metacl_module_current_frame != NULL) {
    if (__meta_gen_opencl_metacl_module_current_frame->calc_dd_coeff_progLen != -1) {
      releaseError = clReleaseKernel(__meta_gen_opencl_metacl_module_current_frame->calc_dd_coeff_calc_dd_coeff_kernel);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      releaseError = clReleaseProgram(__meta_gen_opencl_metacl_module_current_frame->calc_dd_coeff_prog);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL program release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      free(__meta_gen_opencl_metacl_module_current_frame->calc_dd_coeff_progSrc);
      __meta_gen_opencl_metacl_module_current_frame->calc_dd_coeff_progLen = 0;
      __meta_gen_opencl_metacl_module_current_frame->calc_dd_coeff_init = 0;
    }
    if (__meta_gen_opencl_metacl_module_current_frame->calc_denominator_progLen != -1) {
      releaseError = clReleaseKernel(__meta_gen_opencl_metacl_module_current_frame->calc_denominator_calc_denominator_kernel);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      releaseError = clReleaseProgram(__meta_gen_opencl_metacl_module_current_frame->calc_denominator_prog);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL program release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      free(__meta_gen_opencl_metacl_module_current_frame->calc_denominator_progSrc);
      __meta_gen_opencl_metacl_module_current_frame->calc_denominator_progLen = 0;
      __meta_gen_opencl_metacl_module_current_frame->calc_denominator_init = 0;
    }
    if (__meta_gen_opencl_metacl_module_current_frame->calc_velocity_delta_progLen != -1) {
      releaseError = clReleaseKernel(__meta_gen_opencl_metacl_module_current_frame->calc_velocity_delta_calc_velocity_delta_kernel);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      releaseError = clReleaseProgram(__meta_gen_opencl_metacl_module_current_frame->calc_velocity_delta_prog);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL program release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      free(__meta_gen_opencl_metacl_module_current_frame->calc_velocity_delta_progSrc);
      __meta_gen_opencl_metacl_module_current_frame->calc_velocity_delta_progLen = 0;
      __meta_gen_opencl_metacl_module_current_frame->calc_velocity_delta_init = 0;
    }
    if (__meta_gen_opencl_metacl_module_current_frame->inner_source_progLen != -1) {
      releaseError = clReleaseKernel(__meta_gen_opencl_metacl_module_current_frame->inner_source_calc_inner_source_kernel);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      releaseError = clReleaseProgram(__meta_gen_opencl_metacl_module_current_frame->inner_source_prog);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL program release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      free(__meta_gen_opencl_metacl_module_current_frame->inner_source_progSrc);
      __meta_gen_opencl_metacl_module_current_frame->inner_source_progLen = 0;
      __meta_gen_opencl_metacl_module_current_frame->inner_source_init = 0;
    }
    if (__meta_gen_opencl_metacl_module_current_frame->outer_source_progLen != -1) {
      releaseError = clReleaseKernel(__meta_gen_opencl_metacl_module_current_frame->outer_source_calc_outer_source_kernel);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      releaseError = clReleaseProgram(__meta_gen_opencl_metacl_module_current_frame->outer_source_prog);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL program release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      free(__meta_gen_opencl_metacl_module_current_frame->outer_source_progSrc);
      __meta_gen_opencl_metacl_module_current_frame->outer_source_progLen = 0;
      __meta_gen_opencl_metacl_module_current_frame->outer_source_init = 0;
    }
    if (__meta_gen_opencl_metacl_module_current_frame->reduce_flux_progLen != -1) {
      releaseError = clReleaseKernel(__meta_gen_opencl_metacl_module_current_frame->reduce_flux_reduce_flux_kernel);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      releaseError = clReleaseProgram(__meta_gen_opencl_metacl_module_current_frame->reduce_flux_prog);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL program release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      free(__meta_gen_opencl_metacl_module_current_frame->reduce_flux_progSrc);
      __meta_gen_opencl_metacl_module_current_frame->reduce_flux_progLen = 0;
      __meta_gen_opencl_metacl_module_current_frame->reduce_flux_init = 0;
    }
    if (__meta_gen_opencl_metacl_module_current_frame->reduce_flux_moments_progLen != -1) {
      releaseError = clReleaseKernel(__meta_gen_opencl_metacl_module_current_frame->reduce_flux_moments_reduce_flux_moments_kernel);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      releaseError = clReleaseProgram(__meta_gen_opencl_metacl_module_current_frame->reduce_flux_moments_prog);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL program release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      free(__meta_gen_opencl_metacl_module_current_frame->reduce_flux_moments_progSrc);
      __meta_gen_opencl_metacl_module_current_frame->reduce_flux_moments_progLen = 0;
      __meta_gen_opencl_metacl_module_current_frame->reduce_flux_moments_init = 0;
    }
    if (__meta_gen_opencl_metacl_module_current_frame->sweep_plane_progLen != -1) {
      releaseError = clReleaseKernel(__meta_gen_opencl_metacl_module_current_frame->sweep_plane_sweep_plane_kernel);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      releaseError = clReleaseProgram(__meta_gen_opencl_metacl_module_current_frame->sweep_plane_prog);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL program release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      free(__meta_gen_opencl_metacl_module_current_frame->sweep_plane_progSrc);
      __meta_gen_opencl_metacl_module_current_frame->sweep_plane_progLen = 0;
      __meta_gen_opencl_metacl_module_current_frame->sweep_plane_init = 0;
    }
    if (__meta_gen_opencl_metacl_module_current_frame->zero_buffer_progLen != -1) {
      releaseError = clReleaseKernel(__meta_gen_opencl_metacl_module_current_frame->zero_buffer_zero_buffer_kernel);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      releaseError = clReleaseProgram(__meta_gen_opencl_metacl_module_current_frame->zero_buffer_prog);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL program release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      free(__meta_gen_opencl_metacl_module_current_frame->zero_buffer_progSrc);
      __meta_gen_opencl_metacl_module_current_frame->zero_buffer_progLen = 0;
      __meta_gen_opencl_metacl_module_current_frame->zero_buffer_init = 0;
    }
    struct __meta_gen_opencl_metacl_module_frame * next_frame = __meta_gen_opencl_metacl_module_current_frame->next_frame;
    free(__meta_gen_opencl_metacl_module_current_frame);
    __meta_gen_opencl_metacl_module_current_frame = next_frame;
    if (__meta_gen_opencl_metacl_module_current_frame == NULL && meta_gen_opencl_metacl_module_registration != NULL) {
      meta_gen_opencl_metacl_module_registration->initialized = 0;
    }
  }
}

/** Automatically-generated by MetaGen-CL
\param queue the cl_command_queue on which to enqueue the kernel
\param grid_size a size_t[3] providing the number of workgroups in the X and Y dimensions, and the number of iterations in the Z dimension
\param block_size a size_t[3] providing the workgroup size in the X, Y, Z dimensions
\param dx scalar parameter of type "cl_double"
\param dy scalar parameter of type "cl_double"
\param dz scalar parameter of type "cl_double"
\param eta a cl_mem buffer, must internally store cl_double types
\param xi a cl_mem buffer, must internally store cl_double types
\param dd_i a cl_mem buffer, must internally store cl_double types
\param dd_j a cl_mem buffer, must internally store cl_double types
\param dd_k a cl_mem buffer, must internally store cl_double types
\param async whether the kernel should run asynchronously
\param event returns the cl_event corresponding to the kernel launch if run asynchronously
*/
cl_int meta_gen_opencl_calc_dd_coeff_calc_dd_coeff(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], size_t (*meta_offset)[3], cl_double dx, cl_double dy, cl_double dz, cl_mem * eta, cl_mem * xi, cl_mem * dd_i, cl_mem * dd_j, cl_mem * dd_k, int async, cl_event * event) {
  if (meta_gen_opencl_metacl_module_registration == NULL) meta_register_module(&meta_gen_opencl_metacl_module_registry);
  struct __meta_gen_opencl_metacl_module_frame * frame = __meta_gen_opencl_metacl_module_current_frame;
  if (queue != NULL) frame = __meta_gen_opencl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->calc_dd_coeff_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  a_bool nullBlock = (block_size != NULL && (*block_size)[0] == 0 && (*block_size)[1] == 0 && (*block_size)[2] == 0);
  size_t grid[3];
  size_t block[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  int iters;

  //Default runs a single workgroup
  if (grid_size == NULL || block_size == NULL) {
    grid[0] = block[0];
    grid[1] = block[1];
    grid[2] = block[2];
    iters = 1;
  } else {
    grid[0] = (*grid_size)[0] * (nullBlock ? 1 : (*block_size)[0]);
    grid[1] = (*grid_size)[1] * (nullBlock ? 1 : (*block_size)[1]);
    grid[2] = (*grid_size)[2] * (nullBlock ? 1 : (*block_size)[2]);
    block[0] = (*block_size)[0];
    block[1] = (*block_size)[1];
    block[2] = (*block_size)[2];
    iters = (*grid_size)[2];
  }
  retCode = clSetKernelArg(frame->calc_dd_coeff_calc_dd_coeff_kernel, 0, sizeof(cl_double), &dx);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dx\", host wrapper: \"meta_gen_opencl_calc_dd_coeff_calc_dd_coeff\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_dd_coeff_calc_dd_coeff_kernel, 1, sizeof(cl_double), &dy);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dy\", host wrapper: \"meta_gen_opencl_calc_dd_coeff_calc_dd_coeff\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_dd_coeff_calc_dd_coeff_kernel, 2, sizeof(cl_double), &dz);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dz\", host wrapper: \"meta_gen_opencl_calc_dd_coeff_calc_dd_coeff\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_dd_coeff_calc_dd_coeff_kernel, 3, sizeof(cl_mem), eta);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"eta\", host wrapper: \"meta_gen_opencl_calc_dd_coeff_calc_dd_coeff\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_dd_coeff_calc_dd_coeff_kernel, 4, sizeof(cl_mem), xi);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"xi\", host wrapper: \"meta_gen_opencl_calc_dd_coeff_calc_dd_coeff\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_dd_coeff_calc_dd_coeff_kernel, 5, sizeof(cl_mem), dd_i);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_i\", host wrapper: \"meta_gen_opencl_calc_dd_coeff_calc_dd_coeff\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_dd_coeff_calc_dd_coeff_kernel, 6, sizeof(cl_mem), dd_j);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_j\", host wrapper: \"meta_gen_opencl_calc_dd_coeff_calc_dd_coeff\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_dd_coeff_calc_dd_coeff_kernel, 7, sizeof(cl_mem), dd_k);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_k\", host wrapper: \"meta_gen_opencl_calc_dd_coeff_calc_dd_coeff\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->calc_dd_coeff_calc_dd_coeff_kernel, 3, meta_offset, grid, (nullBlock ? NULL : block), 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"meta_gen_opencl_calc_dd_coeff_calc_dd_coeff\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
    if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"meta_gen_opencl_calc_dd_coeff_calc_dd_coeff\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

/** Automatically-generated by MetaGen-CL
\param queue the cl_command_queue on which to enqueue the kernel
\param grid_size a size_t[3] providing the number of workgroups in the X and Y dimensions, and the number of iterations in the Z dimension
\param block_size a size_t[3] providing the workgroup size in the X, Y, Z dimensions
\param nx scalar parameter of type "cl_uint"
\param ny scalar parameter of type "cl_uint"
\param nz scalar parameter of type "cl_uint"
\param nang scalar parameter of type "cl_uint"
\param ng scalar parameter of type "cl_uint"
\param mat_cross_section a cl_mem buffer, must internally store cl_double types
\param velocity_delta a cl_mem buffer, must internally store cl_double types
\param mu a cl_mem buffer, must internally store cl_double types
\param dd_i a cl_mem buffer, must internally store cl_double types
\param dd_j a cl_mem buffer, must internally store cl_double types
\param dd_k a cl_mem buffer, must internally store cl_double types
\param denominator a cl_mem buffer, must internally store cl_double types
\param async whether the kernel should run asynchronously
\param event returns the cl_event corresponding to the kernel launch if run asynchronously
*/
cl_int meta_gen_opencl_calc_denominator_calc_denominator(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], size_t (*meta_offset)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint nang, cl_uint ng, cl_mem * mat_cross_section, cl_mem * velocity_delta, cl_mem * mu, cl_mem * dd_i, cl_mem * dd_j, cl_mem * dd_k, cl_mem * denominator, int async, cl_event * event) {
  if (meta_gen_opencl_metacl_module_registration == NULL) meta_register_module(&meta_gen_opencl_metacl_module_registry);
  struct __meta_gen_opencl_metacl_module_frame * frame = __meta_gen_opencl_metacl_module_current_frame;
  if (queue != NULL) frame = __meta_gen_opencl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->calc_denominator_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  a_bool nullBlock = (block_size != NULL && (*block_size)[0] == 0 && (*block_size)[1] == 0 && (*block_size)[2] == 0);
  size_t grid[3];
  size_t block[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  int iters;

  //Default runs a single workgroup
  if (grid_size == NULL || block_size == NULL) {
    grid[0] = block[0];
    grid[1] = block[1];
    grid[2] = block[2];
    iters = 1;
  } else {
    grid[0] = (*grid_size)[0] * (nullBlock ? 1 : (*block_size)[0]);
    grid[1] = (*grid_size)[1] * (nullBlock ? 1 : (*block_size)[1]);
    grid[2] = (*grid_size)[2] * (nullBlock ? 1 : (*block_size)[2]);
    block[0] = (*block_size)[0];
    block[1] = (*block_size)[1];
    block[2] = (*block_size)[2];
    iters = (*grid_size)[2];
  }
  retCode = clSetKernelArg(frame->calc_denominator_calc_denominator_kernel, 0, sizeof(cl_uint), &nx);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nx\", host wrapper: \"meta_gen_opencl_calc_denominator_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_denominator_calc_denominator_kernel, 1, sizeof(cl_uint), &ny);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ny\", host wrapper: \"meta_gen_opencl_calc_denominator_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_denominator_calc_denominator_kernel, 2, sizeof(cl_uint), &nz);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nz\", host wrapper: \"meta_gen_opencl_calc_denominator_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_denominator_calc_denominator_kernel, 3, sizeof(cl_uint), &nang);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nang\", host wrapper: \"meta_gen_opencl_calc_denominator_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_denominator_calc_denominator_kernel, 4, sizeof(cl_uint), &ng);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ng\", host wrapper: \"meta_gen_opencl_calc_denominator_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_denominator_calc_denominator_kernel, 5, sizeof(cl_mem), mat_cross_section);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"mat_cross_section\", host wrapper: \"meta_gen_opencl_calc_denominator_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_denominator_calc_denominator_kernel, 6, sizeof(cl_mem), velocity_delta);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"velocity_delta\", host wrapper: \"meta_gen_opencl_calc_denominator_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_denominator_calc_denominator_kernel, 7, sizeof(cl_mem), mu);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"mu\", host wrapper: \"meta_gen_opencl_calc_denominator_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_denominator_calc_denominator_kernel, 8, sizeof(cl_mem), dd_i);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_i\", host wrapper: \"meta_gen_opencl_calc_denominator_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_denominator_calc_denominator_kernel, 9, sizeof(cl_mem), dd_j);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_j\", host wrapper: \"meta_gen_opencl_calc_denominator_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_denominator_calc_denominator_kernel, 10, sizeof(cl_mem), dd_k);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_k\", host wrapper: \"meta_gen_opencl_calc_denominator_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_denominator_calc_denominator_kernel, 11, sizeof(cl_mem), denominator);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"denominator\", host wrapper: \"meta_gen_opencl_calc_denominator_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->calc_denominator_calc_denominator_kernel, 3, meta_offset, grid, (nullBlock ? NULL : block), 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"meta_gen_opencl_calc_denominator_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
    if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"meta_gen_opencl_calc_denominator_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

/** Automatically-generated by MetaGen-CL
\param queue the cl_command_queue on which to enqueue the kernel
\param grid_size a size_t[3] providing the number of workgroups in the X and Y dimensions, and the number of iterations in the Z dimension
\param block_size a size_t[3] providing the workgroup size in the X, Y, Z dimensions
\param velocities a cl_mem buffer, must internally store cl_double types
\param dt scalar parameter of type "cl_double"
\param velocity_delta a cl_mem buffer, must internally store cl_double types
\param async whether the kernel should run asynchronously
\param event returns the cl_event corresponding to the kernel launch if run asynchronously
*/
cl_int meta_gen_opencl_calc_velocity_delta_calc_velocity_delta(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], size_t (*meta_offset)[3], cl_mem * velocities, cl_double dt, cl_mem * velocity_delta, int async, cl_event * event) {
  if (meta_gen_opencl_metacl_module_registration == NULL) meta_register_module(&meta_gen_opencl_metacl_module_registry);
  struct __meta_gen_opencl_metacl_module_frame * frame = __meta_gen_opencl_metacl_module_current_frame;
  if (queue != NULL) frame = __meta_gen_opencl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->calc_velocity_delta_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  a_bool nullBlock = (block_size != NULL && (*block_size)[0] == 0 && (*block_size)[1] == 0 && (*block_size)[2] == 0);
  size_t grid[3];
  size_t block[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  int iters;

  //Default runs a single workgroup
  if (grid_size == NULL || block_size == NULL) {
    grid[0] = block[0];
    grid[1] = block[1];
    grid[2] = block[2];
    iters = 1;
  } else {
    grid[0] = (*grid_size)[0] * (nullBlock ? 1 : (*block_size)[0]);
    grid[1] = (*grid_size)[1] * (nullBlock ? 1 : (*block_size)[1]);
    grid[2] = (*grid_size)[2] * (nullBlock ? 1 : (*block_size)[2]);
    block[0] = (*block_size)[0];
    block[1] = (*block_size)[1];
    block[2] = (*block_size)[2];
    iters = (*grid_size)[2];
  }
  retCode = clSetKernelArg(frame->calc_velocity_delta_calc_velocity_delta_kernel, 0, sizeof(cl_mem), velocities);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"velocities\", host wrapper: \"meta_gen_opencl_calc_velocity_delta_calc_velocity_delta\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_velocity_delta_calc_velocity_delta_kernel, 1, sizeof(cl_double), &dt);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dt\", host wrapper: \"meta_gen_opencl_calc_velocity_delta_calc_velocity_delta\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->calc_velocity_delta_calc_velocity_delta_kernel, 2, sizeof(cl_mem), velocity_delta);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"velocity_delta\", host wrapper: \"meta_gen_opencl_calc_velocity_delta_calc_velocity_delta\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->calc_velocity_delta_calc_velocity_delta_kernel, 3, meta_offset, grid, (nullBlock ? NULL : block), 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"meta_gen_opencl_calc_velocity_delta_calc_velocity_delta\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
    if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"meta_gen_opencl_calc_velocity_delta_calc_velocity_delta\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

/** Automatically-generated by MetaGen-CL
\param queue the cl_command_queue on which to enqueue the kernel
\param grid_size a size_t[3] providing the number of workgroups in the X and Y dimensions, and the number of iterations in the Z dimension
\param block_size a size_t[3] providing the workgroup size in the X, Y, Z dimensions
\param nx scalar parameter of type "cl_uint"
\param ny scalar parameter of type "cl_uint"
\param nz scalar parameter of type "cl_uint"
\param ng scalar parameter of type "cl_uint"
\param cmom scalar parameter of type "cl_uint"
\param nmom scalar parameter of type "cl_uint"
\param outer_source a cl_mem buffer, must internally store cl_double types
\param scattering_matrix a cl_mem buffer, must internally store cl_double types
\param scalar_flux a cl_mem buffer, must internally store cl_double types
\param scalar_flux_moments a cl_mem buffer, must internally store cl_double types
\param inner_source a cl_mem buffer, must internally store cl_double types
\param async whether the kernel should run asynchronously
\param event returns the cl_event corresponding to the kernel launch if run asynchronously
*/
cl_int meta_gen_opencl_inner_source_calc_inner_source(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], size_t (*meta_offset)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint ng, cl_uint cmom, cl_uint nmom, cl_mem * outer_source, cl_mem * scattering_matrix, cl_mem * scalar_flux, cl_mem * scalar_flux_moments, cl_mem * inner_source, int async, cl_event * event) {
  if (meta_gen_opencl_metacl_module_registration == NULL) meta_register_module(&meta_gen_opencl_metacl_module_registry);
  struct __meta_gen_opencl_metacl_module_frame * frame = __meta_gen_opencl_metacl_module_current_frame;
  if (queue != NULL) frame = __meta_gen_opencl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->inner_source_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  a_bool nullBlock = (block_size != NULL && (*block_size)[0] == 0 && (*block_size)[1] == 0 && (*block_size)[2] == 0);
  size_t grid[3];
  size_t block[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  int iters;

  //Default runs a single workgroup
  if (grid_size == NULL || block_size == NULL) {
    grid[0] = block[0];
    grid[1] = block[1];
    grid[2] = block[2];
    iters = 1;
  } else {
    grid[0] = (*grid_size)[0] * (nullBlock ? 1 : (*block_size)[0]);
    grid[1] = (*grid_size)[1] * (nullBlock ? 1 : (*block_size)[1]);
    grid[2] = (*grid_size)[2] * (nullBlock ? 1 : (*block_size)[2]);
    block[0] = (*block_size)[0];
    block[1] = (*block_size)[1];
    block[2] = (*block_size)[2];
    iters = (*grid_size)[2];
  }
  retCode = clSetKernelArg(frame->inner_source_calc_inner_source_kernel, 0, sizeof(cl_uint), &nx);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nx\", host wrapper: \"meta_gen_opencl_inner_source_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->inner_source_calc_inner_source_kernel, 1, sizeof(cl_uint), &ny);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ny\", host wrapper: \"meta_gen_opencl_inner_source_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->inner_source_calc_inner_source_kernel, 2, sizeof(cl_uint), &nz);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nz\", host wrapper: \"meta_gen_opencl_inner_source_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->inner_source_calc_inner_source_kernel, 3, sizeof(cl_uint), &ng);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ng\", host wrapper: \"meta_gen_opencl_inner_source_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->inner_source_calc_inner_source_kernel, 4, sizeof(cl_uint), &cmom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"cmom\", host wrapper: \"meta_gen_opencl_inner_source_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->inner_source_calc_inner_source_kernel, 5, sizeof(cl_uint), &nmom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nmom\", host wrapper: \"meta_gen_opencl_inner_source_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->inner_source_calc_inner_source_kernel, 6, sizeof(cl_mem), outer_source);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"outer_source\", host wrapper: \"meta_gen_opencl_inner_source_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->inner_source_calc_inner_source_kernel, 7, sizeof(cl_mem), scattering_matrix);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scattering_matrix\", host wrapper: \"meta_gen_opencl_inner_source_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->inner_source_calc_inner_source_kernel, 8, sizeof(cl_mem), scalar_flux);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scalar_flux\", host wrapper: \"meta_gen_opencl_inner_source_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->inner_source_calc_inner_source_kernel, 9, sizeof(cl_mem), scalar_flux_moments);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scalar_flux_moments\", host wrapper: \"meta_gen_opencl_inner_source_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->inner_source_calc_inner_source_kernel, 10, sizeof(cl_mem), inner_source);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"inner_source\", host wrapper: \"meta_gen_opencl_inner_source_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->inner_source_calc_inner_source_kernel, 3, meta_offset, grid, (nullBlock ? NULL : block), 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"meta_gen_opencl_inner_source_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
    if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"meta_gen_opencl_inner_source_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

/** Automatically-generated by MetaGen-CL
\param queue the cl_command_queue on which to enqueue the kernel
\param grid_size a size_t[3] providing the number of workgroups in the X and Y dimensions, and the number of iterations in the Z dimension
\param block_size a size_t[3] providing the workgroup size in the X, Y, Z dimensions
\param nx scalar parameter of type "cl_uint"
\param ny scalar parameter of type "cl_uint"
\param nz scalar parameter of type "cl_uint"
\param ng scalar parameter of type "cl_uint"
\param cmom scalar parameter of type "cl_uint"
\param nmom scalar parameter of type "cl_uint"
\param fixed_source a cl_mem buffer, must internally store cl_double types
\param scattering_matrix a cl_mem buffer, must internally store cl_double types
\param scalar_flux a cl_mem buffer, must internally store cl_double types
\param scalar_flux_moments a cl_mem buffer, must internally store cl_double types
\param outer_source a cl_mem buffer, must internally store cl_double types
\param async whether the kernel should run asynchronously
\param event returns the cl_event corresponding to the kernel launch if run asynchronously
*/
cl_int meta_gen_opencl_outer_source_calc_outer_source(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], size_t (*meta_offset)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint ng, cl_uint cmom, cl_uint nmom, cl_mem * fixed_source, cl_mem * scattering_matrix, cl_mem * scalar_flux, cl_mem * scalar_flux_moments, cl_mem * outer_source, int async, cl_event * event) {
  if (meta_gen_opencl_metacl_module_registration == NULL) meta_register_module(&meta_gen_opencl_metacl_module_registry);
  struct __meta_gen_opencl_metacl_module_frame * frame = __meta_gen_opencl_metacl_module_current_frame;
  if (queue != NULL) frame = __meta_gen_opencl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->outer_source_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  a_bool nullBlock = (block_size != NULL && (*block_size)[0] == 0 && (*block_size)[1] == 0 && (*block_size)[2] == 0);
  size_t grid[3];
  size_t block[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  int iters;

  //Default runs a single workgroup
  if (grid_size == NULL || block_size == NULL) {
    grid[0] = block[0];
    grid[1] = block[1];
    grid[2] = block[2];
    iters = 1;
  } else {
    grid[0] = (*grid_size)[0] * (nullBlock ? 1 : (*block_size)[0]);
    grid[1] = (*grid_size)[1] * (nullBlock ? 1 : (*block_size)[1]);
    grid[2] = (*grid_size)[2] * (nullBlock ? 1 : (*block_size)[2]);
    block[0] = (*block_size)[0];
    block[1] = (*block_size)[1];
    block[2] = (*block_size)[2];
    iters = (*grid_size)[2];
  }
  retCode = clSetKernelArg(frame->outer_source_calc_outer_source_kernel, 0, sizeof(cl_uint), &nx);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nx\", host wrapper: \"meta_gen_opencl_outer_source_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_source_calc_outer_source_kernel, 1, sizeof(cl_uint), &ny);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ny\", host wrapper: \"meta_gen_opencl_outer_source_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_source_calc_outer_source_kernel, 2, sizeof(cl_uint), &nz);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nz\", host wrapper: \"meta_gen_opencl_outer_source_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_source_calc_outer_source_kernel, 3, sizeof(cl_uint), &ng);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ng\", host wrapper: \"meta_gen_opencl_outer_source_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_source_calc_outer_source_kernel, 4, sizeof(cl_uint), &cmom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"cmom\", host wrapper: \"meta_gen_opencl_outer_source_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_source_calc_outer_source_kernel, 5, sizeof(cl_uint), &nmom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nmom\", host wrapper: \"meta_gen_opencl_outer_source_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_source_calc_outer_source_kernel, 6, sizeof(cl_mem), fixed_source);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"fixed_source\", host wrapper: \"meta_gen_opencl_outer_source_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_source_calc_outer_source_kernel, 7, sizeof(cl_mem), scattering_matrix);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scattering_matrix\", host wrapper: \"meta_gen_opencl_outer_source_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_source_calc_outer_source_kernel, 8, sizeof(cl_mem), scalar_flux);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scalar_flux\", host wrapper: \"meta_gen_opencl_outer_source_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_source_calc_outer_source_kernel, 9, sizeof(cl_mem), scalar_flux_moments);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scalar_flux_moments\", host wrapper: \"meta_gen_opencl_outer_source_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_source_calc_outer_source_kernel, 10, sizeof(cl_mem), outer_source);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"outer_source\", host wrapper: \"meta_gen_opencl_outer_source_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->outer_source_calc_outer_source_kernel, 3, meta_offset, grid, (nullBlock ? NULL : block), 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"meta_gen_opencl_outer_source_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
    if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"meta_gen_opencl_outer_source_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

/** Automatically-generated by MetaGen-CL
\param queue the cl_command_queue on which to enqueue the kernel
\param grid_size a size_t[3] providing the number of workgroups in the X and Y dimensions, and the number of iterations in the Z dimension
\param block_size a size_t[3] providing the workgroup size in the X, Y, Z dimensions
\param nx scalar parameter of type "cl_uint"
\param ny scalar parameter of type "cl_uint"
\param nz scalar parameter of type "cl_uint"
\param nang scalar parameter of type "cl_uint"
\param ng scalar parameter of type "cl_uint"
\param angular_flux_in_0 a cl_mem buffer, must internally store cl_double types
\param angular_flux_in_1 a cl_mem buffer, must internally store cl_double types
\param angular_flux_in_2 a cl_mem buffer, must internally store cl_double types
\param angular_flux_in_3 a cl_mem buffer, must internally store cl_double types
\param angular_flux_in_4 a cl_mem buffer, must internally store cl_double types
\param angular_flux_in_5 a cl_mem buffer, must internally store cl_double types
\param angular_flux_in_6 a cl_mem buffer, must internally store cl_double types
\param angular_flux_in_7 a cl_mem buffer, must internally store cl_double types
\param angular_flux_out_0 a cl_mem buffer, must internally store cl_double types
\param angular_flux_out_1 a cl_mem buffer, must internally store cl_double types
\param angular_flux_out_2 a cl_mem buffer, must internally store cl_double types
\param angular_flux_out_3 a cl_mem buffer, must internally store cl_double types
\param angular_flux_out_4 a cl_mem buffer, must internally store cl_double types
\param angular_flux_out_5 a cl_mem buffer, must internally store cl_double types
\param angular_flux_out_6 a cl_mem buffer, must internally store cl_double types
\param angular_flux_out_7 a cl_mem buffer, must internally store cl_double types
\param velocity_delta a cl_mem buffer, must internally store cl_double types
\param quad_weights a cl_mem buffer, must internally store cl_double types
\param scalar_flux a cl_mem buffer, must internally store cl_double types
\param local_scalar_num_local_elems allocate __local memory space for this many cl_double elements
\param async whether the kernel should run asynchronously
\param event returns the cl_event corresponding to the kernel launch if run asynchronously
*/
cl_int meta_gen_opencl_reduce_flux_reduce_flux(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], size_t (*meta_offset)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint nang, cl_uint ng, cl_mem * angular_flux_in_0, cl_mem * angular_flux_in_1, cl_mem * angular_flux_in_2, cl_mem * angular_flux_in_3, cl_mem * angular_flux_in_4, cl_mem * angular_flux_in_5, cl_mem * angular_flux_in_6, cl_mem * angular_flux_in_7, cl_mem * angular_flux_out_0, cl_mem * angular_flux_out_1, cl_mem * angular_flux_out_2, cl_mem * angular_flux_out_3, cl_mem * angular_flux_out_4, cl_mem * angular_flux_out_5, cl_mem * angular_flux_out_6, cl_mem * angular_flux_out_7, cl_mem * velocity_delta, cl_mem * quad_weights, cl_mem * scalar_flux, size_t local_scalar_num_local_elems, int async, cl_event * event) {
  if (meta_gen_opencl_metacl_module_registration == NULL) meta_register_module(&meta_gen_opencl_metacl_module_registry);
  struct __meta_gen_opencl_metacl_module_frame * frame = __meta_gen_opencl_metacl_module_current_frame;
  if (queue != NULL) frame = __meta_gen_opencl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->reduce_flux_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  a_bool nullBlock = (block_size != NULL && (*block_size)[0] == 0 && (*block_size)[1] == 0 && (*block_size)[2] == 0);
  size_t grid[3];
  size_t block[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  int iters;

  //Default runs a single workgroup
  if (grid_size == NULL || block_size == NULL) {
    grid[0] = block[0];
    grid[1] = block[1];
    grid[2] = block[2];
    iters = 1;
  } else {
    grid[0] = (*grid_size)[0] * (nullBlock ? 1 : (*block_size)[0]);
    grid[1] = (*grid_size)[1] * (nullBlock ? 1 : (*block_size)[1]);
    grid[2] = (*grid_size)[2] * (nullBlock ? 1 : (*block_size)[2]);
    block[0] = (*block_size)[0];
    block[1] = (*block_size)[1];
    block[2] = (*block_size)[2];
    iters = (*grid_size)[2];
  }
  retCode = clSetKernelArg(frame->reduce_flux_reduce_flux_kernel, 0, sizeof(cl_uint), &nx);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nx\", host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_reduce_flux_kernel, 1, sizeof(cl_uint), &ny);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ny\", host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_reduce_flux_kernel, 2, sizeof(cl_uint), &nz);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nz\", host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_reduce_flux_kernel, 3, sizeof(cl_uint), &nang);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nang\", host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_reduce_flux_kernel, 4, sizeof(cl_uint), &ng);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ng\", host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_reduce_flux_kernel, 5, sizeof(cl_mem), angular_flux_in_0);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_0\", host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_reduce_flux_kernel, 6, sizeof(cl_mem), angular_flux_in_1);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_1\", host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_reduce_flux_kernel, 7, sizeof(cl_mem), angular_flux_in_2);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_2\", host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_reduce_flux_kernel, 8, sizeof(cl_mem), angular_flux_in_3);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_3\", host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_reduce_flux_kernel, 9, sizeof(cl_mem), angular_flux_in_4);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_4\", host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_reduce_flux_kernel, 10, sizeof(cl_mem), angular_flux_in_5);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_5\", host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_reduce_flux_kernel, 11, sizeof(cl_mem), angular_flux_in_6);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_6\", host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_reduce_flux_kernel, 12, sizeof(cl_mem), angular_flux_in_7);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_7\", host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_reduce_flux_kernel, 13, sizeof(cl_mem), angular_flux_out_0);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_0\", host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_reduce_flux_kernel, 14, sizeof(cl_mem), angular_flux_out_1);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_1\", host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_reduce_flux_kernel, 15, sizeof(cl_mem), angular_flux_out_2);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_2\", host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_reduce_flux_kernel, 16, sizeof(cl_mem), angular_flux_out_3);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_3\", host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_reduce_flux_kernel, 17, sizeof(cl_mem), angular_flux_out_4);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_4\", host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_reduce_flux_kernel, 18, sizeof(cl_mem), angular_flux_out_5);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_5\", host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_reduce_flux_kernel, 19, sizeof(cl_mem), angular_flux_out_6);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_6\", host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_reduce_flux_kernel, 20, sizeof(cl_mem), angular_flux_out_7);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_7\", host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_reduce_flux_kernel, 21, sizeof(cl_mem), velocity_delta);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"velocity_delta\", host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_reduce_flux_kernel, 22, sizeof(cl_mem), quad_weights);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"quad_weights\", host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_reduce_flux_kernel, 23, sizeof(cl_mem), scalar_flux);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scalar_flux\", host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_reduce_flux_kernel, 24, sizeof(cl_double) * local_scalar_num_local_elems, NULL);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"local_scalar\", host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->reduce_flux_reduce_flux_kernel, 3, meta_offset, grid, (nullBlock ? NULL : block), 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
    if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"meta_gen_opencl_reduce_flux_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

/** Automatically-generated by MetaGen-CL
\param queue the cl_command_queue on which to enqueue the kernel
\param grid_size a size_t[3] providing the number of workgroups in the X and Y dimensions, and the number of iterations in the Z dimension
\param block_size a size_t[3] providing the workgroup size in the X, Y, Z dimensions
\param nx scalar parameter of type "cl_uint"
\param ny scalar parameter of type "cl_uint"
\param nz scalar parameter of type "cl_uint"
\param nang scalar parameter of type "cl_uint"
\param ng scalar parameter of type "cl_uint"
\param cmom scalar parameter of type "cl_uint"
\param angular_flux_in_0 a cl_mem buffer, must internally store cl_double types
\param angular_flux_in_1 a cl_mem buffer, must internally store cl_double types
\param angular_flux_in_2 a cl_mem buffer, must internally store cl_double types
\param angular_flux_in_3 a cl_mem buffer, must internally store cl_double types
\param angular_flux_in_4 a cl_mem buffer, must internally store cl_double types
\param angular_flux_in_5 a cl_mem buffer, must internally store cl_double types
\param angular_flux_in_6 a cl_mem buffer, must internally store cl_double types
\param angular_flux_in_7 a cl_mem buffer, must internally store cl_double types
\param angular_flux_out_0 a cl_mem buffer, must internally store cl_double types
\param angular_flux_out_1 a cl_mem buffer, must internally store cl_double types
\param angular_flux_out_2 a cl_mem buffer, must internally store cl_double types
\param angular_flux_out_3 a cl_mem buffer, must internally store cl_double types
\param angular_flux_out_4 a cl_mem buffer, must internally store cl_double types
\param angular_flux_out_5 a cl_mem buffer, must internally store cl_double types
\param angular_flux_out_6 a cl_mem buffer, must internally store cl_double types
\param angular_flux_out_7 a cl_mem buffer, must internally store cl_double types
\param velocity_delta a cl_mem buffer, must internally store cl_double types
\param quad_weights a cl_mem buffer, must internally store cl_double types
\param scat_coeff a cl_mem buffer, must internally store cl_double types
\param scalar_flux_moments a cl_mem buffer, must internally store cl_double types
\param local_scalar_num_local_elems allocate __local memory space for this many cl_double elements
\param async whether the kernel should run asynchronously
\param event returns the cl_event corresponding to the kernel launch if run asynchronously
*/
cl_int meta_gen_opencl_reduce_flux_moments_reduce_flux_moments(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], size_t (*meta_offset)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint nang, cl_uint ng, cl_uint cmom, cl_mem * angular_flux_in_0, cl_mem * angular_flux_in_1, cl_mem * angular_flux_in_2, cl_mem * angular_flux_in_3, cl_mem * angular_flux_in_4, cl_mem * angular_flux_in_5, cl_mem * angular_flux_in_6, cl_mem * angular_flux_in_7, cl_mem * angular_flux_out_0, cl_mem * angular_flux_out_1, cl_mem * angular_flux_out_2, cl_mem * angular_flux_out_3, cl_mem * angular_flux_out_4, cl_mem * angular_flux_out_5, cl_mem * angular_flux_out_6, cl_mem * angular_flux_out_7, cl_mem * velocity_delta, cl_mem * quad_weights, cl_mem * scat_coeff, cl_mem * scalar_flux_moments, size_t local_scalar_num_local_elems, int async, cl_event * event) {
  if (meta_gen_opencl_metacl_module_registration == NULL) meta_register_module(&meta_gen_opencl_metacl_module_registry);
  struct __meta_gen_opencl_metacl_module_frame * frame = __meta_gen_opencl_metacl_module_current_frame;
  if (queue != NULL) frame = __meta_gen_opencl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->reduce_flux_moments_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  a_bool nullBlock = (block_size != NULL && (*block_size)[0] == 0 && (*block_size)[1] == 0 && (*block_size)[2] == 0);
  size_t grid[3];
  size_t block[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  int iters;

  //Default runs a single workgroup
  if (grid_size == NULL || block_size == NULL) {
    grid[0] = block[0];
    grid[1] = block[1];
    grid[2] = block[2];
    iters = 1;
  } else {
    grid[0] = (*grid_size)[0] * (nullBlock ? 1 : (*block_size)[0]);
    grid[1] = (*grid_size)[1] * (nullBlock ? 1 : (*block_size)[1]);
    grid[2] = (*grid_size)[2] * (nullBlock ? 1 : (*block_size)[2]);
    block[0] = (*block_size)[0];
    block[1] = (*block_size)[1];
    block[2] = (*block_size)[2];
    iters = (*grid_size)[2];
  }
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 0, sizeof(cl_uint), &nx);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nx\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 1, sizeof(cl_uint), &ny);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ny\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 2, sizeof(cl_uint), &nz);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nz\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 3, sizeof(cl_uint), &nang);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nang\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 4, sizeof(cl_uint), &ng);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ng\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 5, sizeof(cl_uint), &cmom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"cmom\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 6, sizeof(cl_mem), angular_flux_in_0);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_0\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 7, sizeof(cl_mem), angular_flux_in_1);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_1\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 8, sizeof(cl_mem), angular_flux_in_2);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_2\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 9, sizeof(cl_mem), angular_flux_in_3);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_3\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 10, sizeof(cl_mem), angular_flux_in_4);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_4\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 11, sizeof(cl_mem), angular_flux_in_5);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_5\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 12, sizeof(cl_mem), angular_flux_in_6);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_6\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 13, sizeof(cl_mem), angular_flux_in_7);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_7\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 14, sizeof(cl_mem), angular_flux_out_0);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_0\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 15, sizeof(cl_mem), angular_flux_out_1);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_1\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 16, sizeof(cl_mem), angular_flux_out_2);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_2\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 17, sizeof(cl_mem), angular_flux_out_3);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_3\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 18, sizeof(cl_mem), angular_flux_out_4);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_4\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 19, sizeof(cl_mem), angular_flux_out_5);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_5\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 20, sizeof(cl_mem), angular_flux_out_6);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_6\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 21, sizeof(cl_mem), angular_flux_out_7);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_7\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 22, sizeof(cl_mem), velocity_delta);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"velocity_delta\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 23, sizeof(cl_mem), quad_weights);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"quad_weights\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 24, sizeof(cl_mem), scat_coeff);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scat_coeff\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 25, sizeof(cl_mem), scalar_flux_moments);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scalar_flux_moments\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->reduce_flux_moments_reduce_flux_moments_kernel, 26, sizeof(cl_double) * local_scalar_num_local_elems, NULL);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"local_scalar\", host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->reduce_flux_moments_reduce_flux_moments_kernel, 3, meta_offset, grid, (nullBlock ? NULL : block), 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
    if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"meta_gen_opencl_reduce_flux_moments_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

/** Automatically-generated by MetaGen-CL
\param queue the cl_command_queue on which to enqueue the kernel
\param grid_size a size_t[3] providing the number of workgroups in the X and Y dimensions, and the number of iterations in the Z dimension
\param block_size a size_t[3] providing the workgroup size in the X, Y, Z dimensions
\param nx scalar parameter of type "cl_uint"
\param ny scalar parameter of type "cl_uint"
\param nz scalar parameter of type "cl_uint"
\param nang scalar parameter of type "cl_uint"
\param ng scalar parameter of type "cl_uint"
\param cmom scalar parameter of type "cl_uint"
\param istep scalar parameter of type "cl_int"
\param jstep scalar parameter of type "cl_int"
\param kstep scalar parameter of type "cl_int"
\param oct scalar parameter of type "cl_uint"
\param z_pos scalar parameter of type "cl_uint"
\param plane a cl_mem buffer, must internally store struct cell_id types
\param source a cl_mem buffer, must internally store cl_double types
\param scat_coeff a cl_mem buffer, must internally store cl_double types
\param dd_i a cl_mem buffer, must internally store cl_double types
\param dd_j a cl_mem buffer, must internally store cl_double types
\param dd_k a cl_mem buffer, must internally store cl_double types
\param mu a cl_mem buffer, must internally store cl_double types
\param velocity_delta a cl_mem buffer, must internally store cl_double types
\param mat_cross_section a cl_mem buffer, must internally store cl_double types
\param denominator a cl_mem buffer, must internally store cl_double types
\param angular_flux_in a cl_mem buffer, must internally store cl_double types
\param flux_i a cl_mem buffer, must internally store cl_double types
\param flux_j a cl_mem buffer, must internally store cl_double types
\param flux_k a cl_mem buffer, must internally store cl_double types
\param angular_flux_out a cl_mem buffer, must internally store cl_double types
\param async whether the kernel should run asynchronously
\param event returns the cl_event corresponding to the kernel launch if run asynchronously
*/
cl_int meta_gen_opencl_sweep_plane_sweep_plane(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], size_t (*meta_offset)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint nang, cl_uint ng, cl_uint cmom, cl_int istep, cl_int jstep, cl_int kstep, cl_uint oct, cl_uint z_pos, cl_mem * plane, cl_mem * source, cl_mem * scat_coeff, cl_mem * dd_i, cl_mem * dd_j, cl_mem * dd_k, cl_mem * mu, cl_mem * velocity_delta, cl_mem * mat_cross_section, cl_mem * denominator, cl_mem * angular_flux_in, cl_mem * flux_i, cl_mem * flux_j, cl_mem * flux_k, cl_mem * angular_flux_out, int async, cl_event * event) {
  if (meta_gen_opencl_metacl_module_registration == NULL) meta_register_module(&meta_gen_opencl_metacl_module_registry);
  struct __meta_gen_opencl_metacl_module_frame * frame = __meta_gen_opencl_metacl_module_current_frame;
  if (queue != NULL) frame = __meta_gen_opencl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->sweep_plane_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  a_bool nullBlock = (block_size != NULL && (*block_size)[0] == 0 && (*block_size)[1] == 0 && (*block_size)[2] == 0);
  size_t grid[3];
  size_t block[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  int iters;

  //Default runs a single workgroup
  if (grid_size == NULL || block_size == NULL) {
    grid[0] = block[0];
    grid[1] = block[1];
    grid[2] = block[2];
    iters = 1;
  } else {
    grid[0] = (*grid_size)[0] * (nullBlock ? 1 : (*block_size)[0]);
    grid[1] = (*grid_size)[1] * (nullBlock ? 1 : (*block_size)[1]);
    grid[2] = (*grid_size)[2] * (nullBlock ? 1 : (*block_size)[2]);
    block[0] = (*block_size)[0];
    block[1] = (*block_size)[1];
    block[2] = (*block_size)[2];
    iters = (*grid_size)[2];
  }
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 0, sizeof(cl_uint), &nx);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nx\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 1, sizeof(cl_uint), &ny);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ny\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 2, sizeof(cl_uint), &nz);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nz\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 3, sizeof(cl_uint), &nang);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nang\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 4, sizeof(cl_uint), &ng);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ng\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 5, sizeof(cl_uint), &cmom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"cmom\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 6, sizeof(cl_int), &istep);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"istep\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 7, sizeof(cl_int), &jstep);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"jstep\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 8, sizeof(cl_int), &kstep);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"kstep\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 9, sizeof(cl_uint), &oct);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"oct\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 10, sizeof(cl_uint), &z_pos);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"z_pos\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 11, sizeof(cl_mem), plane);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"plane\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 12, sizeof(cl_mem), source);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"source\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 13, sizeof(cl_mem), scat_coeff);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scat_coeff\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 14, sizeof(cl_mem), dd_i);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_i\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 15, sizeof(cl_mem), dd_j);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_j\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 16, sizeof(cl_mem), dd_k);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_k\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 17, sizeof(cl_mem), mu);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"mu\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 18, sizeof(cl_mem), velocity_delta);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"velocity_delta\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 19, sizeof(cl_mem), mat_cross_section);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"mat_cross_section\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 20, sizeof(cl_mem), denominator);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"denominator\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 21, sizeof(cl_mem), angular_flux_in);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 22, sizeof(cl_mem), flux_i);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"flux_i\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 23, sizeof(cl_mem), flux_j);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"flux_j\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 24, sizeof(cl_mem), flux_k);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"flux_k\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_plane_sweep_plane_kernel, 25, sizeof(cl_mem), angular_flux_out);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out\", host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->sweep_plane_sweep_plane_kernel, 3, meta_offset, grid, (nullBlock ? NULL : block), 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
    if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"meta_gen_opencl_sweep_plane_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

/** Automatically-generated by MetaGen-CL
\param queue the cl_command_queue on which to enqueue the kernel
\param grid_size a size_t[3] providing the number of workgroups in the X and Y dimensions, and the number of iterations in the Z dimension
\param block_size a size_t[3] providing the workgroup size in the X, Y, Z dimensions
\param buffer a cl_mem buffer, must internally store cl_double types
\param async whether the kernel should run asynchronously
\param event returns the cl_event corresponding to the kernel launch if run asynchronously
*/
cl_int meta_gen_opencl_zero_buffer_zero_buffer(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], size_t (*meta_offset)[3], cl_mem * buffer, int async, cl_event * event) {
  if (meta_gen_opencl_metacl_module_registration == NULL) meta_register_module(&meta_gen_opencl_metacl_module_registry);
  struct __meta_gen_opencl_metacl_module_frame * frame = __meta_gen_opencl_metacl_module_current_frame;
  if (queue != NULL) frame = __meta_gen_opencl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->zero_buffer_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  a_bool nullBlock = (block_size != NULL && (*block_size)[0] == 0 && (*block_size)[1] == 0 && (*block_size)[2] == 0);
  size_t grid[3];
  size_t block[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  int iters;

  //Default runs a single workgroup
  if (grid_size == NULL || block_size == NULL) {
    grid[0] = block[0];
    grid[1] = block[1];
    grid[2] = block[2];
    iters = 1;
  } else {
    grid[0] = (*grid_size)[0] * (nullBlock ? 1 : (*block_size)[0]);
    grid[1] = (*grid_size)[1] * (nullBlock ? 1 : (*block_size)[1]);
    grid[2] = (*grid_size)[2] * (nullBlock ? 1 : (*block_size)[2]);
    block[0] = (*block_size)[0];
    block[1] = (*block_size)[1];
    block[2] = (*block_size)[2];
    iters = (*grid_size)[2];
  }
  retCode = clSetKernelArg(frame->zero_buffer_zero_buffer_kernel, 0, sizeof(cl_mem), buffer);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"buffer\", host wrapper: \"meta_gen_opencl_zero_buffer_zero_buffer\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->zero_buffer_zero_buffer_kernel, 3, meta_offset, grid, (nullBlock ? NULL : block), 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"meta_gen_opencl_zero_buffer_zero_buffer\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
    if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"meta_gen_opencl_zero_buffer_zero_buffer\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

