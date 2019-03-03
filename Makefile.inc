# relevant for building foMPI, libtopodisc and self written tests
CC=cc
FC=ftn
CXX=CC

# relevant for building foMPI and self written tests
# -DNOTIFIED_ACCESS_SUPPORT enables the notified access module
# -DSHARED_MEMORY_SUPPORT enables the shared memory modul for on-node processes
# -DNDEBUG disables some checks and log messages
FOMPIOPTS=  -DNDEBUG -DNOTIFIED_ACCESS_SUPPORT -DSHARED_MEMORY_SUPPORT  #-DPAPI #-DSYMHEAP


# relevant for building foMPI and libtopodisc
CCFLAGS=-O3
FCFLAGS=-O3
CXXFLAGS=-O3

LDFLAGS=-ldmapp

# relevant for self written tests
LDFLAGS_TESTS=../../libfompi.a -L../../mpitypes/install/lib -ldmapp -lmpitypes # -L../../libtopodisc -ltopodisc

# clear out all suffixes
.SUFFIXES:
# list only those we use
.SUFFIXES: .o .c .f90 .cpp

# some suffix rules
.c.o:
	$(CC) $(CCFLAGS) -c $<

.f90.o:
	$(FC) $(FCFLAGS) -c $<

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<
