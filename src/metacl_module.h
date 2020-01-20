
extern const char * __meta_gen_opencl_outer_zero_and_others_custom_args;
extern const char * __meta_gen_opencl_sweep_zero_inner_reducef_custom_args;
struct __meta_gen_opencl_metacl_module_frame;
struct __meta_gen_opencl_metacl_module_frame {
  struct __meta_gen_opencl_metacl_module_frame * next_frame;
  cl_device_id device;
  cl_context context;
  cl_command_queue queue;
  const char * outer_zero_and_others_progSrc;
  size_t outer_zero_and_others_progLen;
  cl_program outer_zero_and_others_prog;
  cl_int outer_zero_and_others_init;
  cl_kernel outer_zero_and_others_reduce_flux_moments_kernel;
  cl_kernel outer_zero_and_others_calc_outer_source_kernel;
  cl_kernel outer_zero_and_others_calc_dd_coeff_kernel;
  cl_kernel outer_zero_and_others_calc_velocity_delta_kernel;
  cl_kernel outer_zero_and_others_calc_denominator_kernel;
  cl_kernel outer_zero_and_others_zero_buffer_kernel;
  const char * sweep_zero_inner_reducef_progSrc;
  size_t sweep_zero_inner_reducef_progLen;
  cl_program sweep_zero_inner_reducef_prog;
  cl_int sweep_zero_inner_reducef_init;
  cl_kernel sweep_zero_inner_reducef_sweep_plane_kernel;
  cl_kernel sweep_zero_inner_reducef_reduce_flux_kernel;
  cl_kernel sweep_zero_inner_reducef_calc_inner_source_kernel;
  cl_kernel sweep_zero_inner_reducef_zero_buffer_kernel;
};
#ifdef __cplusplus
extern "C" {
#endif
a_module_record * meta_gen_opencl_metacl_module_registry(a_module_record * record);
void meta_gen_opencl_metacl_module_init();
void meta_gen_opencl_metacl_module_deinit();
cl_int meta_gen_opencl_outer_zero_and_others_reduce_flux_moments(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], size_t (*meta_offset)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint nang, cl_uint ng, cl_uint cmom, cl_mem * angular_flux_in_0, cl_mem * angular_flux_in_1, cl_mem * angular_flux_in_2, cl_mem * angular_flux_in_3, cl_mem * angular_flux_in_4, cl_mem * angular_flux_in_5, cl_mem * angular_flux_in_6, cl_mem * angular_flux_in_7, cl_mem * angular_flux_out_0, cl_mem * angular_flux_out_1, cl_mem * angular_flux_out_2, cl_mem * angular_flux_out_3, cl_mem * angular_flux_out_4, cl_mem * angular_flux_out_5, cl_mem * angular_flux_out_6, cl_mem * angular_flux_out_7, cl_mem * velocity_delta, cl_mem * quad_weights, cl_mem * scat_coeff, cl_mem * scalar_flux_moments, size_t local_scalar_num_local_elems, int async, cl_event * event);
cl_int meta_gen_opencl_outer_zero_and_others_calc_outer_source(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], size_t (*meta_offset)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint ng, cl_uint cmom, cl_uint nmom, cl_mem * fixed_source, cl_mem * scattering_matrix, cl_mem * scalar_flux, cl_mem * scalar_flux_moments, cl_mem * outer_source, int async, cl_event * event);
cl_int meta_gen_opencl_outer_zero_and_others_calc_dd_coeff(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], size_t (*meta_offset)[3], cl_double dx, cl_double dy, cl_double dz, cl_mem * eta, cl_mem * xi, cl_mem * dd_i, cl_mem * dd_j, cl_mem * dd_k, int async, cl_event * event);
cl_int meta_gen_opencl_outer_zero_and_others_calc_velocity_delta(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], size_t (*meta_offset)[3], cl_mem * velocities, cl_double dt, cl_mem * velocity_delta, int async, cl_event * event);
cl_int meta_gen_opencl_outer_zero_and_others_calc_denominator(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], size_t (*meta_offset)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint nang, cl_uint ng, cl_mem * mat_cross_section, cl_mem * velocity_delta, cl_mem * mu, cl_mem * dd_i, cl_mem * dd_j, cl_mem * dd_k, cl_mem * denominator, int async, cl_event * event);
cl_int meta_gen_opencl_outer_zero_and_others_zero_buffer(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], size_t (*meta_offset)[3], cl_mem * buffer, int async, cl_event * event);
cl_int meta_gen_opencl_sweep_zero_inner_reducef_sweep_plane(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], size_t (*meta_offset)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint nang, cl_uint ng, cl_uint cmom, cl_int istep, cl_int jstep, cl_int kstep, cl_uint oct, cl_uint z_pos, cl_mem * plane, cl_mem * source, cl_mem * scat_coeff, cl_mem * dd_i, cl_mem * dd_j, cl_mem * dd_k, cl_mem * mu, cl_mem * velocity_delta, cl_mem * mat_cross_section, cl_mem * denominator, cl_mem * angular_flux_in, cl_mem * flux_i, cl_mem * flux_j, cl_mem * flux_k, cl_mem * angular_flux_out, int async, cl_event * event);
cl_int meta_gen_opencl_sweep_zero_inner_reducef_reduce_flux(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], size_t (*meta_offset)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint nang, cl_uint ng, cl_mem * angular_flux_in_0, cl_mem * angular_flux_in_1, cl_mem * angular_flux_in_2, cl_mem * angular_flux_in_3, cl_mem * angular_flux_in_4, cl_mem * angular_flux_in_5, cl_mem * angular_flux_in_6, cl_mem * angular_flux_in_7, cl_mem * angular_flux_out_0, cl_mem * angular_flux_out_1, cl_mem * angular_flux_out_2, cl_mem * angular_flux_out_3, cl_mem * angular_flux_out_4, cl_mem * angular_flux_out_5, cl_mem * angular_flux_out_6, cl_mem * angular_flux_out_7, cl_mem * velocity_delta, cl_mem * quad_weights, cl_mem * scalar_flux, size_t local_scalar_num_local_elems, int async, cl_event * event);
cl_int meta_gen_opencl_sweep_zero_inner_reducef_calc_inner_source(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], size_t (*meta_offset)[3], cl_uint nx, cl_uint ny, cl_uint nz, cl_uint ng, cl_uint cmom, cl_uint nmom, cl_mem * outer_source, cl_mem * scattering_matrix, cl_mem * scalar_flux, cl_mem * scalar_flux_moments, cl_mem * inner_source, int async, cl_event * event);
cl_int meta_gen_opencl_sweep_zero_inner_reducef_zero_buffer(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], size_t (*meta_offset)[3], cl_mem * buffer, int async, cl_event * event);
#ifdef __cplusplus
}
#endif
