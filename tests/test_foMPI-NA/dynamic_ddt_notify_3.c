#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rma_dynamic_ddt_notify_3(int* base, int DIM, foMPI_Win win, MPI_Comm comm) {

	int i;
	int rank;
	int commsize;
	int status;
	MPI_Aint start;

	MPI_Datatype target_dtype;

	int* send_buffer;

	MPI_Comm_rank(comm, &rank);
	MPI_Comm_size(comm, &commsize);
	int tag = 0;
	int flag = 0;
	int receive_from = commsize - rank - 1;
	int send_to = commsize - rank - 1;
	foMPI_Request notification_request;
	MPI_Status notification_status;

	status = foMPI_Notify_init(win, receive_from, tag, 1, &notification_request);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

	send_buffer = malloc(3 * DIM * sizeof(int));

	for (i = 0; i < 3 * DIM; i++) {
		send_buffer[i] = 3 * DIM * rank + i;
		base[i] = -1;
	}

	MPI_Get_address(&base[0], &start);
	MPI_Sendrecv_replace(&start, 1, MPI_AINT, commsize - rank - 1, 0, commsize - rank - 1, 0, comm,
	MPI_STATUS_IGNORE);

	MPI_Type_contiguous(3 * DIM - 1, MPI_INT, &target_dtype);
	MPI_Type_commit(&target_dtype);

	status = foMPI_Win_fence(0, win);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

	status = foMPI_Put_notify(&send_buffer[0], 3 * DIM - 1, MPI_INT, commsize - rank - 1, start, 1,
			target_dtype, win, tag);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

	status = foMPI_Start(&notification_request);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
	status = foMPI_Wait(&notification_request, &notification_status);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
	/*fence was here*/

	status = foMPI_Start(&notification_request);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
	status = foMPI_Test(&notification_request, &flag, &notification_status);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

	status = foMPI_Win_fence(0, win);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
	if (flag) {
		fprintf(stderr, "Error on rank %i in %s line %i: Multiple notification on single put.\n",
				rank, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	foMPI_Type_free(&target_dtype);

	for (i = 0; i < 3 * DIM - 1; i++) {
		if (base[i] != 3 * DIM * (commsize - rank - 1) + i) {
			fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n",
					rank, __FILE__, __LINE__, i, base[i], 3 * DIM * (commsize - rank - 1) + i);
			exit(EXIT_FAILURE);
		}
	}
	if (base[3 * DIM - 1] != -1) {
		fprintf(stderr, "Error on rank %i in %s line %i: element %i is %i instead of %i.\n", rank,
		__FILE__, __LINE__, 3 * DIM - 1, base[3 * DIM - 1], -1);
		exit(EXIT_FAILURE);
	}
	status = foMPI_Request_free(&notification_request);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

	free(send_buffer);

	if (rank == 0) {
		printf("No Error\n");
	}

	return;
}
