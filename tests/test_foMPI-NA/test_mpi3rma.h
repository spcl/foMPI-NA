
typedef struct float_int {
  float value;
  int index;
} float_int_t;

void test_mpi3rma_acc_replace_lock( int* base, int DIM, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_acc_sum_fence( int* base, int DIM, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_acc_sum_lock( int* base, int DIM, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_acc_sum_pscw( int* base, int DIM, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_cswap_fence( int* base, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_cswap_lock( int* base, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_cswap_pscw( int* base, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_cswap_special_fence( int64_t* base, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_cswap_special_lock( int64_t* base, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_cswap_special_pscw( int64_t* base, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_ddt_1( int* base, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_ddt_2( int* base, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_ddt_3( int* base, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_dynamic_attach_1( int DIM, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_dynamic_attach_2( int DIM, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_dynamic_ddt_1( int* base, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_dynamic_ddt_2( int* base, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_dynamic_ddt_3( int* base, int DIM, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_dynamic_ddt_4( int* base, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_dynamic_ddt_5( int* base, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_dynamic_ddt_6( int* base, int DIM, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_excl_lock( MPI_Comm comm );
void test_mpi3rma_fetch_and_op_sum_fence( int* base, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_fetch_and_op_sum_lock( int* base, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_fetch_and_op_sum_pscw( int* base, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_get_acc_maxloc_lock( float_int_t* base, int DIM, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_get_acc_no_op_lock( int* base, int DIM, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_get_acc_replace_lock( int* base, int DIM, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_get_acc_sum_fence( int* base, int DIM, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_get_acc_sum_lock( int* base, int DIM, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_get_acc_sum_pscw( int* base, int DIM, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_get_fence( int* base, int DIM, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_get_lock( int* base, int DIM, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_get_pscw( int* base, int DIM, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_lock_exclusive_concurrency( int* base, int DIM, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_null_pointer( MPI_Comm );
void test_mpi3rma_pscw1( foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_pscw2( foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_put_fence( int* base, int DIM, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_put_lock( int* base, int DIM, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_put_pscw( int* base, int DIM, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_racc_sum_lock( int* base, int DIM, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_rget_acc_sum_lock( int* base, int DIM, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_rget_lockall( int* base, int DIM, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_rput_rget_lock( int* base, int DIM, foMPI_Win win, MPI_Comm comm );
void test_mpi3rma_win_attr( MPI_Comm comm );
void test_mpi3rma_win_name( foMPI_Win win );

void test_mpi3rmaex_put_cons(MPI_Comm comm);
void test_mpi3rmaex_put_notify(MPI_Comm comm);
void test_mpi3rmaex_put_notify_2(MPI_Comm comm);
void test_mpi3rmaex_put_notify_pingpong(MPI_Comm comm);
void test_mpi3rmaex_put_notify_no_data(MPI_Comm comm);
void test_mpi3rmaex_put_notify_no_data_wildcards(MPI_Comm comm);
void test_mpi3rmaex_get_notify(MPI_Comm comm);
//void test_mpi3rmaex_dscqueue();
void test_mpi3rma_dynamic_ddt_notify_1(int* base, foMPI_Win win, MPI_Comm comm);
void test_mpi3rma_dynamic_ddt_notify_2(int* base, foMPI_Win win, MPI_Comm comm);
void test_mpi3rma_dynamic_ddt_notify_3(int* base, int DIM, foMPI_Win win, MPI_Comm comm);
void test_mpi3rma_dynamic_ddt_get_notify_1(int* base, foMPI_Win win, MPI_Comm comm);
void test_mpi3rma_dynamic_ddt_get_notify_2(int* base, foMPI_Win win, MPI_Comm comm);


inline static void _check_fompi_status(int actual, int expected, char* file, int line) {
  if (actual == expected) return;
  fprintf(stderr, "Error on rank %i in %s line %i: return code %i instead of %i\n", glob_info.debug_pe, file, line, actual, expected);
  exit(EXIT_FAILURE);
}
