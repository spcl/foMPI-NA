/*
 * put_notification.c
 *
 *  Created on: Jul 8, 2014
 *      Author: Roberto Belli
 */
#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rmaex_put_notify_no_data(MPI_Comm comm) {
	int rank, commsize;
	MPI_Comm_rank(comm, &rank);
	MPI_Comm_size(comm, &commsize);

	int first_sender = (rank % 2 == 0) ? 1 : 0;
	int receive_from = (first_sender) ? rank + 1 : rank - 1;
	int send_to = (first_sender) ? rank + 1 : rank - 1;
	/* needed for measurements */
	int test_count = 1;
	int status;

	//INIT

	foMPI_Win win_send;
	foMPI_Request notification_request;
	MPI_Status notification_status;
	int size = 4;
	//int num_item64 = size / sizeof(uint64_t);
	int disp_unit = 1;
	int tag = 0;
	int test = 0;

	void *baseptr_send;
	status = foMPI_Win_allocate(size, disp_unit, MPI_INFO_NULL, comm, &baseptr_send, &win_send);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

	//this notification waits for send_buffer_elem transfers

	status = foMPI_Notify_init(win_send, receive_from, tag, 1, &notification_request);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
	foMPI_Win_fence(0,win_send);
	if (first_sender) {

		for (test = -1 /* 1 warmup test */; test < test_count; test++) {

			status = foMPI_Put_notify(baseptr_send, 0, MPI_UINT64_T, send_to, 1, 0, MPI_UINT64_T,
					win_send, tag);
			_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
			status = foMPI_Start(&notification_request);
			_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
			status = foMPI_Wait(&notification_request, &notification_status);
			_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

			/* TODO: check received data */

		}

	} else {
		//other

		for (test = -1 /* 1 warmup test */; test < test_count; test++) {
			status = foMPI_Start(&notification_request);
			_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
			status = foMPI_Wait(&notification_request, &notification_status);
			_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
			status = foMPI_Put_notify(baseptr_send, 0, MPI_UINT64_T, send_to, 1, 0, MPI_UINT64_T,
					win_send, tag);
			_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

		}
	}

	//cleanup
	status = foMPI_Request_free(&notification_request);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
	status = foMPI_Win_free(&win_send);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

	if (rank == 0) {
		printf("No Error\n");
	}
}
