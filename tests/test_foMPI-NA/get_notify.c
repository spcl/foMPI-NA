/*
 * get_notify.c
 *
 *  Created on: Jul 8, 2014
 *      Author: Roberto Belli
 */
#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rmaex_get_notify(MPI_Comm comm) {
	int rank, commsize;
	MPI_Comm_rank(comm, &rank);
	MPI_Comm_size(comm, &commsize);

	int first_sender = (rank % 2 == 0) ? 1 : 0;
	int receive_from = (first_sender) ? rank + 1 : rank - 1;
	int send_to = (first_sender) ? rank + 1 : rank - 1;

	foMPI_Request notification_request;
	foMPI_Win win_send;
	MPI_Status notification_status;
	int num_item_to_send = 4;
	int tot_item = num_item_to_send * 2;
	int size = tot_item * sizeof(uint64_t);	//byte
	int tag = 0;
	int status;

	void *baseptr_send;
	uint64_t *cast_baseptr;

	status = foMPI_Win_allocate(size, sizeof(uint64_t), MPI_INFO_NULL, comm, &baseptr_send,
			&win_send);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
	cast_baseptr = (uint64_t *) baseptr_send;
	int i = 0;
	if (first_sender) {
		for (i = 0; i < num_item_to_send; i++) {
			cast_baseptr[i] = rank * 100000 + i;
		}
		for (; i < tot_item; i++) {
			cast_baseptr[i] = 0;
		}
	} else {
		for (i = 0; i < tot_item; i++) {
			cast_baseptr[i] = rank;
		}
	}

	foMPI_Win_fence(0,win_send);

	if (!first_sender) {
		for (i = 0; i < num_item_to_send; i++) {
			status = foMPI_Get_notify(&(cast_baseptr[i]), 1, MPI_UINT64_T, send_to, i, 1,
					MPI_UINT64_T, win_send, tag);
			_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
		}
		foMPI_Win_flush_all(win_send);
		/*check results*/
		int error = 0;
		for (i = 0; i < num_item_to_send; i++) {
			if (cast_baseptr[i] != send_to * 100000 + i) {
				error++;
				fprintf(stdout, "TEST[%d]: receive_buffer[%d]=%d expected=%d   \n", rank, i,
						cast_baseptr[i], send_to * 100000 + i);
			}
		}
		if(error!=0){
			abort();
		}
	} else {
		status = foMPI_Notify_init(win_send, foMPI_ANY_SOURCE, tag , num_item_to_send, &notification_request);
		_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

		status = foMPI_Start(&notification_request);
		_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
		status = foMPI_Wait(&notification_request, &notification_status);
		_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

		status = foMPI_Request_free(&notification_request);
		_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
	}

	if (rank == 0) {
		printf("No Error\n");
	}
	status = foMPI_Win_free(&win_send);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
}
