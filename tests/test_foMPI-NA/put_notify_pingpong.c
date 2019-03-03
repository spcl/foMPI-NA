/*
 * put_notification.c
 *
 *  Created on: Jul 8, 2014
 *      Author: Roberto Belli
 */
#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rmaex_put_notify_pingpong(MPI_Comm comm) {
	int rank, commsize;
	MPI_Comm_rank(comm, &rank);
	MPI_Comm_size(comm, &commsize);

	int first_sender = (rank % 2 == 0) ? 1 : 0;
	int receive_from = (first_sender) ? rank + 1 : rank - 1;
	int send_to = (first_sender) ? rank + 1 : rank - 1;

	foMPI_Request notification_request;
	foMPI_Win win_send;
	MPI_Status notification_status;
	int num_item_to_send = 64;
	int tot_item = num_item_to_send * 2;
	int size = tot_item * sizeof(uint64_t);	//byte
	int disp_unit = sizeof(uint64_t);
	int tag = 0;
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
	status = foMPI_Notify_init(win_send, receive_from, tag, 1, &notification_request);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
	foMPI_Win_fence(0, win_send);
	if (first_sender) {
		status = foMPI_Put_notify(&baseptr_send[0], num_item_to_send, MPI_UINT64_T, send_to, 0,
				num_item_to_send, MPI_UINT64_T, win_send, tag);
		foMPI_Win_flush_all(win_send);
		_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
		status = foMPI_Start(&notification_request);
		_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
		status = foMPI_Wait(&notification_request, &notification_status);
		_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

		/*check results*/
		for (i = 0; i < num_item_to_send; i++) {
			if (baseptr_send[i] != baseptr_send[num_item_to_send + i]) {
				error++;
				fprintf(stdout, "TEST[%d]: receive_buffer[%d]=%"PRIu64" expected=%"PRIu64"   \n", rank, i,
						 baseptr_send[num_item_to_send + i], baseptr_send[i]);
			}
		}
		if (error == 0) {
			printf("No Error\n");
		} else {
			abort();
		}
	} else {

		status = foMPI_Start(&notification_request);
		_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
		status = foMPI_Wait(&notification_request, &notification_status);
		_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
		status = foMPI_Put_notify(&baseptr_send[0], num_item_to_send, MPI_UINT64_T, send_to,
				num_item_to_send , num_item_to_send, MPI_UINT64_T, win_send, tag);
		foMPI_Win_flush_all(win_send);

	}
	status = foMPI_Request_free(&notification_request);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
	status = foMPI_Win_free(&win_send);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

}
