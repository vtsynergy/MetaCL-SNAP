
ifndef COMPILER
	MESSAGE=select a compiler to compiler in OpenMP, e.g. make COMPILER=INTEL
endif


PLATFORM = $(shell uname -s)
ifeq ($(PLATFORM), Darwin)
  OCL = -framework OpenCL -g
else ifeq ($(PLATFORM), INTEL_FPGA)
  OCL = $(shell aocl link-config)
else
  OCL = -lOpenCL 
endif


MCL = -lmetamorph_opencl -lmetamorph

METAMORPH_PATH=../../MetaMorph


CFLAGS_		=  -D WITH_OPENCL  
CFLAGS_INTEL	=  -D WITH_OPENCL  
CFLAGS_CRAY	=  -D WITH_OPENCL  
CFLAGS_GNU	=  -D WITH_OPENCL  

ifeq ($(PLATFORM), INTEL_FPGA)
 CFLAGS_      +=  $(shell aocl compile-config)
 CFLAGS_INTEL +=  $(shell aocl compile-config)
 CFLAGS_CRAY  +=  $(shell aocl compile-config)
 CFLAGS_GNU   +=  $(shell aocl compile-config)
endif

CFLAGS_		+=   -I $(METAMORPH_PATH)/include -I $(METAMORPH_PATH)/metamorph-backends/opencl-backend -O3 -std=c99 -lm
CFLAGS_INTEL	+=   -I $(METAMORPH_PATH)/include -I $(METAMORPH_PATH)/metamorph-backends/opencl-backend -O3 -std=c99 -lm
CFLAGS_CRAY	+=   -I $(METAMORPH_PATH)/include -I $(METAMORPH_PATH)/metamorph-backends/opencl-backend -O3 -std=c99 -lm
CFLAGS_GNU	+=   -I $(METAMORPH_PATH)/include -I $(METAMORPH_PATH)/metamorph-backends/opencl-backend -O3 -std=c99 -lm


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
	$(MPI_COMPILER) $^ $(CFLAGS_$(COMPILER)) -L $(METAMORPH_PATH)/lib  $(OCL) $(MCL) $(OPTIONS) -o $@

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
	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(METAMORPH_PATH)/lib  ./snap $(i) $(device)
clean:
	rm -f snap kernels/*.h

