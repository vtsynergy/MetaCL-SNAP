ifndef METACL
METACL := $(shell which metaCL)
define metaCL_help
Set METACL to change binary location, attempting to use MetaCL at "$(METACL)"

endef
$(info $(metaCL_help))
endif
ifndef COMPILER
	MESSAGE=select a compiler to compiler in OpenMP, e.g. make COMPILER=INTEL
endif


ifeq ($(shell uname -s), Darwin)
  OCL = -framework OpenCL -g
else
  OCL = -lOpenCL 
endif

CFLAGS_			= -O3
CFLAGS_INTEL	= -O3 -std=c99
CFLAGS_CRAY		= -O3
CFLAGS_GNU		= -O3 -std=c99 -lm
CFLAGS := $(CFLAGS) $(CFLAGS_$(COMPILER))

CL_OPTS = -I . -cl-mad-enable -cl-fast-relaxed-math
AOC_OPTS := $(AOC_OPTS) -v $(CL_OPTS)
ifeq ($(FPGA), INTEL_EMU)
  AOC_OPTS := $(AOC_OPTS) -march=emulator
  FPGA=INTEL
endif
ifeq ($(FPGA), INTEL)
  DEPS := $(DEPS) outer_zero_and_others.aocx sweep_zero_inner_reducef.aocx
  CFLAGS := $(CFLAGS) $(shell aocl compile-config)
  OCL := $(OCL) $(shell aocl link-config)
endif
ifdef USE_METACL
OCL := $(OCL) -lmetamorph -lmetamorph_opencl
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
	$(MPI_COMPILER) $(SRC) $(CFLAGS) $(OCL) $(OPTIONS) -o $@

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

metacl_module.h: metacl_module.c

METACL_CFLAGS := $(METACL_CFLAGS) $(CL_OPTS)
metacl_module.c: outer_zero_and_others.cl sweep_zero_inner_reducef.cl
	$(METACL) $^ --unified-output-file="metacl_module" -- -cl-std=CL1.2 --include opencl-c.h $(METACL_CFLAGS)

outer_zero_and_others.aocx: outer_zero_and_others.cl
	aoc $(AOC_OPTS) outer_zero_and_others.cl

sweep_zero_inner_reducef.aocx: sweep_zero_inner_reducef.cl
	aoc $(AOC_OPTS) sweep_zero_inner_reducef.cl

.PHONY: clean
clean:
	rm -f snap kernels/*.h metacl_module.*
