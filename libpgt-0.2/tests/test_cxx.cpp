#include "../libpgt.h"
#include "../config.h"

#include <unistd.h>


#ifdef HAVE_MPI
#include <mpi.h>
#endif

float y=10;
int x=100;

int main() {
#ifdef HAVE_MPI
  MPI_Init(NULL,NULL);
#endif

  PGT_Init("test_v1", 100);


  //sleep(10);
  int iters = 100000000;
  PGT_Reg_param("iters=%i", iters);
  for(int i=0; i<iters; i++) { x=x%10; y=y/2; }

  PGT_Finalize();

#ifdef HAVE_MPI
  MPI_Finalize();
#endif

}
