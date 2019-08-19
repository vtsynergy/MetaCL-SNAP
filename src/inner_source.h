const char inner_source_ocl[] ={
"\n"
"#pragma OPENCL EXTENSION cl_khr_fp64 : enable\n"
"\n"
"#define SOURCE_INDEX(m,g,i,j,k,cmom,ng,nx,ny) ((m)+((cmom)*(g))+((cmom)*(ng)*(i))+((cmom)*(ng)*(nx)*(j))+((cmom)*(ng)*(nx)*(ny)*(k)))\n"
"#define SCATTERING_MATRIX_INDEX(m,g1,g2,nmom,ng) ((m)+((nmom)*(g1))+((nmom)*(ng)*(g2)))\n"
"#define SCALAR_FLUX_INDEX(g,i,j,k,ng,nx,ny) ((g)+((ng)*(i))+((ng)*(nx)*(j))+((ng)*(nx)*(ny)*(k)))\n"
"#define SCALAR_FLUX_MOMENTS_INDEX(m,g,i,j,k,cmom,ng,nx,ny) ((m)+((cmom-1)*(g))+((cmom-1)*(ng)*(i))+((cmom-1)*(ng)*(nx)*(j))+((cmom-1)*(ng)*(nx)*(ny)*(k)))\n"
"\n"
"\n"
"#define outer_source(m,g,i,j,k) outer_source[SOURCE_INDEX((m),(g),(i),(j),(k),cmom,ng,nx,ny)]\n"
"#define inner_source(m,g,i,j,k) inner_source[SOURCE_INDEX((m),(g),(i),(j),(k),cmom,ng,nx,ny)]\n"
"#define scattering_matrix(m,g1,g2) scattering_matrix[SCATTERING_MATRIX_INDEX((m),(g1),(g2),nmom,ng)]\n"
"#define scalar_flux(g,i,j,k) scalar_flux[SCALAR_FLUX_INDEX((g),(i),(j),(k),ng,nx,ny)]\n"
"#define scalar_flux_moments(m,g,i,j,k) scalar_flux_moments[SCALAR_FLUX_MOMENTS_INDEX((m),(g),(i),(j),(k),cmom,ng,nx,ny)]\n"
"\n"
"\n"
"// 3D kernel, in local nx,ny,nz dimensions\n"
"// Probably not going to vectorise very well..\n"
"kernel void calc_inner_source(\n"
"    const unsigned int nx,\n"
"    const unsigned int ny,\n"
"    const unsigned int nz,\n"
"    const unsigned int ng,\n"
"    const unsigned int cmom,\n"
"    const unsigned int nmom,\n"
"    global const double * restrict outer_source,\n"
"    global const double * restrict scattering_matrix,\n"
"    global const double * restrict scalar_flux,\n"
"    global const double * restrict scalar_flux_moments,\n"
"    global double * restrict inner_source\n"
"    )\n"
"{\n"
"    const size_t i = get_global_id(0);\n"
"    const size_t j = get_global_id(1);\n"
"    const size_t k = get_global_id(2);\n"
"\n"
"    for (unsigned int g = 0; g < ng; g++)\n"
"    {\n"
"        // Set first moment to outer source plus scattering contribution of scalar flux\n"
"        inner_source(0,g,i,j,k) = outer_source(0,g,i,j,k) + scattering_matrix(0,g,g) * scalar_flux(g,i,j,k);\n"
"\n"
"        // Set other moments similarly based on scalar flux moments\n"
"        unsigned int mom = 1;\n"
"        for (unsigned int l = 1; l < nmom; l++)\n"
"        {\n"
"            for (unsigned int m = 0; m < 2*l+1; m++)\n"
"            {\n"
"                inner_source(mom,g,i,j,k) = outer_source(mom,g,i,j,k) + scattering_matrix(l,g,g) * scalar_flux_moments(mom-1,g,i,j,k);\n"
"                mom += 1;\n"
"            }\n"
"        }\n"
"    }\n"
"\n"
"}\n"
};
