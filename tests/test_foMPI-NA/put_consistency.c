/*
 * put_consistency.c
 *
 *  Created on: Jul 8, 2014
 *      Author: Roberto Belli
 */
#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rmaex_put_cons(MPI_Comm comm) {
	int rank, commsize;
	MPI_Comm_rank(comm, &rank);
	MPI_Comm_size(comm, &commsize);

	int status;
	int first_sender = (rank % 2 == 0) ? 1 : 0;
	int receive_from = (first_sender) ? rank + 1 : rank - 1;
	int send_to = (first_sender) ? rank + 1 : rank - 1;


	foMPI_Win win_send;
	int num_item_to_send = 4;
	int tot_item = num_item_to_send * 2;
	int size = tot_item * sizeof(uint64_t);	//byte
	int disp_unit = 1;

	void *baseptr_send;

	status = foMPI_Win_allocate(size, disp_unit, MPI_INFO_NULL, comm, &baseptr_send, &win_send);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

	int i = 0;
	if (first_sender) {
		for (i = 0; i < num_item_to_send; i++) {
			((uint64_t *) baseptr_send)[i] = rank * 100000 + i;
		}
		for (; i < tot_item; i++) {
			((uint64_t *) baseptr_send)[i] = 0;
		}
	} else {
		for (i = 0; i < tot_item; i++) {
			((uint64_t *) baseptr_send)[i] = rank;
		}
	}

	if (first_sender) {
		for (i = 0; i < num_item_to_send; i++) {
			status = foMPI_Put_notify( baseptr_send + (i * sizeof(uint64_t)), 1 , MPI_UINT64_T, send_to, (i * sizeof(uint64_t)) , 1, MPI_UINT64_T, win_send, MPI_ANY_TAG );
			_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

		}

	} else {
					status = foMPI_Put_notify( baseptr_send + (i * sizeof(uint64_t)), 1 , MPI_UINT64_T, send_to, (i * sizeof(uint64_t)) , 1, MPI_UINT64_T, win_send, MPI_ANY_TAG );
					_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);


	}

	foMPI_Win_fence( 0, win_send );

	int error = 0;
	if (rank == 1) {
		for (i = 0; i < num_item_to_send; i++) {
			if (((uint64_t *) baseptr_send)[i] != receive_from * 100000 + i) {
				error++;
				fprintf(stdout, "TEST[%d]: receive_buffer[%d]=%d expected=%d   \n", rank, i,
															((uint64_t *) baseptr_send)[i], receive_from * 100000 + i);
			}
		}
		if (error == 0) {
				printf("No Error\n");
		}
	}

	status = foMPI_Win_free(&win_send);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

}
