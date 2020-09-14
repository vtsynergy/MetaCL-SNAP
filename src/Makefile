
ifndef COMPILER
	MESSAGE=select a compiler to compiler in OpenMP, e.g. make COMPILER=INTEL
endif

ifeq ($(shell uname), Darwin)
	OCL = -framework OpenCL
else
	OCL = $(shell aocl link-config) -lOpenCL
endif

CFLAGS_			= -O3
CFLAGS_INTEL	= -O3 -std=c99
CFLAGS_CRAY		= -O3
CFLAGS_GNU		= -O3 -std=c99 -lm  $(shell aocl compile-config )

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
	profiler.c
	$(MPI_COMPILER) $^ $(CFLAGS_$(COMPILER)) $(OCL) $(OPTIONS) -o $@

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

.PHONY:		 gen_aocx_hw

gen_aocx_hw:
	aoc -v -cl-mad-enable -cl-fast-relaxed-math sweep_zero_inner_reducef.cl 
	aoc -v -cl-mad-enable -cl-fast-relaxed-math outer_zero_and_others.cl

.PHONY:		 gen_aocx_emu

gen_aocx_emu:
	aoc -v -march=emulator -cl-mad-enable -cl-fast-relaxed-math sweep_zero_inner_reducef.cl
	aoc -v -march=emulator -cl-mad-enable -cl-fast-relaxed-math outer_zero_and_others.cl                            
                            
                            

.PHONY: clean

clean:
	rm -f snap kernels/*.h
