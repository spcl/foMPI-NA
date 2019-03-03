/*
 * put_notification.c
 *
 *  Created on: Jul 8, 2014
 *      Author: Roberto Belli
 */
#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rmaex_put_notify_2(MPI_Comm comm) {
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
	int disp_unit = sizeof(uint64_t);
	int tag = 99;
	int status;
	int error = 0;

	uint64_t *baseptr_send;

	status = foMPI_Win_allocate(size, disp_unit, MPI_INFO_NULL, comm, &baseptr_send, &win_send);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

	int i = 0;
	if (first_sender) {
		for (i = 0; i < num_item_to_send; i++) {
			baseptr_send[i] = rank * 100000 + i;
		}
		for (; i < tot_item; i++) {
			baseptr_send[i] = 0;
		}
	} else {
		for (i = 0; i < tot_item; i++) {
			baseptr_send[i] = rank;
		}
	}
	foMPI_Win_fence(0, win_send);
	if (first_sender) {
		for (i = 1; i <= num_item_to_send; i++) {
			status = foMPI_Put_notify( &baseptr_send[0], i, MPI_UINT64_T,
					send_to, 0, i, MPI_UINT64_T, win_send, tag);
			_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
		}
	} else {

		status = foMPI_Notify_init(win_send, receive_from, tag, 1, &notification_request);
		_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

		for (i = 1; i <= num_item_to_send; i++) {
			//wait on multiple post
			status = foMPI_Start(&notification_request);
			_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
			status = foMPI_Wait(&notification_request, &notification_status);
			_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
		}
		/*check results*/
		for (i = 0; i < num_item_to_send; i++) {
			if (baseptr_send[i] != receive_from * 100000 + i) {
				error++;
				fprintf(stdout, "TEST[%d]: receive_buffer[%"PRIu64"]=%d expected=%d   \n", rank, i,
						baseptr_send[i], receive_from * 100000 + i);
			}
		}
		status = foMPI_Request_free(&notification_request);
		_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

		if (error == 0) {
			printf("No Error\n");
		} else {
			abort();
		}
	}
	status = foMPI_Win_free(&win_send);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

}
