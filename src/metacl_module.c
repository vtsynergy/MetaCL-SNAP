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
const char * __metacl_outer_zero_and_others_custom_args = NULL;
//TODO: Expose this with a function (with safety checks) rather than a variable
const char * __metacl_sweep_zero_inner_reducef_custom_args = NULL;
struct __metacl_metacl_module_frame * __metacl_metacl_module_current_frame = NULL;

struct __metacl_metacl_module_frame * __metacl_metacl_module_lookup_frame(cl_command_queue queue) {
  struct __metacl_metacl_module_frame * current = __metacl_metacl_module_current_frame;
  while (current != NULL) {
    if (current->queue == queue) break;
    current = current->next_frame;
  }
  return current;
}
a_module_record * metacl_metacl_module_registration = NULL;
a_module_record * metacl_metacl_module_registry(a_module_record * record) {
  if (record == NULL) return metacl_metacl_module_registration;
  a_module_record * old_registration = metacl_metacl_module_registration;
  if (old_registration == NULL) {
    record->implements = module_implements_opencl;
    record->module_init = &metacl_metacl_module_init;
    record->module_deinit = &metacl_metacl_module_deinit;
    record->module_registry_func = &metacl_metacl_module_registry;
    metacl_metacl_module_registration = record;
  }
  if (old_registration != NULL && old_registration != record) return record;
  if (old_registration == record) metacl_metacl_module_registration = NULL;
  return old_registration;
}
void metacl_metacl_module_init() {
  cl_int buildError, createError;
  if (metacl_metacl_module_registration == NULL) {
    meta_register_module(&metacl_metacl_module_registry);
    return;
  }
  if (meta_context == NULL) metaOpenCLFallBack();
  struct __metacl_metacl_module_frame * new_frame = (struct __metacl_metacl_module_frame *) calloc(1, sizeof(struct __metacl_metacl_module_frame));
  new_frame->next_frame = __metacl_metacl_module_current_frame;
  new_frame->device = meta_device;
  new_frame->queue = meta_queue;
  new_frame->context = meta_context;
  __metacl_metacl_module_current_frame = new_frame;
  meta_cl_device_vendor vendor = metaOpenCLDetectDevice(new_frame->device);
  if ((vendor & meta_cl_device_is_accel) && ((vendor & meta_cl_device_vendor_mask) == meta_cl_device_vendor_intelfpga)) {
    __metacl_metacl_module_current_frame->outer_zero_and_others_progLen = metaOpenCLLoadProgramSource("outer_zero_and_others.aocx", &__metacl_metacl_module_current_frame->outer_zero_and_others_progSrc);
    if (__metacl_metacl_module_current_frame->outer_zero_and_others_progLen != -1)
      __metacl_metacl_module_current_frame->outer_zero_and_others_prog = clCreateProgramWithBinary(meta_context, 1, &meta_device, &__metacl_metacl_module_current_frame->outer_zero_and_others_progLen, (const unsigned char **)&__metacl_metacl_module_current_frame->outer_zero_and_others_progSrc, NULL, &buildError);
  } else {
    __metacl_metacl_module_current_frame->outer_zero_and_others_progLen = metaOpenCLLoadProgramSource("outer_zero_and_others.cl", &__metacl_metacl_module_current_frame->outer_zero_and_others_progSrc);
    if (__metacl_metacl_module_current_frame->outer_zero_and_others_progLen != -1)
      __metacl_metacl_module_current_frame->outer_zero_and_others_prog = clCreateProgramWithSource(meta_context, 1, &__metacl_metacl_module_current_frame->outer_zero_and_others_progSrc, &__metacl_metacl_module_current_frame->outer_zero_and_others_progLen, &buildError);
  }
  if (__metacl_metacl_module_current_frame->outer_zero_and_others_progLen != -1) {
    if (buildError != CL_SUCCESS) fprintf(stderr, "OpenCL program creation error %d at %s:%d\n", buildError, __FILE__, __LINE__);
    buildError = clBuildProgram(__metacl_metacl_module_current_frame->outer_zero_and_others_prog, 1, &meta_device, __metacl_outer_zero_and_others_custom_args ? __metacl_outer_zero_and_others_custom_args : "", NULL, NULL);
    if (buildError != CL_SUCCESS) {
      size_t logsize = 0;
      clGetProgramBuildInfo(__metacl_metacl_module_current_frame->outer_zero_and_others_prog, meta_device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logsize);
      char * buildLog = (char *) malloc(sizeof(char) * (logsize + 1));
      clGetProgramBuildInfo(__metacl_metacl_module_current_frame->outer_zero_and_others_prog, meta_device, CL_PROGRAM_BUILD_LOG, logsize, buildLog, NULL);
      if (buildError != CL_SUCCESS) fprintf(stderr, "OpenCL program build error %d at %s:%d\n", buildError, __FILE__, __LINE__);
      fprintf(stderr, "Build Log:\n%s\n", buildLog);
      free(buildLog);
    } else {
      __metacl_metacl_module_current_frame->outer_zero_and_others_init = 1;
    }
    __metacl_metacl_module_current_frame->outer_zero_and_others_reduce_flux_moments_kernel = clCreateKernel(__metacl_metacl_module_current_frame->outer_zero_and_others_prog, "reduce_flux_moments", &createError);
    if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
    __metacl_metacl_module_current_frame->outer_zero_and_others_calc_outer_source_kernel = clCreateKernel(__metacl_metacl_module_current_frame->outer_zero_and_others_prog, "calc_outer_source", &createError);
    if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
    __metacl_metacl_module_current_frame->outer_zero_and_others_calc_dd_coeff_kernel = clCreateKernel(__metacl_metacl_module_current_frame->outer_zero_and_others_prog, "calc_dd_coeff", &createError);
    if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
    __metacl_metacl_module_current_frame->outer_zero_and_others_calc_velocity_delta_kernel = clCreateKernel(__metacl_metacl_module_current_frame->outer_zero_and_others_prog, "calc_velocity_delta", &createError);
    if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
    __metacl_metacl_module_current_frame->outer_zero_and_others_calc_denominator_kernel = clCreateKernel(__metacl_metacl_module_current_frame->outer_zero_and_others_prog, "calc_denominator", &createError);
    if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
    __metacl_metacl_module_current_frame->outer_zero_and_others_zero_buffer_kernel = clCreateKernel(__metacl_metacl_module_current_frame->outer_zero_and_others_prog, "zero_buffer", &createError);
    if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
  }
  if ((vendor & meta_cl_device_is_accel) && ((vendor & meta_cl_device_vendor_mask) == meta_cl_device_vendor_intelfpga)) {
    __metacl_metacl_module_current_frame->sweep_zero_inner_reducef_progLen = metaOpenCLLoadProgramSource("sweep_zero_inner_reducef.aocx", &__metacl_metacl_module_current_frame->sweep_zero_inner_reducef_progSrc);
    if (__metacl_metacl_module_current_frame->sweep_zero_inner_reducef_progLen != -1)
      __metacl_metacl_module_current_frame->sweep_zero_inner_reducef_prog = clCreateProgramWithBinary(meta_context, 1, &meta_device, &__metacl_metacl_module_current_frame->sweep_zero_inner_reducef_progLen, (const unsigned char **)&__metacl_metacl_module_current_frame->sweep_zero_inner_reducef_progSrc, NULL, &buildError);
  } else {
    __metacl_metacl_module_current_frame->sweep_zero_inner_reducef_progLen = metaOpenCLLoadProgramSource("sweep_zero_inner_reducef.cl", &__metacl_metacl_module_current_frame->sweep_zero_inner_reducef_progSrc);
    if (__metacl_metacl_module_current_frame->sweep_zero_inner_reducef_progLen != -1)
      __metacl_metacl_module_current_frame->sweep_zero_inner_reducef_prog = clCreateProgramWithSource(meta_context, 1, &__metacl_metacl_module_current_frame->sweep_zero_inner_reducef_progSrc, &__metacl_metacl_module_current_frame->sweep_zero_inner_reducef_progLen, &buildError);
  }
  if (__metacl_metacl_module_current_frame->sweep_zero_inner_reducef_progLen != -1) {
    if (buildError != CL_SUCCESS) fprintf(stderr, "OpenCL program creation error %d at %s:%d\n", buildError, __FILE__, __LINE__);
    buildError = clBuildProgram(__metacl_metacl_module_current_frame->sweep_zero_inner_reducef_prog, 1, &meta_device, __metacl_sweep_zero_inner_reducef_custom_args ? __metacl_sweep_zero_inner_reducef_custom_args : "", NULL, NULL);
    if (buildError != CL_SUCCESS) {
      size_t logsize = 0;
      clGetProgramBuildInfo(__metacl_metacl_module_current_frame->sweep_zero_inner_reducef_prog, meta_device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logsize);
      char * buildLog = (char *) malloc(sizeof(char) * (logsize + 1));
      clGetProgramBuildInfo(__metacl_metacl_module_current_frame->sweep_zero_inner_reducef_prog, meta_device, CL_PROGRAM_BUILD_LOG, logsize, buildLog, NULL);
      if (buildError != CL_SUCCESS) fprintf(stderr, "OpenCL program build error %d at %s:%d\n", buildError, __FILE__, __LINE__);
      fprintf(stderr, "Build Log:\n%s\n", buildLog);
      free(buildLog);
    } else {
      __metacl_metacl_module_current_frame->sweep_zero_inner_reducef_init = 1;
    }
    __metacl_metacl_module_current_frame->sweep_zero_inner_reducef_sweep_plane_kernel = clCreateKernel(__metacl_metacl_module_current_frame->sweep_zero_inner_reducef_prog, "sweep_plane", &createError);
    if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
    __metacl_metacl_module_current_frame->sweep_zero_inner_reducef_reduce_flux_kernel = clCreateKernel(__metacl_metacl_module_current_frame->sweep_zero_inner_reducef_prog, "reduce_flux", &createError);
    if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
    __metacl_metacl_module_current_frame->sweep_zero_inner_reducef_calc_inner_source_kernel = clCreateKernel(__metacl_metacl_module_current_frame->sweep_zero_inner_reducef_prog, "calc_inner_source", &createError);
    if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
    __metacl_metacl_module_current_frame->sweep_zero_inner_reducef_zero_buffer_kernel = clCreateKernel(__metacl_metacl_module_current_frame->sweep_zero_inner_reducef_prog, "zero_buffer", &createError);
    if (createError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel creation error %d at %s:%d\n", createError, __FILE__, __LINE__);
  }
  metacl_metacl_module_registration->initialized = 1;
}

void metacl_metacl_module_deinit() {
  cl_int releaseError;
  if (__metacl_metacl_module_current_frame != NULL) {
    if (__metacl_metacl_module_current_frame->outer_zero_and_others_progLen != -1) {
      releaseError = clReleaseKernel(__metacl_metacl_module_current_frame->outer_zero_and_others_reduce_flux_moments_kernel);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      releaseError = clReleaseKernel(__metacl_metacl_module_current_frame->outer_zero_and_others_calc_outer_source_kernel);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      releaseError = clReleaseKernel(__metacl_metacl_module_current_frame->outer_zero_and_others_calc_dd_coeff_kernel);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      releaseError = clReleaseKernel(__metacl_metacl_module_current_frame->outer_zero_and_others_calc_velocity_delta_kernel);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      releaseError = clReleaseKernel(__metacl_metacl_module_current_frame->outer_zero_and_others_calc_denominator_kernel);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      releaseError = clReleaseKernel(__metacl_metacl_module_current_frame->outer_zero_and_others_zero_buffer_kernel);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      releaseError = clReleaseProgram(__metacl_metacl_module_current_frame->outer_zero_and_others_prog);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL program release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      free((char *)__metacl_metacl_module_current_frame->outer_zero_and_others_progSrc);
      __metacl_metacl_module_current_frame->outer_zero_and_others_progLen = 0;
      __metacl_metacl_module_current_frame->outer_zero_and_others_init = 0;
    }
    if (__metacl_metacl_module_current_frame->sweep_zero_inner_reducef_progLen != -1) {
      releaseError = clReleaseKernel(__metacl_metacl_module_current_frame->sweep_zero_inner_reducef_sweep_plane_kernel);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      releaseError = clReleaseKernel(__metacl_metacl_module_current_frame->sweep_zero_inner_reducef_reduce_flux_kernel);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      releaseError = clReleaseKernel(__metacl_metacl_module_current_frame->sweep_zero_inner_reducef_calc_inner_source_kernel);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      releaseError = clReleaseKernel(__metacl_metacl_module_current_frame->sweep_zero_inner_reducef_zero_buffer_kernel);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL kernel release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      releaseError = clReleaseProgram(__metacl_metacl_module_current_frame->sweep_zero_inner_reducef_prog);
      if (releaseError != CL_SUCCESS) fprintf(stderr, "OpenCL program release error %d at %s:%d\n", releaseError, __FILE__, __LINE__);
      free((char *)__metacl_metacl_module_current_frame->sweep_zero_inner_reducef_progSrc);
      __metacl_metacl_module_current_frame->sweep_zero_inner_reducef_progLen = 0;
      __metacl_metacl_module_current_frame->sweep_zero_inner_reducef_init = 0;
    }
    struct __metacl_metacl_module_frame * next_frame = __metacl_metacl_module_current_frame->next_frame;
    free(__metacl_metacl_module_current_frame);
    __metacl_metacl_module_current_frame = next_frame;
    if (__metacl_metacl_module_current_frame == NULL && metacl_metacl_module_registration != NULL) {
      metacl_metacl_module_registration->initialized = 0;
    }
  }
}

cl_int metacl_outer_zero_and_others_reduce_flux_moments(cl_command_queue queue, size_t (*global_size)[3], size_t (*local_size)[3], size_t (*meta_offset)[3], int async, cl_event * event, cl_uint nx, cl_uint ny, cl_uint nz, cl_uint nang, cl_uint ng, cl_uint cmom, cl_mem * angular_flux_in_0, cl_mem * angular_flux_in_1, cl_mem * angular_flux_in_2, cl_mem * angular_flux_in_3, cl_mem * angular_flux_in_4, cl_mem * angular_flux_in_5, cl_mem * angular_flux_in_6, cl_mem * angular_flux_in_7, cl_mem * angular_flux_out_0, cl_mem * angular_flux_out_1, cl_mem * angular_flux_out_2, cl_mem * angular_flux_out_3, cl_mem * angular_flux_out_4, cl_mem * angular_flux_out_5, cl_mem * angular_flux_out_6, cl_mem * angular_flux_out_7, cl_mem * velocity_delta, cl_mem * quad_weights, cl_mem * scat_coeff, cl_mem * scalar_flux_moments, size_t local_scalar_num_local_elems) {
  if (metacl_metacl_module_registration == NULL) meta_register_module(&metacl_metacl_module_registry);
  struct __metacl_metacl_module_frame * frame = __metacl_metacl_module_current_frame;
  if (queue != NULL) frame = __metacl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->outer_zero_and_others_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  a_bool nullBlock = (local_size != NULL && (*local_size)[0] == 0 && (*local_size)[1] == 0 && (*local_size)[2] == 0);
  size_t _global_size[3];
  size_t _local_size[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  size_t _temp_offset[3];
  if (meta_offset == NULL) { _temp_offset[0] = 0, _temp_offset[1] = 0, _temp_offset[2] = 0;}
  else { _temp_offset[0] = (*meta_offset)[0], _temp_offset[1] = (*meta_offset)[1], _temp_offset[2] = (*meta_offset)[2]; }
  const size_t _meta_offset[3] = {_temp_offset[0], _temp_offset[1], _temp_offset[2]};
  int iters;

  //Default runs a single workgroup
  if (global_size == NULL || local_size == NULL) {
    _global_size[0] = _local_size[0];
    _global_size[1] = _local_size[1];
    _global_size[2] = _local_size[2];
    iters = 1;
  } else {
    _global_size[0] = (*global_size)[0];
    _global_size[1] = (*global_size)[1];
    _global_size[2] = (*global_size)[2];
    _local_size[0] = (*local_size)[0];
    _local_size[1] = (*local_size)[1];
    _local_size[2] = (*local_size)[2];
  }
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 0, sizeof(cl_uint), &nx);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nx\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 1, sizeof(cl_uint), &ny);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ny\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 2, sizeof(cl_uint), &nz);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nz\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 3, sizeof(cl_uint), &nang);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nang\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 4, sizeof(cl_uint), &ng);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ng\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 5, sizeof(cl_uint), &cmom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"cmom\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 6, sizeof(cl_mem), angular_flux_in_0);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_0\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 7, sizeof(cl_mem), angular_flux_in_1);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_1\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 8, sizeof(cl_mem), angular_flux_in_2);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_2\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 9, sizeof(cl_mem), angular_flux_in_3);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_3\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 10, sizeof(cl_mem), angular_flux_in_4);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_4\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 11, sizeof(cl_mem), angular_flux_in_5);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_5\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 12, sizeof(cl_mem), angular_flux_in_6);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_6\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 13, sizeof(cl_mem), angular_flux_in_7);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_7\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 14, sizeof(cl_mem), angular_flux_out_0);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_0\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 15, sizeof(cl_mem), angular_flux_out_1);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_1\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 16, sizeof(cl_mem), angular_flux_out_2);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_2\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 17, sizeof(cl_mem), angular_flux_out_3);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_3\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 18, sizeof(cl_mem), angular_flux_out_4);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_4\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 19, sizeof(cl_mem), angular_flux_out_5);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_5\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 20, sizeof(cl_mem), angular_flux_out_6);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_6\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 21, sizeof(cl_mem), angular_flux_out_7);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_7\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 22, sizeof(cl_mem), velocity_delta);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"velocity_delta\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 23, sizeof(cl_mem), quad_weights);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"quad_weights\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 24, sizeof(cl_mem), scat_coeff);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scat_coeff\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 25, sizeof(cl_mem), scalar_flux_moments);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scalar_flux_moments\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_reduce_flux_moments_kernel, 26, sizeof(cl_double) * local_scalar_num_local_elems, NULL);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"local_scalar\", host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->outer_zero_and_others_reduce_flux_moments_kernel, 3, &_meta_offset[0], _global_size, (nullBlock ? NULL : _local_size), 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
    if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"metacl_outer_zero_and_others_reduce_flux_moments\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

cl_int metacl_outer_zero_and_others_calc_outer_source(cl_command_queue queue, size_t (*global_size)[3], size_t (*local_size)[3], size_t (*meta_offset)[3], int async, cl_event * event, cl_uint nx, cl_uint ny, cl_uint nz, cl_uint ng, cl_uint cmom, cl_uint nmom, cl_mem * fixed_source, cl_mem * scattering_matrix, cl_mem * scalar_flux, cl_mem * scalar_flux_moments, cl_mem * outer_source) {
  if (metacl_metacl_module_registration == NULL) meta_register_module(&metacl_metacl_module_registry);
  struct __metacl_metacl_module_frame * frame = __metacl_metacl_module_current_frame;
  if (queue != NULL) frame = __metacl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->outer_zero_and_others_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  a_bool nullBlock = (local_size != NULL && (*local_size)[0] == 0 && (*local_size)[1] == 0 && (*local_size)[2] == 0);
  size_t _global_size[3];
  size_t _local_size[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  size_t _temp_offset[3];
  if (meta_offset == NULL) { _temp_offset[0] = 0, _temp_offset[1] = 0, _temp_offset[2] = 0;}
  else { _temp_offset[0] = (*meta_offset)[0], _temp_offset[1] = (*meta_offset)[1], _temp_offset[2] = (*meta_offset)[2]; }
  const size_t _meta_offset[3] = {_temp_offset[0], _temp_offset[1], _temp_offset[2]};
  int iters;

  //Default runs a single workgroup
  if (global_size == NULL || local_size == NULL) {
    _global_size[0] = _local_size[0];
    _global_size[1] = _local_size[1];
    _global_size[2] = _local_size[2];
    iters = 1;
  } else {
    _global_size[0] = (*global_size)[0];
    _global_size[1] = (*global_size)[1];
    _global_size[2] = (*global_size)[2];
    _local_size[0] = (*local_size)[0];
    _local_size[1] = (*local_size)[1];
    _local_size[2] = (*local_size)[2];
  }
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_outer_source_kernel, 0, sizeof(cl_uint), &nx);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nx\", host wrapper: \"metacl_outer_zero_and_others_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_outer_source_kernel, 1, sizeof(cl_uint), &ny);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ny\", host wrapper: \"metacl_outer_zero_and_others_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_outer_source_kernel, 2, sizeof(cl_uint), &nz);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nz\", host wrapper: \"metacl_outer_zero_and_others_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_outer_source_kernel, 3, sizeof(cl_uint), &ng);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ng\", host wrapper: \"metacl_outer_zero_and_others_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_outer_source_kernel, 4, sizeof(cl_uint), &cmom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"cmom\", host wrapper: \"metacl_outer_zero_and_others_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_outer_source_kernel, 5, sizeof(cl_uint), &nmom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nmom\", host wrapper: \"metacl_outer_zero_and_others_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_outer_source_kernel, 6, sizeof(cl_mem), fixed_source);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"fixed_source\", host wrapper: \"metacl_outer_zero_and_others_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_outer_source_kernel, 7, sizeof(cl_mem), scattering_matrix);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scattering_matrix\", host wrapper: \"metacl_outer_zero_and_others_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_outer_source_kernel, 8, sizeof(cl_mem), scalar_flux);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scalar_flux\", host wrapper: \"metacl_outer_zero_and_others_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_outer_source_kernel, 9, sizeof(cl_mem), scalar_flux_moments);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scalar_flux_moments\", host wrapper: \"metacl_outer_zero_and_others_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_outer_source_kernel, 10, sizeof(cl_mem), outer_source);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"outer_source\", host wrapper: \"metacl_outer_zero_and_others_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->outer_zero_and_others_calc_outer_source_kernel, 3, &_meta_offset[0], _global_size, (nullBlock ? NULL : _local_size), 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"metacl_outer_zero_and_others_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
    if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"metacl_outer_zero_and_others_calc_outer_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

cl_int metacl_outer_zero_and_others_calc_dd_coeff(cl_command_queue queue, size_t (*global_size)[3], size_t (*local_size)[3], size_t (*meta_offset)[3], int async, cl_event * event, cl_double dx, cl_double dy, cl_double dz, cl_mem * eta, cl_mem * xi, cl_mem * dd_i, cl_mem * dd_j, cl_mem * dd_k) {
  if (metacl_metacl_module_registration == NULL) meta_register_module(&metacl_metacl_module_registry);
  struct __metacl_metacl_module_frame * frame = __metacl_metacl_module_current_frame;
  if (queue != NULL) frame = __metacl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->outer_zero_and_others_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  a_bool nullBlock = (local_size != NULL && (*local_size)[0] == 0 && (*local_size)[1] == 0 && (*local_size)[2] == 0);
  size_t _global_size[3];
  size_t _local_size[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  size_t _temp_offset[3];
  if (meta_offset == NULL) { _temp_offset[0] = 0, _temp_offset[1] = 0, _temp_offset[2] = 0;}
  else { _temp_offset[0] = (*meta_offset)[0], _temp_offset[1] = (*meta_offset)[1], _temp_offset[2] = (*meta_offset)[2]; }
  const size_t _meta_offset[3] = {_temp_offset[0], _temp_offset[1], _temp_offset[2]};
  int iters;

  //Default runs a single workgroup
  if (global_size == NULL || local_size == NULL) {
    _global_size[0] = _local_size[0];
    _global_size[1] = _local_size[1];
    _global_size[2] = _local_size[2];
    iters = 1;
  } else {
    _global_size[0] = (*global_size)[0];
    _global_size[1] = (*global_size)[1];
    _global_size[2] = (*global_size)[2];
    _local_size[0] = (*local_size)[0];
    _local_size[1] = (*local_size)[1];
    _local_size[2] = (*local_size)[2];
  }
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_dd_coeff_kernel, 0, sizeof(cl_double), &dx);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dx\", host wrapper: \"metacl_outer_zero_and_others_calc_dd_coeff\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_dd_coeff_kernel, 1, sizeof(cl_double), &dy);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dy\", host wrapper: \"metacl_outer_zero_and_others_calc_dd_coeff\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_dd_coeff_kernel, 2, sizeof(cl_double), &dz);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dz\", host wrapper: \"metacl_outer_zero_and_others_calc_dd_coeff\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_dd_coeff_kernel, 3, sizeof(cl_mem), eta);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"eta\", host wrapper: \"metacl_outer_zero_and_others_calc_dd_coeff\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_dd_coeff_kernel, 4, sizeof(cl_mem), xi);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"xi\", host wrapper: \"metacl_outer_zero_and_others_calc_dd_coeff\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_dd_coeff_kernel, 5, sizeof(cl_mem), dd_i);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_i\", host wrapper: \"metacl_outer_zero_and_others_calc_dd_coeff\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_dd_coeff_kernel, 6, sizeof(cl_mem), dd_j);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_j\", host wrapper: \"metacl_outer_zero_and_others_calc_dd_coeff\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_dd_coeff_kernel, 7, sizeof(cl_mem), dd_k);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_k\", host wrapper: \"metacl_outer_zero_and_others_calc_dd_coeff\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->outer_zero_and_others_calc_dd_coeff_kernel, 3, &_meta_offset[0], _global_size, (nullBlock ? NULL : _local_size), 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"metacl_outer_zero_and_others_calc_dd_coeff\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
    if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"metacl_outer_zero_and_others_calc_dd_coeff\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

cl_int metacl_outer_zero_and_others_calc_velocity_delta(cl_command_queue queue, size_t (*global_size)[3], size_t (*local_size)[3], size_t (*meta_offset)[3], int async, cl_event * event, cl_mem * velocities, cl_double dt, cl_mem * velocity_delta) {
  if (metacl_metacl_module_registration == NULL) meta_register_module(&metacl_metacl_module_registry);
  struct __metacl_metacl_module_frame * frame = __metacl_metacl_module_current_frame;
  if (queue != NULL) frame = __metacl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->outer_zero_and_others_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  a_bool nullBlock = (local_size != NULL && (*local_size)[0] == 0 && (*local_size)[1] == 0 && (*local_size)[2] == 0);
  size_t _global_size[3];
  size_t _local_size[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  size_t _temp_offset[3];
  if (meta_offset == NULL) { _temp_offset[0] = 0, _temp_offset[1] = 0, _temp_offset[2] = 0;}
  else { _temp_offset[0] = (*meta_offset)[0], _temp_offset[1] = (*meta_offset)[1], _temp_offset[2] = (*meta_offset)[2]; }
  const size_t _meta_offset[3] = {_temp_offset[0], _temp_offset[1], _temp_offset[2]};
  int iters;

  //Default runs a single workgroup
  if (global_size == NULL || local_size == NULL) {
    _global_size[0] = _local_size[0];
    _global_size[1] = _local_size[1];
    _global_size[2] = _local_size[2];
    iters = 1;
  } else {
    _global_size[0] = (*global_size)[0];
    _global_size[1] = (*global_size)[1];
    _global_size[2] = (*global_size)[2];
    _local_size[0] = (*local_size)[0];
    _local_size[1] = (*local_size)[1];
    _local_size[2] = (*local_size)[2];
  }
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_velocity_delta_kernel, 0, sizeof(cl_mem), velocities);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"velocities\", host wrapper: \"metacl_outer_zero_and_others_calc_velocity_delta\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_velocity_delta_kernel, 1, sizeof(cl_double), &dt);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dt\", host wrapper: \"metacl_outer_zero_and_others_calc_velocity_delta\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_velocity_delta_kernel, 2, sizeof(cl_mem), velocity_delta);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"velocity_delta\", host wrapper: \"metacl_outer_zero_and_others_calc_velocity_delta\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->outer_zero_and_others_calc_velocity_delta_kernel, 3, &_meta_offset[0], _global_size, (nullBlock ? NULL : _local_size), 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"metacl_outer_zero_and_others_calc_velocity_delta\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
    if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"metacl_outer_zero_and_others_calc_velocity_delta\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

cl_int metacl_outer_zero_and_others_calc_denominator(cl_command_queue queue, size_t (*global_size)[3], size_t (*local_size)[3], size_t (*meta_offset)[3], int async, cl_event * event, cl_uint nx, cl_uint ny, cl_uint nz, cl_uint nang, cl_uint ng, cl_mem * mat_cross_section, cl_mem * velocity_delta, cl_mem * mu, cl_mem * dd_i, cl_mem * dd_j, cl_mem * dd_k, cl_mem * denominator) {
  if (metacl_metacl_module_registration == NULL) meta_register_module(&metacl_metacl_module_registry);
  struct __metacl_metacl_module_frame * frame = __metacl_metacl_module_current_frame;
  if (queue != NULL) frame = __metacl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->outer_zero_and_others_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  a_bool nullBlock = (local_size != NULL && (*local_size)[0] == 0 && (*local_size)[1] == 0 && (*local_size)[2] == 0);
  size_t _global_size[3];
  size_t _local_size[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  size_t _temp_offset[3];
  if (meta_offset == NULL) { _temp_offset[0] = 0, _temp_offset[1] = 0, _temp_offset[2] = 0;}
  else { _temp_offset[0] = (*meta_offset)[0], _temp_offset[1] = (*meta_offset)[1], _temp_offset[2] = (*meta_offset)[2]; }
  const size_t _meta_offset[3] = {_temp_offset[0], _temp_offset[1], _temp_offset[2]};
  int iters;

  //Default runs a single workgroup
  if (global_size == NULL || local_size == NULL) {
    _global_size[0] = _local_size[0];
    _global_size[1] = _local_size[1];
    _global_size[2] = _local_size[2];
    iters = 1;
  } else {
    _global_size[0] = (*global_size)[0];
    _global_size[1] = (*global_size)[1];
    _global_size[2] = (*global_size)[2];
    _local_size[0] = (*local_size)[0];
    _local_size[1] = (*local_size)[1];
    _local_size[2] = (*local_size)[2];
  }
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_denominator_kernel, 0, sizeof(cl_uint), &nx);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nx\", host wrapper: \"metacl_outer_zero_and_others_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_denominator_kernel, 1, sizeof(cl_uint), &ny);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ny\", host wrapper: \"metacl_outer_zero_and_others_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_denominator_kernel, 2, sizeof(cl_uint), &nz);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nz\", host wrapper: \"metacl_outer_zero_and_others_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_denominator_kernel, 3, sizeof(cl_uint), &nang);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nang\", host wrapper: \"metacl_outer_zero_and_others_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_denominator_kernel, 4, sizeof(cl_uint), &ng);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ng\", host wrapper: \"metacl_outer_zero_and_others_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_denominator_kernel, 5, sizeof(cl_mem), mat_cross_section);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"mat_cross_section\", host wrapper: \"metacl_outer_zero_and_others_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_denominator_kernel, 6, sizeof(cl_mem), velocity_delta);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"velocity_delta\", host wrapper: \"metacl_outer_zero_and_others_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_denominator_kernel, 7, sizeof(cl_mem), mu);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"mu\", host wrapper: \"metacl_outer_zero_and_others_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_denominator_kernel, 8, sizeof(cl_mem), dd_i);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_i\", host wrapper: \"metacl_outer_zero_and_others_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_denominator_kernel, 9, sizeof(cl_mem), dd_j);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_j\", host wrapper: \"metacl_outer_zero_and_others_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_denominator_kernel, 10, sizeof(cl_mem), dd_k);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_k\", host wrapper: \"metacl_outer_zero_and_others_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->outer_zero_and_others_calc_denominator_kernel, 11, sizeof(cl_mem), denominator);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"denominator\", host wrapper: \"metacl_outer_zero_and_others_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->outer_zero_and_others_calc_denominator_kernel, 3, &_meta_offset[0], _global_size, (nullBlock ? NULL : _local_size), 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"metacl_outer_zero_and_others_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
    if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"metacl_outer_zero_and_others_calc_denominator\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

cl_int metacl_outer_zero_and_others_zero_buffer(cl_command_queue queue, size_t (*global_size)[3], size_t (*local_size)[3], size_t (*meta_offset)[3], int async, cl_event * event, cl_mem * buffer) {
  if (metacl_metacl_module_registration == NULL) meta_register_module(&metacl_metacl_module_registry);
  struct __metacl_metacl_module_frame * frame = __metacl_metacl_module_current_frame;
  if (queue != NULL) frame = __metacl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->outer_zero_and_others_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  a_bool nullBlock = (local_size != NULL && (*local_size)[0] == 0 && (*local_size)[1] == 0 && (*local_size)[2] == 0);
  size_t _global_size[3];
  size_t _local_size[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  size_t _temp_offset[3];
  if (meta_offset == NULL) { _temp_offset[0] = 0, _temp_offset[1] = 0, _temp_offset[2] = 0;}
  else { _temp_offset[0] = (*meta_offset)[0], _temp_offset[1] = (*meta_offset)[1], _temp_offset[2] = (*meta_offset)[2]; }
  const size_t _meta_offset[3] = {_temp_offset[0], _temp_offset[1], _temp_offset[2]};
  int iters;

  //Default runs a single workgroup
  if (global_size == NULL || local_size == NULL) {
    _global_size[0] = _local_size[0];
    _global_size[1] = _local_size[1];
    _global_size[2] = _local_size[2];
    iters = 1;
  } else {
    _global_size[0] = (*global_size)[0];
    _global_size[1] = (*global_size)[1];
    _global_size[2] = (*global_size)[2];
    _local_size[0] = (*local_size)[0];
    _local_size[1] = (*local_size)[1];
    _local_size[2] = (*local_size)[2];
  }
  retCode = clSetKernelArg(frame->outer_zero_and_others_zero_buffer_kernel, 0, sizeof(cl_mem), buffer);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"buffer\", host wrapper: \"metacl_outer_zero_and_others_zero_buffer\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->outer_zero_and_others_zero_buffer_kernel, 3, &_meta_offset[0], _global_size, (nullBlock ? NULL : _local_size), 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"metacl_outer_zero_and_others_zero_buffer\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
    if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"metacl_outer_zero_and_others_zero_buffer\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

cl_int metacl_sweep_zero_inner_reducef_sweep_plane(cl_command_queue queue, size_t (*global_size)[3], size_t (*local_size)[3], size_t (*meta_offset)[3], int async, cl_event * event, cl_uint nx, cl_uint ny, cl_uint nz, cl_uint nang, cl_uint ng, cl_uint cmom, cl_int istep, cl_int jstep, cl_int kstep, cl_uint oct, cl_uint z_pos, cl_mem * plane, cl_mem * source, cl_mem * scat_coeff, cl_mem * dd_i, cl_mem * dd_j, cl_mem * dd_k, cl_mem * mu, cl_mem * velocity_delta, cl_mem * mat_cross_section, cl_mem * denominator, cl_mem * angular_flux_in, cl_mem * flux_i, cl_mem * flux_j, cl_mem * flux_k, cl_mem * angular_flux_out) {
  if (metacl_metacl_module_registration == NULL) meta_register_module(&metacl_metacl_module_registry);
  struct __metacl_metacl_module_frame * frame = __metacl_metacl_module_current_frame;
  if (queue != NULL) frame = __metacl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->sweep_zero_inner_reducef_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  a_bool nullBlock = (local_size != NULL && (*local_size)[0] == 0 && (*local_size)[1] == 0 && (*local_size)[2] == 0);
  size_t _global_size[3];
  size_t _local_size[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  size_t _temp_offset[3];
  if (meta_offset == NULL) { _temp_offset[0] = 0, _temp_offset[1] = 0, _temp_offset[2] = 0;}
  else { _temp_offset[0] = (*meta_offset)[0], _temp_offset[1] = (*meta_offset)[1], _temp_offset[2] = (*meta_offset)[2]; }
  const size_t _meta_offset[3] = {_temp_offset[0], _temp_offset[1], _temp_offset[2]};
  int iters;

  //Default runs a single workgroup
  if (global_size == NULL || local_size == NULL) {
    _global_size[0] = _local_size[0];
    _global_size[1] = _local_size[1];
    _global_size[2] = _local_size[2];
    iters = 1;
  } else {
    _global_size[0] = (*global_size)[0];
    _global_size[1] = (*global_size)[1];
    _global_size[2] = (*global_size)[2];
    _local_size[0] = (*local_size)[0];
    _local_size[1] = (*local_size)[1];
    _local_size[2] = (*local_size)[2];
  }
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 0, sizeof(cl_uint), &nx);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nx\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 1, sizeof(cl_uint), &ny);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ny\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 2, sizeof(cl_uint), &nz);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nz\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 3, sizeof(cl_uint), &nang);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nang\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 4, sizeof(cl_uint), &ng);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ng\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 5, sizeof(cl_uint), &cmom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"cmom\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 6, sizeof(cl_int), &istep);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"istep\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 7, sizeof(cl_int), &jstep);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"jstep\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 8, sizeof(cl_int), &kstep);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"kstep\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 9, sizeof(cl_uint), &oct);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"oct\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 10, sizeof(cl_uint), &z_pos);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"z_pos\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 11, sizeof(cl_mem), plane);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"plane\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 12, sizeof(cl_mem), source);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"source\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 13, sizeof(cl_mem), scat_coeff);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scat_coeff\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 14, sizeof(cl_mem), dd_i);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_i\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 15, sizeof(cl_mem), dd_j);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_j\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 16, sizeof(cl_mem), dd_k);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"dd_k\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 17, sizeof(cl_mem), mu);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"mu\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 18, sizeof(cl_mem), velocity_delta);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"velocity_delta\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 19, sizeof(cl_mem), mat_cross_section);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"mat_cross_section\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 20, sizeof(cl_mem), denominator);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"denominator\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 21, sizeof(cl_mem), angular_flux_in);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 22, sizeof(cl_mem), flux_i);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"flux_i\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 23, sizeof(cl_mem), flux_j);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"flux_j\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 24, sizeof(cl_mem), flux_k);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"flux_k\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_sweep_plane_kernel, 25, sizeof(cl_mem), angular_flux_out);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out\", host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->sweep_zero_inner_reducef_sweep_plane_kernel, 3, &_meta_offset[0], _global_size, (nullBlock ? NULL : _local_size), 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
    if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"metacl_sweep_zero_inner_reducef_sweep_plane\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

cl_int metacl_sweep_zero_inner_reducef_reduce_flux(cl_command_queue queue, size_t (*global_size)[3], size_t (*local_size)[3], size_t (*meta_offset)[3], int async, cl_event * event, cl_uint nx, cl_uint ny, cl_uint nz, cl_uint nang, cl_uint ng, cl_mem * angular_flux_in_0, cl_mem * angular_flux_in_1, cl_mem * angular_flux_in_2, cl_mem * angular_flux_in_3, cl_mem * angular_flux_in_4, cl_mem * angular_flux_in_5, cl_mem * angular_flux_in_6, cl_mem * angular_flux_in_7, cl_mem * angular_flux_out_0, cl_mem * angular_flux_out_1, cl_mem * angular_flux_out_2, cl_mem * angular_flux_out_3, cl_mem * angular_flux_out_4, cl_mem * angular_flux_out_5, cl_mem * angular_flux_out_6, cl_mem * angular_flux_out_7, cl_mem * velocity_delta, cl_mem * quad_weights, cl_mem * scalar_flux, size_t local_scalar_num_local_elems) {
  if (metacl_metacl_module_registration == NULL) meta_register_module(&metacl_metacl_module_registry);
  struct __metacl_metacl_module_frame * frame = __metacl_metacl_module_current_frame;
  if (queue != NULL) frame = __metacl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->sweep_zero_inner_reducef_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  a_bool nullBlock = (local_size != NULL && (*local_size)[0] == 0 && (*local_size)[1] == 0 && (*local_size)[2] == 0);
  size_t _global_size[3];
  size_t _local_size[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  size_t _temp_offset[3];
  if (meta_offset == NULL) { _temp_offset[0] = 0, _temp_offset[1] = 0, _temp_offset[2] = 0;}
  else { _temp_offset[0] = (*meta_offset)[0], _temp_offset[1] = (*meta_offset)[1], _temp_offset[2] = (*meta_offset)[2]; }
  const size_t _meta_offset[3] = {_temp_offset[0], _temp_offset[1], _temp_offset[2]};
  int iters;

  //Default runs a single workgroup
  if (global_size == NULL || local_size == NULL) {
    _global_size[0] = _local_size[0];
    _global_size[1] = _local_size[1];
    _global_size[2] = _local_size[2];
    iters = 1;
  } else {
    _global_size[0] = (*global_size)[0];
    _global_size[1] = (*global_size)[1];
    _global_size[2] = (*global_size)[2];
    _local_size[0] = (*local_size)[0];
    _local_size[1] = (*local_size)[1];
    _local_size[2] = (*local_size)[2];
  }
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_reduce_flux_kernel, 0, sizeof(cl_uint), &nx);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nx\", host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_reduce_flux_kernel, 1, sizeof(cl_uint), &ny);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ny\", host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_reduce_flux_kernel, 2, sizeof(cl_uint), &nz);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nz\", host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_reduce_flux_kernel, 3, sizeof(cl_uint), &nang);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nang\", host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_reduce_flux_kernel, 4, sizeof(cl_uint), &ng);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ng\", host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_reduce_flux_kernel, 5, sizeof(cl_mem), angular_flux_in_0);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_0\", host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_reduce_flux_kernel, 6, sizeof(cl_mem), angular_flux_in_1);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_1\", host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_reduce_flux_kernel, 7, sizeof(cl_mem), angular_flux_in_2);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_2\", host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_reduce_flux_kernel, 8, sizeof(cl_mem), angular_flux_in_3);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_3\", host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_reduce_flux_kernel, 9, sizeof(cl_mem), angular_flux_in_4);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_4\", host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_reduce_flux_kernel, 10, sizeof(cl_mem), angular_flux_in_5);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_5\", host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_reduce_flux_kernel, 11, sizeof(cl_mem), angular_flux_in_6);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_6\", host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_reduce_flux_kernel, 12, sizeof(cl_mem), angular_flux_in_7);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_in_7\", host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_reduce_flux_kernel, 13, sizeof(cl_mem), angular_flux_out_0);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_0\", host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_reduce_flux_kernel, 14, sizeof(cl_mem), angular_flux_out_1);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_1\", host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_reduce_flux_kernel, 15, sizeof(cl_mem), angular_flux_out_2);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_2\", host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_reduce_flux_kernel, 16, sizeof(cl_mem), angular_flux_out_3);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_3\", host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_reduce_flux_kernel, 17, sizeof(cl_mem), angular_flux_out_4);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_4\", host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_reduce_flux_kernel, 18, sizeof(cl_mem), angular_flux_out_5);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_5\", host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_reduce_flux_kernel, 19, sizeof(cl_mem), angular_flux_out_6);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_6\", host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_reduce_flux_kernel, 20, sizeof(cl_mem), angular_flux_out_7);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"angular_flux_out_7\", host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_reduce_flux_kernel, 21, sizeof(cl_mem), velocity_delta);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"velocity_delta\", host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_reduce_flux_kernel, 22, sizeof(cl_mem), quad_weights);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"quad_weights\", host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_reduce_flux_kernel, 23, sizeof(cl_mem), scalar_flux);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scalar_flux\", host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_reduce_flux_kernel, 24, sizeof(cl_double) * local_scalar_num_local_elems, NULL);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"local_scalar\", host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->sweep_zero_inner_reducef_reduce_flux_kernel, 3, &_meta_offset[0], _global_size, (nullBlock ? NULL : _local_size), 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
    if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"metacl_sweep_zero_inner_reducef_reduce_flux\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

cl_int metacl_sweep_zero_inner_reducef_calc_inner_source(cl_command_queue queue, size_t (*global_size)[3], size_t (*local_size)[3], size_t (*meta_offset)[3], int async, cl_event * event, cl_uint nx, cl_uint ny, cl_uint nz, cl_uint ng, cl_uint cmom, cl_uint nmom, cl_mem * outer_source, cl_mem * scattering_matrix, cl_mem * scalar_flux, cl_mem * scalar_flux_moments, cl_mem * inner_source) {
  if (metacl_metacl_module_registration == NULL) meta_register_module(&metacl_metacl_module_registry);
  struct __metacl_metacl_module_frame * frame = __metacl_metacl_module_current_frame;
  if (queue != NULL) frame = __metacl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->sweep_zero_inner_reducef_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  a_bool nullBlock = (local_size != NULL && (*local_size)[0] == 0 && (*local_size)[1] == 0 && (*local_size)[2] == 0);
  size_t _global_size[3];
  size_t _local_size[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  size_t _temp_offset[3];
  if (meta_offset == NULL) { _temp_offset[0] = 0, _temp_offset[1] = 0, _temp_offset[2] = 0;}
  else { _temp_offset[0] = (*meta_offset)[0], _temp_offset[1] = (*meta_offset)[1], _temp_offset[2] = (*meta_offset)[2]; }
  const size_t _meta_offset[3] = {_temp_offset[0], _temp_offset[1], _temp_offset[2]};
  int iters;

  //Default runs a single workgroup
  if (global_size == NULL || local_size == NULL) {
    _global_size[0] = _local_size[0];
    _global_size[1] = _local_size[1];
    _global_size[2] = _local_size[2];
    iters = 1;
  } else {
    _global_size[0] = (*global_size)[0];
    _global_size[1] = (*global_size)[1];
    _global_size[2] = (*global_size)[2];
    _local_size[0] = (*local_size)[0];
    _local_size[1] = (*local_size)[1];
    _local_size[2] = (*local_size)[2];
  }
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_calc_inner_source_kernel, 0, sizeof(cl_uint), &nx);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nx\", host wrapper: \"metacl_sweep_zero_inner_reducef_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_calc_inner_source_kernel, 1, sizeof(cl_uint), &ny);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ny\", host wrapper: \"metacl_sweep_zero_inner_reducef_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_calc_inner_source_kernel, 2, sizeof(cl_uint), &nz);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nz\", host wrapper: \"metacl_sweep_zero_inner_reducef_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_calc_inner_source_kernel, 3, sizeof(cl_uint), &ng);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"ng\", host wrapper: \"metacl_sweep_zero_inner_reducef_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_calc_inner_source_kernel, 4, sizeof(cl_uint), &cmom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"cmom\", host wrapper: \"metacl_sweep_zero_inner_reducef_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_calc_inner_source_kernel, 5, sizeof(cl_uint), &nmom);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"nmom\", host wrapper: \"metacl_sweep_zero_inner_reducef_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_calc_inner_source_kernel, 6, sizeof(cl_mem), outer_source);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"outer_source\", host wrapper: \"metacl_sweep_zero_inner_reducef_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_calc_inner_source_kernel, 7, sizeof(cl_mem), scattering_matrix);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scattering_matrix\", host wrapper: \"metacl_sweep_zero_inner_reducef_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_calc_inner_source_kernel, 8, sizeof(cl_mem), scalar_flux);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scalar_flux\", host wrapper: \"metacl_sweep_zero_inner_reducef_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_calc_inner_source_kernel, 9, sizeof(cl_mem), scalar_flux_moments);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"scalar_flux_moments\", host wrapper: \"metacl_sweep_zero_inner_reducef_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_calc_inner_source_kernel, 10, sizeof(cl_mem), inner_source);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"inner_source\", host wrapper: \"metacl_sweep_zero_inner_reducef_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->sweep_zero_inner_reducef_calc_inner_source_kernel, 3, &_meta_offset[0], _global_size, (nullBlock ? NULL : _local_size), 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"metacl_sweep_zero_inner_reducef_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
    if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"metacl_sweep_zero_inner_reducef_calc_inner_source\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

cl_int metacl_sweep_zero_inner_reducef_zero_buffer(cl_command_queue queue, size_t (*global_size)[3], size_t (*local_size)[3], size_t (*meta_offset)[3], int async, cl_event * event, cl_mem * buffer) {
  if (metacl_metacl_module_registration == NULL) meta_register_module(&metacl_metacl_module_registry);
  struct __metacl_metacl_module_frame * frame = __metacl_metacl_module_current_frame;
  if (queue != NULL) frame = __metacl_metacl_module_lookup_frame(queue);
  //If the user requests a queue this module doesn't know about, or a NULL queue and there is no current frame
  if (frame == NULL) return CL_INVALID_COMMAND_QUEUE;
  if (frame->sweep_zero_inner_reducef_init != 1) return CL_INVALID_PROGRAM;
  cl_int retCode = CL_SUCCESS;
  a_bool nullBlock = (local_size != NULL && (*local_size)[0] == 0 && (*local_size)[1] == 0 && (*local_size)[2] == 0);
  size_t _global_size[3];
  size_t _local_size[3] = METAMORPH_OCL_DEFAULT_BLOCK_3D;
  size_t _temp_offset[3];
  if (meta_offset == NULL) { _temp_offset[0] = 0, _temp_offset[1] = 0, _temp_offset[2] = 0;}
  else { _temp_offset[0] = (*meta_offset)[0], _temp_offset[1] = (*meta_offset)[1], _temp_offset[2] = (*meta_offset)[2]; }
  const size_t _meta_offset[3] = {_temp_offset[0], _temp_offset[1], _temp_offset[2]};
  int iters;

  //Default runs a single workgroup
  if (global_size == NULL || local_size == NULL) {
    _global_size[0] = _local_size[0];
    _global_size[1] = _local_size[1];
    _global_size[2] = _local_size[2];
    iters = 1;
  } else {
    _global_size[0] = (*global_size)[0];
    _global_size[1] = (*global_size)[1];
    _global_size[2] = (*global_size)[2];
    _local_size[0] = (*local_size)[0];
    _local_size[1] = (*local_size)[1];
    _local_size[2] = (*local_size)[2];
  }
  retCode = clSetKernelArg(frame->sweep_zero_inner_reducef_zero_buffer_kernel, 0, sizeof(cl_mem), buffer);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel argument assignment error (arg: \"buffer\", host wrapper: \"metacl_sweep_zero_inner_reducef_zero_buffer\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  retCode = clEnqueueNDRangeKernel(frame->queue, frame->sweep_zero_inner_reducef_zero_buffer_kernel, 3, &_meta_offset[0], _global_size, (nullBlock ? NULL : _local_size), 0, NULL, event);
  if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel enqueue error (host wrapper: \"metacl_sweep_zero_inner_reducef_zero_buffer\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  if (!async) {
    retCode = clFinish(frame->queue);
    if (retCode != CL_SUCCESS) fprintf(stderr, "OpenCL kernel execution error (host wrapper: \"metacl_sweep_zero_inner_reducef_zero_buffer\") %d at %s:%d\n", retCode, __FILE__, __LINE__);
  }
  return retCode;
}

