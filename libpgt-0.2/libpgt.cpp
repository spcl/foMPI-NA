#include "config.h"
#include "libpgt_internal.hpp"

#ifdef HAVE_MPI
#include <mpi.h>
#endif
#ifdef HAVE_HRTIMER
#include "hrtimer/hrtimer.h"
#endif

PGT_Data *pgt_data;

#ifdef HAVE_LIKWID
std::vector<long long> PMvalues(10);
#endif

#ifdef HAVE_HRTIMER
uint64_t libpgt_g_timerfreq;
double libpgt_g_hrtimer_startvalue;
#endif

/**
 * end the current epoch and start a new one
 * same as PGT_Rec() but doesn't record anything -> all measurements will be lost
 */
void PGT_Res() {
  CHK_DISABLED;
#ifdef _OPENMP
#pragma omp master
{
#endif
#if defined HAVE_PAPI 
  std::vector<long long> values(2);
  PAPI_stop(pgt_data->eventset, &values[0]);
  pgt_data->tlast = PAPI_get_real_usec();
  PAPI_start(pgt_data->eventset);
#elif defined HAVE_LIKWID
    int cpu_id;
    cpu_id = likwid_getProcessorId();
    PMvalues[0] = msr_read(cpu_id, MSR_AMD15_PMC0);
    PMvalues[1] = msr_read(cpu_id, MSR_AMD15_PMC1);
    PMvalues[2] = msr_read(cpu_id, MSR_AMD15_PMC2);
    PMvalues[3] = msr_read(cpu_id, MSR_AMD15_PMC3);
    PMvalues[4] = msr_read(cpu_id, MSR_AMD15_PMC4);
    PMvalues[5] = msr_read(cpu_id, MSR_AMD15_PMC5);
    PMvalues[6] = msr_read(cpu_id, MSR_AMD15_NB_PMC0);
    PMvalues[7] = msr_read(cpu_id, MSR_AMD15_NB_PMC1);
    PMvalues[8] = msr_read(cpu_id, MSR_AMD15_NB_PMC2);
    PMvalues[9] = msr_read(cpu_id, MSR_AMD15_NB_PMC3);
    unsigned long long ticks;
    HRT_TIMESTAMP_T t;
    HRT_GET_TIMESTAMP(t);
    HRT_GET_TIME(t, ticks);
    pgt_data->tlast =  HRT_GET_USEC(ticks);
#elif defined HAVE_HRTIMER
	unsigned long long ticks;
	HRT_TIMESTAMP_T t;
	HRT_GET_TIMESTAMP(t);
	HRT_GET_TIME(t, ticks);
	pgt_data->tlast = HRT_GET_USEC(ticks);
#else
#error "No possibility to do accurate timing!\n"
#endif
#ifdef _OPENMP
}
#endif
}

void PGT_Rec_ints(unsigned int id, int int1, int int2) {
  CHK_DISABLED;
#ifdef _OPENMP
#pragma omp master
{
#endif
  t_recint rec = {id,int1,int2};
  if(pgt_data->rec_enabled)  pgt_data->recints.push_back(rec);
#ifdef _OPENMP
}
#endif
}

void PGT_Rec_intdbl(unsigned int id, int int1, double dbl) {
  CHK_DISABLED;
#ifdef _OPENMP
#pragma omp master
{
#endif
  t_recintdbl rec = {id,int1,dbl};
  if(pgt_data->rec_enabled) pgt_data->recintdbl.push_back(rec);
#ifdef _OPENMP
}
#endif
}


/**
 * write a tracing record
 * this ends the epoch with the specified id and opens a new epoch
 * @param id user-defined kernel id
 */
void PGT_Rec(unsigned int id) {
  CHK_DISABLED;
#ifdef _OPENMP
#pragma omp master
{
#endif
  unsigned long long tstart; // epoch duration and function start time
#if defined HAVE_PAPI
  tstart  = PAPI_get_real_usec();

  std::vector<long long> values(2);
  PAPI_stop(pgt_data->eventset, &values[0]);
#elif defined HAVE_LIKWID
    //std::vector<long long> values(10);
    int cpu_id;
    cpu_id = likwid_getProcessorId();
    PMvalues[0] = msr_read(cpu_id, MSR_AMD15_PMC0) - PMvalues[0];
    PMvalues[1] = msr_read(cpu_id, MSR_AMD15_PMC1) - PMvalues[1];
    PMvalues[2] = msr_read(cpu_id, MSR_AMD15_PMC2) - PMvalues[2];
    PMvalues[3] = msr_read(cpu_id, MSR_AMD15_PMC3) - PMvalues[3];
    PMvalues[4] = msr_read(cpu_id, MSR_AMD15_PMC4) - PMvalues[4];
    PMvalues[5] = msr_read(cpu_id, MSR_AMD15_PMC5) - PMvalues[5];
    PMvalues[6] = msr_read(cpu_id, MSR_AMD15_NB_PMC0) - PMvalues[6];
    PMvalues[7] = msr_read(cpu_id, MSR_AMD15_NB_PMC1) - PMvalues[7];
    PMvalues[8] = msr_read(cpu_id, MSR_AMD15_NB_PMC2) - PMvalues[8];
    PMvalues[9] = msr_read(cpu_id, MSR_AMD15_NB_PMC3) - PMvalues[9];
    unsigned long long ticks;
    HRT_TIMESTAMP_T t;
    HRT_GET_TIMESTAMP(t);
    HRT_GET_TIME(t, ticks);
    tstart = HRT_GET_USEC(ticks);
#elif defined HAVE_HRTIMER
	unsigned long long ticks;
	HRT_TIMESTAMP_T t;
	HRT_GET_TIMESTAMP(t);
	HRT_GET_TIME(t, ticks);
	tstart = HRT_GET_USEC(ticks);
#else
#error "No possibility to do accurate timing!\n"
#endif
  
#if defined HAVE_PAPI
  t_rec rec = {values[0], values[1], tstart-pgt_data->tlast, 0, id};
#elif defined HAVE_LIKWID
  t_rec rec = {PMvalues[0], PMvalues[1], PMvalues[2], PMvalues[3], PMvalues[4], \
               PMvalues[5], PMvalues[6], PMvalues[7], PMvalues[8], PMvalues[9], \
               tstart-pgt_data->tlast, 0, id};
#else
  t_rec rec = {tstart-pgt_data->tlast, 0, id};
#endif
  if(pgt_data->rec_enabled) pgt_data->recs.push_back(rec);

#ifdef HAVE_UNWIND
  unw_cursor_t cursor; unw_context_t uc;
  unw_word_t ip;
  unw_getcontext(&uc);

  if(pgt_data->rec_enabled) {
    pgt_data->iptrace.resize(pgt_data->iptrace.size()+1);
    unw_init_local(&cursor, &uc);
    while (unw_step(&cursor) > 0) {
      unw_get_reg(&cursor, UNW_REG_IP, &ip);
      //unw_get_reg(&cursor, UNW_REG_SP, &sp);
      pgt_data->iptrace[pgt_data->iptrace.size()-1].push_back(ip);
      //printf ("ip = %lx, sp = %lx\n", (long) ip, (long) sp);
    }
  }
#endif

#if defined HAVE_PAPI
  PAPI_start(pgt_data->eventset);

  pgt_data->tlast = PAPI_get_real_usec();
#elif  HAVE_LIKWID
    PMvalues[0] = msr_read(cpu_id, MSR_AMD15_PMC0);
    PMvalues[1] = msr_read(cpu_id, MSR_AMD15_PMC1);
    PMvalues[2] = msr_read(cpu_id, MSR_AMD15_PMC2);
    PMvalues[3] = msr_read(cpu_id, MSR_AMD15_PMC3);
    PMvalues[4] = msr_read(cpu_id, MSR_AMD15_PMC4);
    PMvalues[5] = msr_read(cpu_id, MSR_AMD15_PMC5);
    PMvalues[6] = msr_read(cpu_id, MSR_AMD15_NB_PMC0);
    PMvalues[7] = msr_read(cpu_id, MSR_AMD15_NB_PMC1);
    PMvalues[8] = msr_read(cpu_id, MSR_AMD15_NB_PMC2);
    PMvalues[9] = msr_read(cpu_id, MSR_AMD15_NB_PMC3);
    HRT_GET_TIMESTAMP(t);
    HRT_GET_TIME(t, ticks);
    pgt_data->tlast =  HRT_GET_USEC(ticks);
#elif defined HAVE_HRTIMER
	HRT_GET_TIMESTAMP(t);
	HRT_GET_TIME(t, ticks);
	pgt_data->tlast = HRT_GET_USEC(ticks);
#else
#error "No possibility to do accurate timing!\n"
#endif
  if(pgt_data->rec_enabled) pgt_data->recs.back().toverhead = pgt_data->tlast-tstart;
#ifdef _OPENMP
}
#endif
}

/**
 * simple handler for SIGALRM callback
 * simply calls PGT_Rec to record an epoch
 * @param x epoch identifier
 */
void PGT_Handler(int x) {
  PGT_Rec(0);
}

/**
 *  flush data to disk and end current measurement
 *  this is only used if an application is started multiple times with different parameters
 */
void PGT_Flush() {
  CHK_DISABLED;
#ifdef _OPENMP
#pragma omp master
{
#endif

  unsigned long long tend;
#if defined HAVE_PAPI
  tend = PAPI_get_real_usec();
#elif defined HAVE_HRTIMER
  	unsigned long long ticks;
	HRT_TIMESTAMP_T t;
	HRT_GET_TIMESTAMP(t);
	HRT_GET_TIME(t, ticks);
	tend = HRT_GET_USEC(ticks);
#else
#error "No possibility to do accurate timing!\n"
#endif

  FILE *fp=pgt_data->outfd;

  enum {PRETTY, EFFICIENT, ACCUMULATED} style = PRETTY; // printing style
  char *env = getenv("PGT_OUTPUT_FORMAT"); // print format
  if(env != NULL) {
    if(strstr(env, "efficient")) style = EFFICIENT;
    if(strstr(env, "accumulated")) style = ACCUMULATED;
  }

#ifdef HAVE_MPI
  if(pgt_data->write_file==1) fprintf(fp, "# MPI execution on rank %i with %i processes in world\n", pgt_data->r, pgt_data->p);
#endif

  unsigned long long dur = 0; for(unsigned int i=0; i<pgt_data->recs.size(); ++i) dur += pgt_data->recs[i].toverhead;
  if(pgt_data->write_file==1) fprintf(fp, "# Runtime: %lf s (overhead: %lf %%) %i records\n", (double)(tend-pgt_data->tstart)/1e6, (double)dur/(tend-pgt_data->tstart)*100, (int)pgt_data->recs.size());

  if((style == PRETTY) && (pgt_data->write_file==1)) {
    fprintf(fp, "# pretty output format\n");
    fprintf(fp, "  %8s %12s %12s ", "id", "time", "overhead");
  #ifdef HAVE_PAPI
    char p1[1024], p2[1024];
    PAPI_event_code_to_name(pgt_data->papi1, p1);
    PAPI_event_code_to_name(pgt_data->papi2, p2);
    fprintf(fp, "%16s %16s", p1, p2);
  #endif
  #ifdef HAVE_UNWIND
    fprintf(fp, " #ips ip-list");
  #endif
    fprintf(fp, "\n");

    // print all records
    for(unsigned int i=0; i < pgt_data->recs.size(); ++i) {
      fprintf(fp, "  %8i ", pgt_data->recs[i].id);
      fprintf(fp, "%12llu ", pgt_data->recs[i].tduration);
      fprintf(fp, "%12lu ", pgt_data->recs[i].toverhead);
  #ifdef HAVE_PAPI
      fprintf(fp, "%16lli ", pgt_data->recs[i].papictr1);
      fprintf(fp, "%16lli ", pgt_data->recs[i].papictr2);
  #endif
  #ifdef HAVE_LIKWID
      fprintf(fp, "%16lli ", pgt_data->recs[i].PMcounter0);
      fprintf(fp, "%16lli ", pgt_data->recs[i].PMcounter1);
      fprintf(fp, "%16lli ", pgt_data->recs[i].PMcounter2);
      fprintf(fp, "%16lli ", pgt_data->recs[i].PMcounter3);
      fprintf(fp, "%16lli ", pgt_data->recs[i].PMcounter4);
      fprintf(fp, "%16lli ", pgt_data->recs[i].PMcounter5);
      fprintf(fp, "%16lli ", pgt_data->recs[i].PMcounter6);
      fprintf(fp, "%16lli ", pgt_data->recs[i].PMcounter7);
      fprintf(fp, "%16lli ", pgt_data->recs[i].PMcounter8);
      fprintf(fp, "%16lli ", pgt_data->recs[i].PMcounter9);
  #endif
  #ifdef HAVE_UNWIND
      fprintf(fp, "%i ", (int)pgt_data->iptrace[i].size());
      for(unsigned int j=0; j<pgt_data->iptrace[i].size(); ++j) fprintf(fp, "%lx ", pgt_data->iptrace[i][j]);
  #endif
      fprintf(fp, "\n");
    }

    if(pgt_data->recints.size()) {
      // print all ints
      fprintf(fp, "# printing integer list, %i records\n  %8s %8s %8s\n", (int)pgt_data->recints.size(), "id", "int1", "int2");
      for(unsigned int i=0; i < pgt_data->recints.size(); ++i) {
        fprintf(fp, "  %8i ", pgt_data->recints[i].id);
        fprintf(fp, "%8i ", pgt_data->recints[i].int1);
        fprintf(fp, "%8i ", pgt_data->recints[i].int2);
        fprintf(fp, "\n");
      }
    }

    if(pgt_data->recintdbl.size()) {
      // print all ints
      fprintf(fp, "# printing integer/double list, %i records\n  %8s %8s %16s\n", (int)pgt_data->recintdbl.size(), "id", "int1", "double");
      for(unsigned int i=0; i < pgt_data->recintdbl.size(); ++i) {
        fprintf(fp, "  %8i ", pgt_data->recintdbl[i].id);
        fprintf(fp, "%8i ", pgt_data->recintdbl[i].int1);
        fprintf(fp, "%16f ", pgt_data->recintdbl[i].dbl);
        fprintf(fp, "\n");
      }
    }
  } else if ((style == EFFICIENT) && (pgt_data->write_file==1)) {
    fprintf(fp, "# efficient output format\n");
    fprintf(fp, "  %s %s %s ", "id", "time", "overhead");
#ifdef HAVE_PAPI
    char p1[1024], p2[1024];
    PAPI_event_code_to_name(pgt_data->papi1, p1);
    PAPI_event_code_to_name(pgt_data->papi2, p2);
    fprintf(fp, "%s %s", p1, p2);
#endif
#ifdef HAVE_UNWIND
    fprintf(fp, " #ips ip-list");
#endif
    fprintf(fp, "\n");

    // print all records
    for(unsigned int i=0; i < pgt_data->recs.size(); ++i) {
      fprintf(fp, "%i ", pgt_data->recs[i].id);
      fprintf(fp, "%llu ", pgt_data->recs[i].tduration);
      fprintf(fp, "%lu ", pgt_data->recs[i].toverhead);
  #ifdef HAVE_PAPI
      fprintf(fp, "%lli ", pgt_data->recs[i].papictr1);
      fprintf(fp, "%lli ", pgt_data->recs[i].papictr2);
  #endif
  #ifdef HAVE_LIKWID
      fprintf(fp, "%lli ", pgt_data->recs[i].PMcounter0);
      fprintf(fp, "%lli ", pgt_data->recs[i].PMcounter1);
      fprintf(fp, "%lli ", pgt_data->recs[i].PMcounter2);
      fprintf(fp, "%lli ", pgt_data->recs[i].PMcounter3);
      fprintf(fp, "%lli ", pgt_data->recs[i].PMcounter4);
      fprintf(fp, "%lli ", pgt_data->recs[i].PMcounter5);
      fprintf(fp, "%lli ", pgt_data->recs[i].PMcounter6);
      fprintf(fp, "%lli ", pgt_data->recs[i].PMcounter7);
      fprintf(fp, "%lli ", pgt_data->recs[i].PMcounter8);
      fprintf(fp, "%lli ", pgt_data->recs[i].PMcounter9);
  #endif
  #ifdef HAVE_UNWIND
      fprintf(fp, "%i ", (int)pgt_data->iptrace[i].size());
      for(unsigned int j=0; j<pgt_data->iptrace[i].size(); ++j) fprintf(fp, "%lx ", pgt_data->iptrace[i][j]);
  #endif
      fprintf(fp, "\n");
    }

    if(pgt_data->recints.size()) {
      // print all ints
      fprintf(fp, "# printing integer list, %i records\n  %8s %8s %8s\n", (int)pgt_data->recints.size(), "id", "int1", "int2");
      for(unsigned int i=0; i < pgt_data->recints.size(); ++i) {
        fprintf(fp, "%i ", pgt_data->recints[i].id);
        fprintf(fp, "%i ", pgt_data->recints[i].int1);
        fprintf(fp, "%i ", pgt_data->recints[i].int2);
        fprintf(fp, "\n");
      }
    }

    if(pgt_data->recintdbl.size()) {
      // print all ints
      fprintf(fp, "# printing integer/double list, %i records\n  %8s %8s %16s\n", (int)pgt_data->recintdbl.size(), "id", "int1", "double");
      for(unsigned int i=0; i < pgt_data->recintdbl.size(); ++i) {
        fprintf(fp, "%i ", pgt_data->recintdbl[i].id);
        fprintf(fp, "%i ", pgt_data->recintdbl[i].int1);
        fprintf(fp, "%f ", pgt_data->recintdbl[i].dbl);
        fprintf(fp, "\n");
      }
    }
  } else if (style == ACCUMULATED) {

    MPI_Comm comm = MPI_COMM_WORLD;
    int size = pgt_data->recs.size();
    int max, min;
    MPI_Allreduce(&size,&max,1,MPI_INT,MPI_MAX,comm);
    MPI_Allreduce(&size,&min,1,MPI_INT,MPI_MIN,comm);
    if(max != min) {
      if(pgt_data->r == 0) printf("not the same number of records on all processes (min: %i, max: %i)! No output is generated!", min, max);
    } else {
      std::vector<long long> my_duration(size); // convert to long long for MPI :-(
      std::vector<long> my_overhead(size); // convert to long for MPI :-(
#ifdef HAVE_PAPI
      std::vector<long long> my_papictr1(size), my_papictr2(size); // convert to long long for MPI :-(
#endif
      for(int i=0; i<size; ++i) {
        my_duration[i] = pgt_data->recs[i].tduration;
        my_overhead[i] = pgt_data->recs[i].toverhead;
#ifdef HAVE_PAPI
        my_papictr1[i] = pgt_data->recs[i].papictr1;
        my_papictr2[i] = pgt_data->recs[i].papictr2;
#endif
      }

      std::vector<long long> max_duration(size), min_duration(size), avg_duration(size);
      std::vector<long> max_overhead(size), min_overhead(size), avg_overhead(size);
#ifdef HAVE_PAPI
      std::vector<long long> max_papictr1(size), max_papictr2(size), min_papictr1(size),
                             min_papictr2(size), avg_papictr1(size), avg_papictr2(size);
#endif
      MPI_Allreduce(&my_overhead[0],&min_overhead[0],size,MPI_LONG,MPI_MIN,comm);
      MPI_Allreduce(&my_overhead[0],&max_overhead[0],size,MPI_LONG,MPI_MAX,comm);
      MPI_Allreduce(&my_overhead[0],&avg_overhead[0],size,MPI_LONG,MPI_SUM,comm);
      for(int i=0; i<size; ++i) avg_overhead[i]/= pgt_data->p;

      MPI_Allreduce(&my_duration[0],&min_duration[0],size,MPI_LONG_LONG,MPI_MIN,comm);
      MPI_Allreduce(&my_duration[0],&max_duration[0],size,MPI_LONG_LONG,MPI_MAX,comm);
      MPI_Allreduce(&my_duration[0],&avg_duration[0],size,MPI_LONG_LONG,MPI_SUM,comm);
      for(int i=0; i<size; ++i) avg_duration[i]/= pgt_data->p;

#ifdef HAVE_PAPI
      MPI_Allreduce(&my_papictr1[0],&min_papictr1[0],size,MPI_LONG_LONG,MPI_MIN,comm);
      MPI_Allreduce(&my_papictr1[0],&max_papictr1[0],size,MPI_LONG_LONG,MPI_MAX,comm);
      MPI_Allreduce(&my_papictr1[0],&avg_papictr1[0],size,MPI_LONG_LONG,MPI_SUM,comm);
      for(int i=0; i<size; ++i) avg_papictr1[i]/= pgt_data->p;

      MPI_Allreduce(&my_papictr2[0],&min_papictr2[0],size,MPI_LONG_LONG,MPI_MIN,comm);
      MPI_Allreduce(&my_papictr2[0],&max_papictr2[0],size,MPI_LONG_LONG,MPI_MAX,comm);
      MPI_Allreduce(&my_papictr2[0],&avg_papictr2[0],size,MPI_LONG_LONG,MPI_SUM,comm);
      for(int i=0; i<size; ++i) avg_papictr2[i]/= pgt_data->p;
#endif

      if(pgt_data->write_file==1) {
        fprintf(fp, "# accumulated output format\n");
        fprintf(fp, "%s %s - %s - ", "id", "time (min, avg, max)", "overhead (min, avg, max)");
#ifdef HAVE_PAPI
        char p1[1024], p2[1024];
        PAPI_event_code_to_name(pgt_data->papi1, p1);
        PAPI_event_code_to_name(pgt_data->papi2, p2);
        fprintf(fp, "%s - %s", p1, p2);
#endif
#ifdef HAVE_UNWIND
        fprintf(fp, " #ips ip-list");
#endif
        fprintf(fp, "\n");
      }

      // print all records
      if(pgt_data->write_file==1) for(unsigned int i=0; i < size; ++i) {
        fprintf(fp, "%i ", pgt_data->recs[i].id);
        fprintf(fp, "%lli %lli %lli - ", min_duration[i], avg_duration[i], max_duration[i] );
        fprintf(fp, "%li %li %li - ", min_overhead[i], avg_overhead[i], max_overhead[i]);
#ifdef HAVE_PAPI
        fprintf(fp, "%lli %lli %lli - ", min_papictr1[i], avg_papictr1[i], max_papictr1[i]);
        fprintf(fp, "%lli %lli %lli - ", min_papictr2[i], avg_papictr2[i], max_papictr2[i]);
#endif
#ifdef HAVE_UNWIND
        fprintf(fp, "%i ", (int)pgt_data->iptrace[i].size());
        for(unsigned int j=0; j<pgt_data->iptrace[i].size(); ++j) fprintf(fp, "%lx ", pgt_data->iptrace[i][j]);
#endif
        fprintf(fp, "\n");
      }
    }
  }

  pgt_data->recs.clear();
  pgt_data->recints.clear();
  pgt_data->recintdbl.clear();

#ifdef HAVE_UNWIND
  pgt_data->ipctrace.clear();
#endif

#if defined HAVE_PAPI
  pgt_data->tstart = PAPI_get_real_usec();
#elif defined HAVE_LIKWID
    int cpu_id;
    cpu_id = likwid_getProcessorId();
    PMvalues[0] = msr_read(cpu_id, MSR_AMD15_PMC0);
    PMvalues[1] = msr_read(cpu_id, MSR_AMD15_PMC1);
    PMvalues[2] = msr_read(cpu_id, MSR_AMD15_PMC2);
    PMvalues[3] = msr_read(cpu_id, MSR_AMD15_PMC3);
    PMvalues[4] = msr_read(cpu_id, MSR_AMD15_PMC4);
    PMvalues[5] = msr_read(cpu_id, MSR_AMD15_PMC5);
    PMvalues[6] = msr_read(cpu_id, MSR_AMD15_NB_PMC0);
    PMvalues[7] = msr_read(cpu_id, MSR_AMD15_NB_PMC1);
    PMvalues[8] = msr_read(cpu_id, MSR_AMD15_NB_PMC2);
    PMvalues[9] = msr_read(cpu_id, MSR_AMD15_NB_PMC3);
    HRT_GET_TIMESTAMP(t);
    HRT_GET_TIME(t, ticks);
    pgt_data->tstart =  HRT_GET_USEC(ticks);
#elif defined HAVE_HRTIMER
	HRT_GET_TIMESTAMP(t);
	HRT_GET_TIME(t, ticks);
	pgt_data->tstart = HRT_GET_USEC(ticks);
#else
#error "No possibility to do accurate timing!\n"
#endif
#ifdef _OPENMP
}
#endif

}

/**
 * ends profiling
 * last function to be called in a profiling session
 */
static int PGT_finalized;
void PGT_Finalize() {
  CHK_DISABLED;
  if(PGT_finalized) return; // so that it can be called before atexit() if atexit doesn't work (e.g., BG/P)
#ifdef _OPENMP
#pragma omp master
{
#endif

  PGT_finalized = 1;

  if(pgt_data->print) printf("******* PGT_Finalize *******\n");

  if(pgt_data->recs.size() > 0 || pgt_data->recints.size() || pgt_data->recintdbl.size()) PGT_Flush();

  if(pgt_data->write_file==1) fclose(pgt_data->outfd);
#ifdef _OPENMP
}
#endif
#ifdef HAVE_LIKWID
  perfmon_stopCounters();
  msr_finalize();
#endif
}

/**
 * internal helper function to write benchmark information in header
 * @param fd file descriptor to write to
 */
static void write_host_information(FILE *fd) {
#ifdef _OPENMP
#pragma omp master
{
#endif
  struct utsname uninfo;

  if (uname (&uninfo) >= 0) {
    fprintf(fd, "# Sysname : %s\n", uninfo.sysname);
    fprintf(fd, "# Nodename: %s\n", uninfo.nodename);
    fprintf(fd, "# Release : %s\n", uninfo.release);
    fprintf(fd, "# Version : %s\n", uninfo.version);
    fprintf(fd, "# Machine : %s\n", uninfo.machine);
  }

  time_t t = time(NULL);
  struct tm *local = gmtime(&t);
  fprintf(fd, "# Execution time and date (UTC): %s", asctime(local));
  local = localtime(&t);
  fprintf(fd, "# Execution time and date (local): %s", asctime(local));
#ifdef _OPENMP
}
#endif

}

/**
 * first function to be called to open a tracing session
 * @param projname name of the project
 * @param autoprof_interval automatic (SIGALRM) profiling interval, set 0 to disable automatic profiling
 */
void PGT_Init(const char* projname, int autoprof_interval /* in ms, off if 0 */) {
  const char *fname = projname;
  char name[1024];
  int rev;
  struct stat st;
  char *env;

  pgt_data = new(PGT_Data);
  pgt_data->rec_enabled = 1;
  pgt_data->pgt_disabled = 0;
  pgt_data->print=1;

  env = getenv("PGT_OUTFILE");
  if(env != NULL) fname = env;
  snprintf(name, 1023, "pgt.%s", fname);
  rev = 0;
  while(stat(name, &st) != -1) { // path exists!
    snprintf(name, 1023, "pgt.%s-%i", fname, ++rev);
  }

#ifdef HAVE_MPI
  int r; MPI_Comm_rank(MPI_COMM_WORLD, &r);
  pgt_data->r=r;
  MPI_Comm_size(MPI_COMM_WORLD, &pgt_data->p);
  if(r > 0) pgt_data->print = 0;
  snprintf(name, 1023, "pgt.%s.r%i", fname, r);
  rev = 0;
  while(stat(name, &st) != -1) { // path exists!
    snprintf(name, 1023, "pgt.%s.r%i-%i", fname, r, ++rev);
  }

  pgt_data->write_file=1;
  env = getenv("PGT_OUTPUT_FORMAT"); // if accumulated is specified, then all processes but rank 0 are disabled!
  if((env != NULL) && (strstr(env, "accumulated") != NULL)) {
    pgt_data->write_file=0;
    if(pgt_data->r == 0) pgt_data->write_file=1;
  }
#endif

  env = getenv("PGT_ENABLE_PROCMASK"); // list of enabled processes
  if(env != NULL) {
    pgt_data->pgt_disabled=1;
    // parse comma-separated list of ints
    for( char *s2 = env; s2; ){
      while( *s2 == ' ' || *s2 == '\t' ) s2++;
      char *s1 = strsep( &s2, "," );
      if( !*s1 ){
        //printf("val: (empty)\n" );
      } else {
        int val;
        char ch;
        int ret = sscanf( s1, " %i %c", &val, &ch );
        if( ret != 1 ){
          //printf("val: (syntax error)\n" );
        }
        else{
          //printf("val: %i\n", val );
          if(r==val) pgt_data->pgt_disabled=0;
        }
      }
    }
  }
  CHK_DISABLED;

  if(pgt_data->write_file==1) {
    pgt_data->outfd = fopen(name, "w");
    write_host_information(pgt_data->outfd);
  }

#ifdef HAVE_PAPI
  if(PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT ) {
    printf("PAPI initialization error! \n");
    exit(1);
  }
  pgt_data->eventset = PAPI_NULL;
  assert(PAPI_create_eventset(&pgt_data->eventset) == PAPI_OK);

  // define what to count! -- check "papi_event_chooser PRESET event1 event2"
  env = getenv("PGT_PAPI1");
  if(env != NULL) PAPI_event_name_to_code(env, &pgt_data->papi1);
  else pgt_data->papi1 = PAPI_TOT_INS;
  env = getenv("PGT_PAPI2");
  if(env != NULL) PAPI_event_name_to_code(env, &pgt_data->papi2);
  else pgt_data->papi2 = PAPI_TOT_CYC;

  if(PAPI_add_event(pgt_data->eventset, pgt_data->papi1) != PAPI_OK) {
    char p1[1024];
    PAPI_event_code_to_name(pgt_data->papi1, p1);
    if(pgt_data->print) printf("Adding PAPI counter 1 (%s) failed!\n", p1);
    exit(11);
  }
  if(PAPI_add_event(pgt_data->eventset, pgt_data->papi2) != PAPI_OK) {
    char p1[1024];
    PAPI_event_code_to_name(pgt_data->papi1, p1);
    if(pgt_data->print) printf("Adding PAPI counter 2 (%s) failed!\n", p1);
    exit(11);
  }
#endif

#ifdef HAVE_LIKWID
// init likwid using the low level API 
  FILE *OUTSTREAM = stdout;
  int numThreads=1;
  int threads[1];
  char *myeventstring;


// start likwid things
  cpuid_init();
  // printf("cpuid_topology.numHWThreads %d\n",cpuid_topology.numHWThreads);
  // numa_init(); // do we need this one ?
  // affinity_init(); // skip this, let aprun pin
  msr_init();

  threads[0]=likwid_getProcessorId();
  perfmon_init(numThreads,threads,OUTSTREAM);
  timer_init();

// Set the event set string
  myeventstring=(char *)malloc(60*sizeof(char));
  sprintf(myeventstring,"UNC_L3_CACHE_MISS_CORE_%d:UPMC%d",likwid_getProcessorId()%8,pgt_data->r%4);
  printf("LIKWID: rank %d event %s\n",pgt_data->r,myeventstring);
  bstring eventString = bfromcstr(myeventstring);
  perfmon_setupEventSet(eventString);
  perfmon_startCounters();
  free(myeventstring);
#endif

  if(pgt_data->print) {
    printf("***** PGT_Init >%s< writing to >%s< ", projname, name);
    if(autoprof_interval) printf("prof int: %i ms ", autoprof_interval);
    printf("*****\n");
  }

  if(autoprof_interval) {
    // register signal handler
    struct sigaction newact;
    newact.sa_handler = PGT_Handler;
    newact.sa_flags = 0;
    sigemptyset (&newact.sa_mask);

    sigaction(SIGVTALRM, &newact, NULL);

    // set timer
    struct itimerval timer;
    timer.it_interval.tv_sec=timer.it_value.tv_sec=0;
    timer.it_interval.tv_usec=timer.it_value.tv_usec=autoprof_interval*1000; // 1ms

    setitimer(ITIMER_VIRTUAL, &timer, NULL);
    
    // this is a workaround for autoprof malloc non-reentrant issues --
    // see USAGE for details
    env = getenv("PGT_AUTOPROF_WORKAROUND");
    if(env != NULL) {
      pgt_data->recs.reserve(atoi(env));
      if(pgt_data->print) printf("***** autoprof workaround - preallocating %i elements\n", atoi(env));
    }
  }

  PGT_finalized = 0;
  atexit(PGT_Finalize);

#if defined HAVE_PAPI
  // start PAPI
  PAPI_start(pgt_data->eventset);
  pgt_data->tlast = pgt_data->tstart = PAPI_get_real_usec();
#elif defined HAVE_LIKWID
    int cpu_id;
    cpu_id = likwid_getProcessorId();
    PMvalues[0] = msr_read(cpu_id, MSR_AMD15_PMC0);
    PMvalues[1] = msr_read(cpu_id, MSR_AMD15_PMC1);
    PMvalues[2] = msr_read(cpu_id, MSR_AMD15_PMC2);
    PMvalues[3] = msr_read(cpu_id, MSR_AMD15_PMC3);
    PMvalues[4] = msr_read(cpu_id, MSR_AMD15_PMC4);
    PMvalues[5] = msr_read(cpu_id, MSR_AMD15_PMC5);
    PMvalues[6] = msr_read(cpu_id, MSR_AMD15_NB_PMC0);
    PMvalues[7] = msr_read(cpu_id, MSR_AMD15_NB_PMC1);
    PMvalues[8] = msr_read(cpu_id, MSR_AMD15_NB_PMC2);
    PMvalues[9] = msr_read(cpu_id, MSR_AMD15_NB_PMC3);
    HRT_INIT(0, libpgt_g_timerfreq);
    unsigned long long ticks;
    HRT_TIMESTAMP_T t;
    HRT_GET_TIMESTAMP(t);
    HRT_GET_TIME(t, ticks);
    pgt_data->tstart = pgt_data->tlast =  HRT_GET_USEC(ticks);
#elif defined HAVE_HRTIMER
	HRT_INIT(0, libpgt_g_timerfreq);
  	unsigned long long ticks;
	HRT_TIMESTAMP_T t;
	HRT_GET_TIMESTAMP(t);
	HRT_GET_TIME(t, ticks);
	pgt_data->tlast = HRT_GET_USEC(ticks);
#else
#error "No possibility to do accurate timing!\n" 
#endif 
  
}

/**
 * registers a program parameter
 * @param format free float text (in printf-style)
 */
void PGT_Reg_param(const char *format, ...) {
  CHK_DISABLED;
#ifdef _OPENMP
#pragma omp master
{
#endif
  va_list val;
  char msg[1024];
  va_start(val, format);
  vsnprintf(msg, 1024, format, val);
  va_end(val);
  if(pgt_data->write_file) fprintf(pgt_data->outfd, "# Param: %s\n", msg);
#ifdef _OPENMP
}
#endif
}

/**
 * registers an identifier for output in the trace
 * @param format free float text (in printf-style)
 */
void PGT_Reg_id(const char *format, ...) {
  CHK_DISABLED;
#ifdef _OPENMP
#pragma omp master
{
#endif

  va_list val;
  char msg[1024];
  va_start(val, format);
  vsnprintf(msg, 1024, format, val);
  va_end(val);
  if(pgt_data->write_file) fprintf(pgt_data->outfd, "# Id: %s\n", msg);

#ifdef _OPENMP
}
#endif
}


void PGT_Rec_disable() {
  CHK_DISABLED;
#ifdef _OPENMP
#pragma omp master
{
#endif
  pgt_data->rec_enabled = 0;
#ifdef _OPENMP
}
#endif
}

void PGT_Rec_enable() {
  CHK_DISABLED;
#ifdef _OPENMP
#pragma omp master
{
#endif
  pgt_data->rec_enabled = 1;
#ifdef _OPENMP
}
#endif
}

