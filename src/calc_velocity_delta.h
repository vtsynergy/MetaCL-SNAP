const char calc_velocity_delta_ocl[] ={
"\n"
"#pragma OPENCL EXTENSION cl_khr_fp64 : enable\n"
"\n"
"// Calculate the time absorbtion coefficient\n"
"kernel void calc_velocity_delta(\n"
"    global const double * restrict velocities,\n"
"    const double dt,\n"
"    global double * restrict velocity_delta\n"
"    )\n"
"{\n"
"    size_t g = get_global_id(0);\n"
"    velocity_delta[g] = 2.0 / (dt * velocities[g]);\n"
"\n"
"}\n"
};
