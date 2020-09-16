
ifndef COMPILER
	MESSAGE=select a compiler to compiler in OpenMP, e.g. make COMPILER=INTEL
endif


PLATFORM = $(shell uname -s)
ifeq ($(PLATFORM), Darwin)
  OCL = -framework OpenCL -g
else
  OCL = -lOpenCL 
endif


MCL = -lmetamorph_opencl -lmetamorph

METAMORPH_PATH=../../MetaMorph


CFLAGS_			= -O3
CFLAGS_INTEL	= -O3 -std=c99
CFLAGS_CRAY		= -O3
CFLAGS_GNU		= -O3 -std=c99 -lm
CFLAGS := $(CFLAGS) $(CFLAGS_$(COMPILER))
CFLAGS := $(CFLAGS) -D WITH_OPENCL -I $(METAMORPH_PATH)/include -I $(METAMORPH_PATH)/metamorph-backends/opencl-backend

AOC_OPTS := $(AOC_OPTS) -v -cl-mad-enable -cl-fast-relaxed-math
ifeq ($(FPGA), INTEL_EMU)
  AOC_OPTS := $(AOC_OPTS) -march=emulator
  FPGA=INTEL
endif
ifeq ($(FPGA), INTEL)
  CFLAGS := $(CFLAGS) $(shell aocl compile-config)
  OCL := $(OCL) $(shell aocl link-config)
endif

MPI_COMPILER 	= mpicc


SRC =	snap_main.c \
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

snap: $(SRC) $(DEPS)
	$(MPI_COMPILER) $(SRC) $(CFLAGS) -L $(METAMORPH_PATH)/lib $(OCL) $(MCL) $(OPTIONS) -o $@

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

outer_zero_and_others.aocx: outer_zero_and_others.cl
	aoc $(AOC_OPTS) outer_zero_and_others.cl

sweep_zero_inner_reducef.aocx: sweep_zero_inner_reducef.cl
	aoc $(AOC_OPTS) sweep_zero_inner_reducef.cl

.PHONY: clean
clean:
	rm -f snap kernels/*.h
