SNAP: SN (Discrete Ordinates) Application Proxy
===============================================

Description
-----------
This description was taken from https://github.com/UoB-HPC/SNAP-OpenCL
SNAP serves as a proxy application to model the performance of a modern discrete ordinates neutral particle transport application. SNAP may be considered an update to [Sweep3D](http://www.ccs3.lanl.gov/PAL/software.shtml), intended for hybrid computing architectures. It is modeled off the Los Alamos National Laboratory code PARTISN. PARTISN solves the linear Boltzmann transport equation (TE), a governing equation for determining the number of neutral particles (e.g., neutrons and gamma rays) in a multi-dimensional phase space. SNAP itself is not a particle transport application; SNAP incorporates no actual physics in its available data, nor does it use numerical operators specifically designed for particle transport. Rather, SNAP mimics the computational workload, memory requirements, and communication patterns of PARTISN. The equation it solves has been composed to use the same number of operations, use the same data layout, and load elements of the arrays in approximately the same order. Although the equation SNAP solves looks similar to the TE, it has no real world relevance.

The solution to the time-dependent TE is a "flux" function of seven independent variables: three spatial (3-D spatial mesh), two angular (set of discrete ordinates, directions in which particles travel), one energy (particle speeds binned into "groups"), and one temporal. PARTISN, and therefore SNAP, uses domain decomposition over these dimensions to coherently distribute the data and the tasks associated with solving the equation. The parallelization strategy is expected to be the most efficient compromise between computing resources and the iterative strategy necessary to converge the flux.

The iterative strategy is comprised of a set of two nested loops. These nested loops are performed for each step of a time-dependent calculation, wherein any particular time step requires information from the preceding one. No parallelization is performed over the temporal domain. However, for time-dependent calculations two copies of the unknown flux must be stored, each copy an array of the six remaining dimensions. The outer iterative loop involves solving for the flux over the energy domain with updated information about coupling among the energy groups. Typical calculations require tens to hundreds of groups, making the energy domain suitable for threading with the node's (or nodes') provided accelerator. The inner loop involves sweeping across the entire spatial mesh along each discrete direction of the angular domain. The spatial mesh may be immensely large. Therefore, SNAP spatially decomposes the problem across nodes and communicates needed information according to the KBA method. KBA is a transport-specific application of general parallel wavefront methods. Lastly, although KBA efficiency is improved by pipelining operations according to the angle, current chipsets operate best with vectorized operations. During a mesh sweep, SNAP operations are vectorized over angles to take advantage of the modern hardware.

SNAP should be tested with problem sizes that accurately reflect the types of calculations PARTISN frequently handles. The spatial domain shall be decomposed to 2,000--4,000 cells per node (MPI rank). Each node will own all the energy groups and angles for that group of cells; typical calculations feature 10--100 energy groups and as few as 100 to as many as 2,000 angles. Moreover, sufficient memory must be provided to store two full copies of the solution vector for time-dependent calculations. The preceding parameters assume current trends in available per core memory. Significant advances or detriments affecting this assumption shall require reconsideration of appropriate parameters per compute node.

Compilation
-----------

To compile SNAP simply type

    make COMPILER=GNU

on the command line within the SNAP directory.

Usage
-----
 To run SNAP type

    make run i=<input_file> d=<device_number>
 
 on the command line within the SNAP directory.
     
          
MetaMorph

-----

MetaMorph (https://github.com/vtsynergy/MetaMorph) is a framework designed to effectively utilize HPC systems that consist of multiple heterogeneous nodes with different hardware accelerators. It acts as middleware between the application code and compute devices, such as CPUs, GPUs, Intel MIC and FPGAs. MetaMorph hides the complexity of developing code for and executing on heterogeneous platform by acting as a unified “meta-platform.” Metamorph is developed by the Synergy Laboratory @ Virginia Tech (http://synergy.cs.vt.edu/)


MetaCL

-----

MetaCL (https://github.com/vtsynergy/MetaMorph/tree/master/metamorph-generators/opencl) is a tool that takes OpenCL kernels as input and generates host-side wrappers. These wrappers effectively make OpenCL host-side code developement efficient. MetaCL makes use of Metamorph's OpenCL APIs and backend for generating host-side wrappers. 

Metacl Tutorial: https://github.com/vtsynergy/MetaMorph/tree/c661366a13dec9eb7b9876c91695da48e96d8ba8/metamorph-generators/opencl/docs/tutorials


Publications

-----
MetaCL: Automated "Meta" OpenCL Code Generation for High-Level Sythesis on FPGA." Paul Sathre, Atharva Gondhalekar, Mohamed Hassan, Wu-chun Feng. In Proceedings of the IEEE High Performance Extreme Computing Virtual Conference (HPEC), Boston, MA, USA, September 2020
