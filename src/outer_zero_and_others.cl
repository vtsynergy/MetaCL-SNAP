#pragma OPENCL EXTENSION cl_khr_fp64 : enable

#define ANGULAR_FLUX_INDEX(a,g,i,j,k,nang,ng,nx,ny) ((a)+((nang)*(g))+((nang)*(ng)*(i))+((nang)*(ng)*(nx)*(j))+((nang)*(ng)*(nx)*(ny)*(k)))
#define SCAT_COEFF_INDEX(a,l,o,nang,cmom) ((a)+((nang)*(l))+((nang)*(cmom)*o))


#define outer_source(m,g,i,j,k) outer_source[SOURCE_INDEX((m),(g),(i),(j),(k),cmom,ng,nx,ny)]
#define fixed_source(g,i,j,k) fixed_source[FIXED_SOURCE_INDEX((g),(i),(j),(k),ng,nx,ny)]
#define scattering_matrix(m,g1,g2) scattering_matrix[SCATTERING_MATRIX_INDEX((m),(g1),(g2),nmom,ng)]
#define scalar_flux(g,i,j,k) scalar_flux[SCALAR_FLUX_INDEX((g),(i),(j),(k),ng,nx,ny)]

#define SOURCE_INDEX(m,g,i,j,k,cmom,ng,nx,ny) ((m)+((cmom)*(g))+((cmom)*(ng)*(i))+((cmom)*(ng)*(nx)*(j))+((cmom)*(ng)*(nx)*(ny)*(k)))
#define FIXED_SOURCE_INDEX(g,i,j,k,ng,nx,ny) ((g)+((ng)*(i))+((ng)*(nx)*(j))+((ng)*(nx)*(ny)*(k)))
#define SCATTERING_MATRIX_INDEX(m,g1,g2,nmom,ng) ((m)+((nmom)*(g1))+((nmom)*(ng)*(g2)))
#define SCALAR_FLUX_INDEX(g,i,j,k,ng,nx,ny) ((g)+((ng)*(i))+((ng)*(nx)*(j))+((ng)*(nx)*(ny)*(k)))
#define SCALAR_FLUX_MOMENTS_INDEX(m,g,i,j,k,cmom,ng,nx,ny) ((m)+((cmom-1)*(g))+((cmom-1)*(ng)*(i))+((cmom-1)*(ng)*(nx)*(j))+((cmom-1)*(ng)*(nx)*(ny)*(k)))


#define angular_flux_in_0(a,g,i,j,k) angular_flux_in_0[ANGULAR_FLUX_INDEX((a),(g),(i),(j),(k),nang,ng,nx,ny)]
#define angular_flux_in_1(a,g,i,j,k) angular_flux_in_1[ANGULAR_FLUX_INDEX((a),(g),(i),(j),(k),nang,ng,nx,ny)]
#define angular_flux_in_2(a,g,i,j,k) angular_flux_in_2[ANGULAR_FLUX_INDEX((a),(g),(i),(j),(k),nang,ng,nx,ny)]
#define angular_flux_in_3(a,g,i,j,k) angular_flux_in_3[ANGULAR_FLUX_INDEX((a),(g),(i),(j),(k),nang,ng,nx,ny)]
#define angular_flux_in_4(a,g,i,j,k) angular_flux_in_4[ANGULAR_FLUX_INDEX((a),(g),(i),(j),(k),nang,ng,nx,ny)]
#define angular_flux_in_5(a,g,i,j,k) angular_flux_in_5[ANGULAR_FLUX_INDEX((a),(g),(i),(j),(k),nang,ng,nx,ny)]
#define angular_flux_in_6(a,g,i,j,k) angular_flux_in_6[ANGULAR_FLUX_INDEX((a),(g),(i),(j),(k),nang,ng,nx,ny)]
#define angular_flux_in_7(a,g,i,j,k) angular_flux_in_7[ANGULAR_FLUX_INDEX((a),(g),(i),(j),(k),nang,ng,nx,ny)]
#define angular_flux_out_0(a,g,i,j,k) angular_flux_out_0[ANGULAR_FLUX_INDEX((a),(g),(i),(j),(k),nang,ng,nx,ny)]
#define angular_flux_out_1(a,g,i,j,k) angular_flux_out_1[ANGULAR_FLUX_INDEX((a),(g),(i),(j),(k),nang,ng,nx,ny)]
#define angular_flux_out_2(a,g,i,j,k) angular_flux_out_2[ANGULAR_FLUX_INDEX((a),(g),(i),(j),(k),nang,ng,nx,ny)]
#define angular_flux_out_3(a,g,i,j,k) angular_flux_out_3[ANGULAR_FLUX_INDEX((a),(g),(i),(j),(k),nang,ng,nx,ny)]
#define angular_flux_out_4(a,g,i,j,k) angular_flux_out_4[ANGULAR_FLUX_INDEX((a),(g),(i),(j),(k),nang,ng,nx,ny)]
#define angular_flux_out_5(a,g,i,j,k) angular_flux_out_5[ANGULAR_FLUX_INDEX((a),(g),(i),(j),(k),nang,ng,nx,ny)]
#define angular_flux_out_6(a,g,i,j,k) angular_flux_out_6[ANGULAR_FLUX_INDEX((a),(g),(i),(j),(k),nang,ng,nx,ny)]
#define angular_flux_out_7(a,g,i,j,k) angular_flux_out_7[ANGULAR_FLUX_INDEX((a),(g),(i),(j),(k),nang,ng,nx,ny)]

#define scalar_flux_moments(l,g,i,j,k) scalar_flux_moments[SCALAR_FLUX_MOMENTS_INDEX((l),(g),(i),(j),(k),cmom,ng,nx,ny)]
#define scat_coeff(a,l,o) scat_coeff[SCAT_COEFF_INDEX((a),(l),(o),nang,cmom)]

#define DENOMINATOR_INDEX(a,g,i,j,k,nang,ng,nx,ny) ((a)+((nang)*(g))+((nang)*(ng)*(i))+((nang)*(ng)*(nx)*(j))+((nang)*(ng)*(nx)*(ny)*(k)))
#define denominator(a,g,i,j,k) denominator[DENOMINATOR_INDEX((a),(g),(i),(j),(k),nang,ng,nx,ny)]
// We want to perform a weighted sum of angles in each cell in each energy group for each moment
// One work-group per cell per energy group, and reduce within a work-group
// Work-groups must be power of two sized
kernel void reduce_flux_moments(
    const unsigned int nx,
    const unsigned int ny,
    const unsigned int nz,
    const unsigned int nang,
    const unsigned int ng,
    const unsigned int cmom,

    global const double * restrict angular_flux_in_0,
    global const double * restrict angular_flux_in_1,
    global const double * restrict angular_flux_in_2,
    global const double * restrict angular_flux_in_3,
    global const double * restrict angular_flux_in_4,
    global const double * restrict angular_flux_in_5,
    global const double * restrict angular_flux_in_6,
    global const double * restrict angular_flux_in_7,

    global const double * restrict angular_flux_out_0,
    global const double * restrict angular_flux_out_1,
    global const double * restrict angular_flux_out_2,
    global const double * restrict angular_flux_out_3,
    global const double * restrict angular_flux_out_4,
    global const double * restrict angular_flux_out_5,
    global const double * restrict angular_flux_out_6,
    global const double * restrict angular_flux_out_7,

    global const double * restrict velocity_delta,
    global const double * restrict quad_weights,
    global const double * restrict scat_coeff,

    global double * restrict scalar_flux_moments,
    local double * restrict local_scalar
    )
{
    const size_t a = get_local_id(0);
    const size_t g = get_group_id(0);

    const size_t i = get_global_id(1) % nx;
    const size_t j = (get_global_id(1) / nx) % ny;
    const size_t k = get_global_id(1) / (nx * ny);

    for (unsigned int l = 0; l < cmom-1; l++)
    {
        // Load into local memory
        local_scalar[a] = 0.0;
        for (unsigned int aa = a; aa < nang; aa += get_local_size(0))
        {
            const double w = quad_weights[aa];
            if (velocity_delta[g] != 0.0)
            {
                local_scalar[a] +=
                    scat_coeff(aa,l+1,0) * w * (0.5 * (angular_flux_out_0(aa,g,i,j,k) + angular_flux_in_0(aa,g,i,j,k))) +
                    scat_coeff(aa,l+1,1) * w * (0.5 * (angular_flux_out_1(aa,g,i,j,k) + angular_flux_in_1(aa,g,i,j,k))) +
                    scat_coeff(aa,l+1,2) * w * (0.5 * (angular_flux_out_2(aa,g,i,j,k) + angular_flux_in_2(aa,g,i,j,k))) +
                    scat_coeff(aa,l+1,3) * w * (0.5 * (angular_flux_out_3(aa,g,i,j,k) + angular_flux_in_3(aa,g,i,j,k))) +
                    scat_coeff(aa,l+1,4) * w * (0.5 * (angular_flux_out_4(aa,g,i,j,k) + angular_flux_in_4(aa,g,i,j,k))) +
                    scat_coeff(aa,l+1,5) * w * (0.5 * (angular_flux_out_5(aa,g,i,j,k) + angular_flux_in_5(aa,g,i,j,k))) +
                    scat_coeff(aa,l+1,6) * w * (0.5 * (angular_flux_out_6(aa,g,i,j,k) + angular_flux_in_6(aa,g,i,j,k))) +
                    scat_coeff(aa,l+1,7) * w * (0.5 * (angular_flux_out_7(aa,g,i,j,k) + angular_flux_in_7(aa,g,i,j,k)));
            }
            else
            {
                local_scalar[a] +=
                    scat_coeff(aa,l+1,0) * w * angular_flux_out_0(aa,g,i,j,k) +
                    scat_coeff(aa,l+1,1) * w * angular_flux_out_1(aa,g,i,j,k) +
                    scat_coeff(aa,l+1,2) * w * angular_flux_out_2(aa,g,i,j,k) +
                    scat_coeff(aa,l+1,3) * w * angular_flux_out_3(aa,g,i,j,k) +
                    scat_coeff(aa,l+1,4) * w * angular_flux_out_4(aa,g,i,j,k) +
                    scat_coeff(aa,l+1,5) * w * angular_flux_out_5(aa,g,i,j,k) +
                    scat_coeff(aa,l+1,6) * w * angular_flux_out_6(aa,g,i,j,k) +
                    scat_coeff(aa,l+1,7) * w * angular_flux_out_7(aa,g,i,j,k);
            }
        }

        barrier(CLK_LOCAL_MEM_FENCE);

        // Reduce in local memory
        for (unsigned int offset = get_local_size(0) / 2; offset > 0; offset /= 2)
        {
            if (a < offset)
            {
                local_scalar[a] += local_scalar[a + offset];
            }
            barrier(CLK_LOCAL_MEM_FENCE);
        }

        // Save result
        if (a == 0)
        {
            scalar_flux_moments(l,g,i,j,k) = local_scalar[0];
        }
    } // End of moment loop
}


kernel void calc_outer_source(
    const unsigned int nx,
    const unsigned int ny,
    const unsigned int nz,
    const unsigned int ng,
    const unsigned int cmom,
    const unsigned int nmom,
    global const double * restrict fixed_source,
    global const double * restrict scattering_matrix,
    global const double * restrict scalar_flux,
    global const double * restrict scalar_flux_moments,
    global double * restrict outer_source
    )
{
    const size_t i = get_global_id(0);
    const size_t j = get_global_id(1);
    const size_t k = get_global_id(2);

    for (unsigned int g = 0; g < ng; g++)
    {
        // Set first moment to the fixed source
        outer_source(0,g,i,j,k) = fixed_source(g,i,j,k);

        // Loop over groups and moments to compute out-of-group scattering
        for (unsigned int g2 = 0; g2 < ng; g2++)
        {
            if (g == g2)
                continue;
            // Compute scattering source
            outer_source(0,g,i,j,k) += scattering_matrix(0,g2,g) * scalar_flux(g2,i,j,k);
            // Other moments
            unsigned int mom = 1;
            for (unsigned int l = 1; l < nmom; l++)
            {
                for (unsigned int m = 0; m < 2*l+1; m++)
                {
                    outer_source(mom,g,i,j,k) += scattering_matrix(l,g2,g) * scalar_flux_moments(mom-1,g2,i,j,k);
                    mom += 1;
                }
            }
        }
    }

}


kernel void calc_dd_coeff(
    const double dx,
    const double dy,
    const double dz,
    global const double * restrict eta,
    global const double * restrict xi,
    global double * restrict dd_i,
    global double * restrict dd_j,
    global double * restrict dd_k
    )
{
    size_t a = get_global_id(0);

    // There is only one dd_i so just get the first work-item to do this
    if (a == 0 && get_group_id(0) == 0)
        dd_i[0] = 2.0 / dx;

    dd_j[a] = (2.0 / dy) * eta[a];
    dd_k[a] = (2.0 / dz) * xi[a];
}

kernel void calc_velocity_delta(
    global const double * restrict velocities,
    const double dt,
    global double * restrict velocity_delta
    )
{
    size_t g = get_global_id(0);
    velocity_delta[g] = 2.0 / (dt * velocities[g]);

}

kernel void calc_denominator(
    const unsigned int nx,
    const unsigned int ny,
    const unsigned int nz,
    const unsigned int nang,
    const unsigned int ng,
    global const double * restrict mat_cross_section,
    global const double * restrict velocity_delta,
    global const double * restrict mu,
    global const double * restrict dd_i,
    global const double * restrict dd_j,
    global const double * restrict dd_k,
    global double * restrict denominator
    )
{
    size_t a = get_global_id(0);
    size_t g = get_global_id(1);

    for (unsigned int k = 0; k < nz; k++)
        for (unsigned int j = 0; j < ny; j++)
            for (unsigned int i = 0; i < nx; i++)
                denominator(a,g,i,j,k) = 1.0 / (mat_cross_section[g] + velocity_delta[g] + mu[a]*dd_i[0] + dd_j[a] + dd_k[a]);
}

// Just zeros the buffer!
kernel void zero_buffer(global double *buffer)
{
    size_t id = get_global_id(0);
    buffer[id] = 0.0;
}
