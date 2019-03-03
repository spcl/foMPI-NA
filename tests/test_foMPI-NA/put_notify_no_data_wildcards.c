/*
 * put_notify_no_data_wildcards.c
 *
 *  Created on: Jul 8, 2014
 *      Author: Roberto Belli
 */
#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rmaex_put_notify_no_data_wildcards(MPI_Comm comm) {
	int rank;
	int commsize;
	MPI_Comm_rank(comm, &rank);
	MPI_Comm_size(comm, &commsize);

	int send_to = 0;
	/* needed for measurements */
	int status;

	//INIT

	foMPI_Win win_send;
	/*stage one anytag - source*/
	foMPI_Request notification_request_s1[commsize - 1];
	/*stage two anysource - tag*/
	foMPI_Request notification_request_s2;
	/*stage three anytag anysource*/
	foMPI_Request notification_request_s3;

	MPI_Status notification_status;
	int size = 4;
	int disp_unit = 1;
	int i = 0;
	int index = 0;
	int flag = 0;

	void *baseptr_send;
	status = foMPI_Win_allocate(size, disp_unit, MPI_INFO_NULL, comm, &baseptr_send, &win_send);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

	for (i = 1; i < commsize; i++) {
		status = foMPI_Notify_init(win_send, i, foMPI_ANY_TAG, 1, &notification_request_s1[i - 1]);
		_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
	}

	status = foMPI_Notify_init(win_send, foMPI_ANY_SOURCE, 0, 1, &notification_request_s2);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

	status = foMPI_Notify_init(win_send, foMPI_ANY_SOURCE, foMPI_ANY_TAG, 1, &notification_request_s3);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

	foMPI_Win_fence(0, win_send);
	/*stage one*/
	if (rank != 0) {

		/*each PE sends a notification with tag==rank */
		status = foMPI_Put_notify(baseptr_send, 0, MPI_UINT64_T, send_to, 1, 0, MPI_UINT64_T,
				win_send, rank);
		_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

	} else {
		//rank == 0

		for (i = 1; i < commsize; i++) {
			status = foMPI_Start(&notification_request_s1[i - 1]);
			_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
		}
		for (i = 1; i < commsize; i++) {
			status = foMPI_Waitany(commsize - 1, &notification_request_s1[0], &index,
					&notification_status);
			_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
			if (notification_status.MPI_SOURCE != notification_status.MPI_TAG) {
				fprintf(stderr, "WaitAny :Wrong notification received.\n");
				abort();
			}
		}
		status = foMPI_Testany(commsize - 1, &notification_request_s1[0], &index, &flag,
				&notification_status);
		_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
		if (flag != 1 && index != MPI_UNDEFINED) {
			fprintf(stderr, "TestAny : Wrong notification received. flag = %d index = %d\n", flag,
					index);
			abort();
		}

	}

	foMPI_Win_fence(0, win_send);
	/*stage two */

	if (rank != 0) {
		/*each PE sends a notification with tag==0 */
		status = foMPI_Put_notify(baseptr_send, 0, MPI_UINT64_T, send_to, 1, 0, MPI_UINT64_T,
				win_send, 0);
		_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

	} else {
		//rank == 0

		for (i = 1; i < commsize; i++) {
			status = foMPI_Start(&notification_request_s2);
			_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
			status = foMPI_Wait(&notification_request_s2, &notification_status);
			_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
			if (0 != notification_status.MPI_TAG) {
				fprintf(stderr, "WaitAny2 :Wrong notification received.\n");
			}
		}
		status = foMPI_Test(&notification_request_s2, &flag, &notification_status);
		_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
		if (flag != 0 && index != MPI_UNDEFINED) {
			fprintf(stderr, "TestAny2 : Wrong notification received. flag = %d index = %d\n", flag,
					index);
			abort();
		}

	}

	foMPI_Win_fence(0, win_send);
	/*stage 3*/

	if (rank != 0) {

		/*each PE sends a notification with tag==rank */
		status = foMPI_Put_notify(baseptr_send, 0, MPI_UINT64_T, send_to, 1, 0, MPI_UINT64_T,
				win_send, rank);
		_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

	} else {
		//rank == 0

		for (i = 1; i < commsize; i++) {
			status = foMPI_Start(&notification_request_s3);
			_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
			status = foMPI_Wait(&notification_request_s3, &notification_status);
			_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
			if (notification_status.MPI_SOURCE != notification_status.MPI_TAG) {
				fprintf(stderr, "WaitAny3 :Wrong notification received. (r:%d - t:%d)\n",notification_status.MPI_SOURCE,notification_status.MPI_TAG);
			}
		}
		status = foMPI_Start(&notification_request_s3);
		_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
		status = foMPI_Test(&notification_request_s3, &flag, &notification_status);
		_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
		if (flag != 0) {
			fprintf(stderr, "TestAny3 : Wrong notification received. flag = %d (r:%d - t:%d) \n", flag, notification_status.MPI_SOURCE,notification_status.MPI_TAG);
			abort();
		}
		status = foMPI_Test(&notification_request_s3, &flag, &notification_status);
		_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
		if (flag != 0
				&& !(notification_status.MPI_TAG == MPI_ANY_TAG
						&& notification_status.MPI_SOURCE == MPI_ANY_SOURCE
						&& notification_status.MPI_ERROR == MPI_SUCCESS)) {
			fprintf(stderr, "TestAny3b : Wrong notification received. flag = %d (r:%d - t:%d) \n", flag, notification_status.MPI_SOURCE,notification_status.MPI_TAG);
			abort();
		}

	}

	//cleanup
	for (i = 1; i < commsize; i++) {
		status = foMPI_Request_free(&notification_request_s1[i - 1]);
		_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
	}

	status = foMPI_Request_free(&notification_request_s2);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

	status = foMPI_Request_free(&notification_request_s3);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

	status = foMPI_Win_free(&win_send);
	_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

	if (rank == 0) {
		printf("No Error\n");
	}
}
