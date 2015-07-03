
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "input.h"
#include "problem.h"
#include "allocate.h"

void check_mpi(const int err, const char *msg)
{
    if (err != MPI_SUCCESS)
    {
        fprintf(stderr, "MPI Error: %d. %s\n", err, msg);
        exit(err);
    }
}


int main(int argc, char **argv)
{
    int mpi_err = MPI_Init(&argc, &argv);
    check_mpi(mpi_err, "MPI_Init");

    int rank, size;
    mpi_err = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    check_mpi(mpi_err, "Getting MPI rank");

    mpi_err = MPI_Comm_size(MPI_COMM_WORLD, &size);
    check_mpi(mpi_err, "Getting MPI size");

    struct problem globals;

    if (rank == 0)
    {
        // Check for two files on CLI
        if (argc != 3)
        {
            fprintf(stderr, "Usage: ./snap snap.in snap.out\n");
            exit(-1);
        }
        read_input(argv[1], &globals);
        if ((globals.npex * globals.npey * globals.npez) != size)
        {
            fprintf(stderr, "Input error: wanted %d ranks but executing with %d\n", globals.npex*globals.npey*globals.npez, size);
            exit(-1);
        }
        check_decomposition(&globals);

    }
    // Broadcast the global variables
    broadcast_problem(&globals, rank);

    // Create the MPI Cartesian topology
    MPI_Comm snap_comms;
    int dims[] = {globals.npex, globals.npey, globals.npez};
    int periods[] = {0, 0, 0};
    mpi_err = MPI_Cart_create(MPI_COMM_WORLD, 3, dims, periods, 0, &snap_comms);
    check_mpi(mpi_err, "Creating MPI Cart");

    // Get my ranks in x, y and z
    struct rankinfo local;
    mpi_err = MPI_Cart_coords(snap_comms, rank, 3, local.ranks);
    check_mpi(mpi_err, "Getting Cart co-ordinates");

    // Note: The following assumes one tile per MPI rank
    // TODO: Change to allow for tiling

    // Calculate local sizes
    local.nx = globals.nx / globals.npex;
    local.ny = globals.ny / globals.npey;
    local.nz = globals.nz / globals.npez;

    // Calculate i,j,k lower and upper bounds in terms of global grid
    local.ilb = local.ranks[0]*local.nx;
    local.iub = (local.ranks[0]+1)*local.nx;
    local.jlb = local.ranks[1]*local.ny;
    local.jub = (local.ranks[1]+1)*local.ny;
    local.klb = local.ranks[2]*local.nz;
    local.kub = (local.ranks[2]+1)*local.nz;


    // Allocate the problem arrays
    struct mem memory;
    allocate_memory(globals, local, &memory);

    // Set up problem

    // Halo exchange routines



    free_memory(&memory);

    mpi_err = MPI_Finalize();
    check_mpi(mpi_err, "MPI_Finalize");
}
