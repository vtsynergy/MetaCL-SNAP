
ifndef COMPILER
	MESSAGE=select a compiler to compiler in OpenMP, e.g. make COMPILER=INTEL
endif

ifeq ($(shell uname), Darwin)
	OCL = -framework OpenCL
else
	OCL = -L/opt/intel/inteloneapi/compiler/2021.1-beta03/linux/lib/oclfpga/host/linux64/lib -lOpenCL -lmm_opencl_backend -lmetamorph 
endif

CFLAGS_			=  -D WITH_OPENCL -I ../../MetaMorph/include -I ../../MetaMorph/metamorph-backends/opencl-backend  -O3
CFLAGS_INTEL	=  -D WITH_OPENCL -I ../../MetaMorph/include -I ../../MetaMorph/metamorph-backends/opencl-backend -O3 -std=c99
CFLAGS_CRAY		=  -D WITH_OPENCL -I ../../MetaMorph/include -I ../../MetaMorph/metamorph-backends/opencl-backend -O3
CFLAGS_GNU		=  -D WITH_OPENCL $(shell aocl compile-config ) -I ../../MetaMorph/include -I ../../MetaMorph/metamorph-backends/opencl-backend -O3 -std=gnu99 -lm

MPI_COMPILER 	= mpicc


snap: \
	snap_main.c \
	input.c \
	allocate.c \
	comms.c \
	problem.c \
	source.c \
	sweep.c \
	ocl_init.c \
	ocl_buffers.c \
	scalar_flux.c \
	convergence.c \
	population.c \
	metacl_module.c\
	profiler.c
	$(MPI_COMPILER) $^ $(CFLAGS_$(COMPILER))    -L ../../MetaMorph/lib  $(OCL) $(OPTIONS) -o $@

kernels/%.h:	kernels/%.cl
	./stringify_opencl $< $@


KERNEL_HEADER_FILES=\
	kernels/calc_velocity_delta.h \
	kernels/calc_dd_coeff.h \
	kernels/calc_denominator.h \
	kernels/zero_buffer.h \
	kernels/outer_source.h \
	kernels/inner_source.h \
	kernels/sweep_plane.h \
	kernels/reduce_flux.h \
	kernels/reduce_flux_moments.h

ocl_init.c:	$(KERNEL_HEADER_FILES)
	@touch ocl_init.c

.PHONY: clean
run: 
	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../../MetaMorph/lib  ./snap $(i) $(device)
clean:
	rm -f snap kernels/*.h
