#include "config.h" 

#ifdef HAVE_PAPI
#include <papi.h> /* This needs to be included every time you use PAPI */
#endif

#ifdef HAVE_UNWIND
#define UNW_LOCAL_ONLY
#include <libunwind.h>
#endif


#include <stdlib.h>
#include <vector>
#include <signal.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <stdarg.h>

#ifdef HAVE_LIKWID
extern "C" {
#include <bstrlib.h>
#include <cpuid.h>
#include <msr.h>
#include <numa.h>
#include <perfmon.h>
#include <timer.h>
#include <registers.h>
#include <hashTable.h>
#include <likwid.h>
}
#endif


/**
 * a single record field to be inserted during each record call
 */
typedef struct {
#ifdef HAVE_PAPI
  long long papictr1; ///< first PAPI counter values
  long long papictr2; ///< second PAPI counter values
#endif
#ifdef HAVE_LIKWID
  long long PMcounter0; ///< likwid MSR counter values
  long long PMcounter1; ///< likwid MSR counter values
  long long PMcounter2; ///< likwid MSR counter values
  long long PMcounter3; ///< likwid MSR counter values
  long long PMcounter4; ///< likwid MSR counter values
  long long PMcounter5; ///< likwid MSR counter values
  long long PMcounter6; ///< likwid MSR counter values
  long long PMcounter7; ///< likwid MSR counter values
  long long PMcounter8; ///< likwid MSR counter values
  long long PMcounter9; ///< likwid MSR counter values
#endif
  unsigned long long tduration; ///< the duration of the current epoch
  unsigned long toverhead; ///< the duration (overhead) of profiling for this epoch
  unsigned int id; ///< the user-specified kernel id
} t_rec;

typedef struct {
  unsigned int id; ///< the user-specified kernel id
  int int1; ///< integer 1
  int int2; ///< integer 2
} t_recint;

typedef struct {
  unsigned int id; ///< the user-specified kernel id
  int int1; ///< integer
  double dbl; ///< double 
} t_recintdbl;

/**
 * the global data for the profiler
 * this holds all state related with the profiling session
 */
typedef struct {
  std::vector<t_rec> recs; ///< records
  std::vector<t_recint> recints; ///< recorded integers
  std::vector<t_recintdbl> recintdbl; ///< recorded integer/double pairs
#ifdef HAVE_PAPI
  int eventset; ///< PAPI eventset config
  int papi1, papi2;
#endif
#ifdef HAVE_UNWIND
  std::vector< std::vector<long> > iptrace; ///< the instruction pointer values for the stack for each epoch
#endif
#ifdef HAVE_MPI
  int r; ///< MPI rank
  int p; ///< MPI processes in world
#endif
  FILE *outfd; ///< the output file
  unsigned long long tstart; ///< the program start time
  unsigned long long tlast; ///< lasttime starttime of the current epoch
  char print; ///< internal flag if this process prints or not
  char write_file; ///< internal flag if this process writes a file or not
  char rec_enabled; ///< true if recording is enabled, false if it is not
  char pgt_disabled; ///< true if pgt is completely disabled
} PGT_Data;

#define CHK_DISABLED if(pgt_data->pgt_disabled) return;

// prototypes
#include "libpgt.h"
