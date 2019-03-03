foMPI implements the one-sided chapter of MPI-3.0 standard. It offers a
C and a Fortran interface.

This library was written by Robert Gerstenberger, Roberto Belli, Maciej Besta and
Torsten Hoefler and is copyrighted by the ETH-Zurich (c) 2014. 
See LICENSE for details on the license.

Building
========

Compiler
--------
Currently the implementation uses inline assembly for intra-node atomics
and SSE for a fast intra-node memory copy, which isn't supported by the
Cray compiler. We used primarily gcc, but also tested the Intel compiler
and other compiler might work as well.

The Fortran interface can be accessed in the Fortran 77 way (include
'fompif.h') or the Fortran 90 way (use fompi). Since the structure of a
module file depends on the compiler and you wish to build foMPI with a
different compiler than your application, please use the following
scheme:
1) load the compiler with whom you intend to compile foMPI
2) make libfompi.a
3) load the compiler with whom you intend to compile your application
4) rm -f fompi_fortran.o module_fompi.o fompi.{compiler-specific file 
   extension for Fortran module files, .mod for gcc}
5) make libfompi.a
At the moment the name mangling of the Fortran wrapper is hard-coded
(single underscore). If you need to change this for your compiler,
please have a look at line 6 in fompi_fortran.c .

Dependencies
------------
The foMPI tar archive comes with two additional libaries: MPITypes and
libtopodisc, which are included. Just type "make" and those libraries and
the foMPI object files will be built, and packed together in the
ar-archive named libfompi.a. Also a little test program will be built.
If you wish to built the libraries with a different compiler, please use
"make mpitypes" or "make libtopodisc".  

foMPI also needs the m4 macro processor.

* m4 is available from http://www.gnu.org/software/m4/ .
* MPITypes: For more information see the homepage 
  (http://www.mcs.anl.gov/mpitypes/) or their paper from the EuroMPI
  2009 conference (R. Ross, "MPITypes: Processing MPI Datatypes Outside
  MPI").
* Libtopodisc was provided by William Gropp
  (http://www.cs.illinois.edu/~wgropp/ ).

Includes/Linking
----------------
Additional includes to build your application with foMPI are:
-I{PATH to foMPI}

Additional parameter for linking your application with foMPI are:
-L{PATH to foMPI} -lfompi
-ldmapp

Build parameter
---------------
The building parameter can be found in the Makefile.inc file.
You can choose your appropriate compiler (CC), flags (CCFLAGS), etc.
Additionally foMPI provides a section in the file fompi_internal.h,
that let you influence some of the inner work of foMPI:
* DXPMEM (line 11): Enables/disables XPMEM support for intra-node
  communication
* DTYPE_HANDLING (line 14): Selects the strategy to handle MPI derived
  datatypes for putting data with inter-node targets. A more detailed
  explanation can be found in the section about MPI derived datatypes.

If you experience problems with foMPI, please try removing the 
XPMEM support.


Using foMPI
===========

All MPI one-sided functions are prefixed with foMPI instead of MPI.
Additional all constants like foMPI_LOCK_{SHARED,EXLCUSIVE}, the error
codes, asserts (though not supported at the moment), flavor, memory
model and MPI_Ops for the accumulate-derived communication functions
need to be prefixed with foMPI. Below you find a complete list of
all implemented constants.

The datatypes MPI_Win and MPI_Request (if used with foMPI RMA functions
that take requests) need to be renamed to foMPI_Win or foMPI_Request.
foMPI_Requests are not compatible with MPI_Requests, and can only be
used with foMPI wait and test functions.

Some functions needed to be overloaded to have foMPI work properly
(MPI_Init, MPI_Finalize, MPI_Type_free). The wait and test functions for
foMPI requests can't be mixed with normal MPI requests. For a full list
of overloaded functions, please check the list below.

Apart from prefixing elements of the MPI one-sided scope the function
interfaces are declared as in the standard. See fompi.h for the full
function prototypes.

Below you can find a complete list of implemented and not implemented
functions of the one-sided chapter.

foMPI implements the unified memory model.

MPI Datatypes
-------------

foMPI supports all datatypes that are supported by MPITypes for
foMPI_Put/foMPI_Get. The following datatypes are in the fast path for
these two communication functions:
* MPI_CHAR
* MPI_UINT64_T
* MPI_DOUBLE
* MPI_INT

All MPI datatypes are supported for the accumulate-based communication
functions except:
* handles from MPI_TYPE_CREATE_F90_INTEGER
* MPI_INTEGER16
* handles from MPI_TYPE_CREATE_F90_REAL
* MPI_REAL2
* MPI_CXX_BOOL
* handles from MPI_TYPE_CREATE_F90_COMPLEX
* MPI_CXX_FLOAT_COMPLEX
* MPI_CXX_DOUBLE_COMPLEX
* MPI_CXX_LONG_DOUBLE_COMPLEX

The following combinations of MPI datatypes and MPI_Ops are
hardware-supported:
* MPI_DOUBLE/MPI_{BAND/BOR/BXOR}
* MPI_INT64_T/MPI_{BAND/BOR/BXOR}
* MPI_INT64_T/MPI_SUM

foMPI supports two different modes to handle MPI derived datatypes for
putting data with inter-node targets:
* The maximal block strategy puts each block of the superposition of the
  two datatypes separately. This was the original bufferless approach
  presented in the foMPI SC'13 paper and was the default strategy until
  version 0.2.2.
* The fixed buffer size strategy copies elements into a temporary buffer
  until a predefined size (= fixed size) is reached, and then the
  temporary buffer is put. This strategy allows the overlapping of
  copying and communication. This strategy is the current default
  (beginning with version 0.2.2).
The strategy can be selected in fompi_internal.h, line 14 with the
DTYPE_HANDLING constant. If the fixed buffer size strategy is selected,
additional parameter are active:
* DTYPE_FIXED_SIZE = size of one buffer = maximum size of a underlying
  DMAPP put operation
* DTYPE_BUFFER_NUMBER = number of buffer that can be used in parallel
The maximal block strategy is always used for intra-node communication.

For a detailed theoretical analysis of both strategies see:
----------------------------------------------------------------------
T. Schneider, R. Gerstenberger, T. Hoefler: Compiler Optimizations for
Non-Contiguous Remote Data Movement. In: Proceedings of 26th
International Workshop on Languages and Compilers for Parallel
Computing (LCPC'13)
----------------------------------------------------------------------

For details on the MPI Datatypes implementation in foMPI:
----------------------------------------------------------------------
R. Gerstenberger, T. Hoefler: Handling Datatypes in MPI-3 One Sided.
ACM Students Research Competition Poster at the The International
Conference for High Performance Computing, Networking, Storage and
Analysis (SC’13).
http://sc13.supercomputing.org/sites/default/files/PostersArchive/spost114.html
----------------------------------------------------------------------

Fortran
-------
All communication functions take a displacement parameter, which is of
the integer(kind=MPI_ADDRESS_KIND) type. To ensure compatibility with the
C wrapper code, please use a variable for the displacement parameter
instead of using a constant (which will be transfered as 4 byte integer
instead of the bigger integer(kind=MPI_ADDRESS_KIND)).

On our test system, we experienced problems with MPI_Get_address, which
gives different results for the same location if called from C and
Fortran. Since our library is implemented in C, this is an issue if you
intend to use dynamic windows.
We included a test_address subdirectory, which contains a small test
program for this problem. If all three locations are the same, you are
fine. If not, please use foMPI_Get_address instead.


Questions
=========

If you have additional questions, feel free to contact me under the
following e-mail address: Robert Gerstenberger <gerro@illinois.edu>.


Citation
========

Any published work which uses this software should include the 
following citation:

----------------------------------------------------------------------
R. Belli T. Hoefler: Notified Access: Extending Remote Memory Access Programming Models
for Producer-Consumer Synchronization. In: Proceedings
of the 29th IEEE International Parallel & Distributed Processing Symposium
(IPDPS'15)
----------------------------------------------------------------------

----------------------------------------------------------------------
R. Gerstenberger, M. Besta and T. Hoefler: Enabling Highly-Scalable
Remote Memory Access Programming with MPI-3 One Sided. In: Proceedings
of the International Conference on High Performance Computing,
Networking, Storage and Analysis (SC'13)
----------------------------------------------------------------------


Implemented function
====================

Window creation
---------------
* foMPI_Win_create
* foMPI_Win_allocate
* foMPI_Win_create_dynamic
* foMPI_Win_free

* foMPI_Win_attach
* foMPI_Win_detach


Synchronization
---------------
* foMPI_Win_fence

* foMPI_Win_post
* foMPI_Win_start
* foMPI_Win_complete
* foMPI_Win_wait
* foMPI_Win_test

* foMPI_Win_lock
* foMPI_Win_unlock
* foMPI_Win_lock_all
* foMPI_Win_unlock_all

* foMPI_Win_flush
* foMPI_Win_flush_all
* foMPI_Win_flush_local
* foMPI_Win_flush_local_all
* foMPI_Win_sync

* foMPI_Notify_init


Communication
-------------
* foMPI_Put
* foMPI_Get
* foMPI_Accumulate
* foMPI_Get_accumulate
* foMPI_Fetch_and_op
* foMPI_Compare_and_swap

* foMPI_Rput
* foMPI_Rget
* foMPI_Raccumulate
* foMPI_Rget_accumulate

* foMPI_Put_notify
* foMPI_Get_notify

Miscellaneous
-------------
* foMPI_Win_get_group

* foMPI_Win_set_name
* foMPI_Win_get_name


Overloaded functions
====================

Additional to MPI one-sided functions also the following functions need
to be overloaded:

* foMPI_Type_free
* foMPI_Init
* foMPI_Finalize

All of them above will call their MPI equivalent with the passed
parameters, but do additional work for the foMPI implementation.

* foMPI_Wait
* foMPI_Test
* foMPI_Waitall
* foMPI_Waitany
* foMPI_Start
* foMPI_Request_free

These functions can only be used together with foMPI_Requests and can't
handle normal MPI_Request. All other wait oder test routines are not
implemented at the moment.


Not implemented functions
=========================

* foMPI_Win_allocate_shared
* foMPI_Win_shared_query
* foMPI_Win_get_attr
* foMPI_Win_set_attr
* foMPI_Win_delete_attr
* foMPI_Win_set_errhandler
* foMPI_Win_call_errhandler
* foMPI_Win_set_info
* foMPI_Win_get_info


Constants
=========

Create flavor
-------------
* foMPI_WIN_FLAVOR_CREATE
* foMPI_WIN_FLAVOR_ALLOCATE
* foMPI_WIN_FLAVOR_DYNAMIC


Memory model
------------
* foMPI_WIN_SEPARATE
* foMPI_WIN_UNIFIED


Asserts
-------
* foMPI_MODE_NOCHECK
* foMPI_MODE_NOSTORE
* foMPI_MODE_NOPUT
* foMPI_MODE_NOPRECEDE
* foMPI_MODE_NOSUCCEED
Asserts are not used at the moment, but will be accepted by the library.


Accumulate operations
---------------------
* foMPI_SUM
* foMPI_PROD
* foMPI_MAX
* foMPI_MIN
* foMPI_LAND
* foMPI_LOR
* foMPI_LXOR
* foMPI_BAND
* foMPI_BOR
* foMPI_BXOR
* foMPI_MAXLOC
* foMPI_MINLOC
* foMPI_REPLACE
* foMPI_NO_OP


Lock type
---------
* foMPI_LOCK_SHARED
* foMPI_LOCK_EXCLUSIVE


Error codes
-----------
* foMPI_ERROR_RMA_NO_SYNC
* foMPI_ERROR_RMA_DATATYPE_MISMATCH
* foMPI_ERROR_RMA_NO_LOCK_HOLDING
* foMPI_ERROR_RMA_SYNC_CONFLICT
* foMPI_ERROR_RMA_WIN_MEM_NOT_FOUND
* foMPI_OP_NOT_SUPPORTED
* foMPI_DATATYPE_NOT_SUPPORTED
* foMPI_NAME_ABBREVIATED
