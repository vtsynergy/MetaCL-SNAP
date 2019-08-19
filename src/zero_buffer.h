const char zero_buffer_ocl[] ={
"\n"
"#pragma OPENCL EXTENSION cl_khr_fp64 : enable\n"
"\n"
"// Just zeros the buffer!\n"
"kernel void zero_buffer(global double *buffer)\n"
"{\n"
"    size_t id = get_global_id(0);\n"
"    buffer[id] = 0.0;\n"
"}\n"
};
