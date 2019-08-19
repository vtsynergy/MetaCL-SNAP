const char calc_denominator_ocl[] ={
"\n"
"#pragma OPENCL EXTENSION cl_khr_fp64 : enable\n"
"\n"
"#define DENOMINATOR_INDEX(a,g,i,j,k,nang,ng,nx,ny) ((a)+((nang)*(g))+((nang)*(ng)*(i))+((nang)*(ng)*(nx)*(j))+((nang)*(ng)*(nx)*(ny)*(k)))\n"
"#define denominator(a,g,i,j,k) denominator[DENOMINATOR_INDEX((a),(g),(i),(j),(k),nang,ng,nx,ny)]\n"
"\n"
"kernel void calc_denominator(\n"
"    const unsigned int nx,\n"
"    const unsigned int ny,\n"
"    const unsigned int nz,\n"
"    const unsigned int nang,\n"
"    const unsigned int ng,\n"
"    global const double * restrict mat_cross_section,\n"
"    global const double * restrict velocity_delta,\n"
"    global const double * restrict mu,\n"
"    global const double * restrict dd_i,\n"
"    global const double * restrict dd_j,\n"
"    global const double * restrict dd_k,\n"
"    global double * restrict denominator\n"
"    )\n"
"{\n"
"    size_t a = get_global_id(0);\n"
"    size_t g = get_global_id(1);\n"
"\n"
"    for (unsigned int k = 0; k < nz; k++)\n"
"        for (unsigned int j = 0; j < ny; j++)\n"
"            for (unsigned int i = 0; i < nx; i++)\n"
"                denominator(a,g,i,j,k) = 1.0 / (mat_cross_section[g] + velocity_delta[g] + mu[a]*dd_i[0] + dd_j[a] + dd_k[a]);\n"
"}\n"
};
