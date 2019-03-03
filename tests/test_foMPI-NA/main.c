#include <stdio.h>

#include "fompi.h"
#include "test_mpi3rma.h"

#define DIM 5

int main(int argc, char *argv[]) {

	/*
	 */
	int rank;
	int commsize;
	char create_flavor[20];
	int j;
	int i;
	foMPI_Win win;
	foMPI_Win win2;
	int* base;
	int* base2;
	float_int_t* base3;
	int64_t* base4;
	MPI_Comm comm;
	double time[3];
	int status;
	int test_counter = 0;
	int grank;
	int gcommsize;

	foMPI_Init(&argc, &argv);
	glob_info.v_option = 0;
	//TODO: remove this DEBUG ONLY

	MPI_Comm_rank( MPI_COMM_WORLD, &grank);
	MPI_Comm_size( MPI_COMM_WORLD, &gcommsize);

	if (gcommsize < 4 && (gcommsize % 2 == 0)) {
		printf("The tests need an even number (at least four) of processes . So please rerun it.\n");
		foMPI_Finalize();
		return 0;
	}
	//=============================================================================
	//========================== NULL pointer test ================================
	//=============================================================================

	if (grank == 0) {
		printf("test %3i: null pointer\n", test_counter++);
		test_mpi3rma_null_pointer(MPI_COMM_SELF);
	}

	//=============================================================================
	//====================== test notifications queue =============================
	//=============================================================================

	//	if (grank == 0) {
	//		printf("test %3i: dscqueue \n", test_counter++);
	//		test_mpi3rmaex_dscqueue();
	//		printf("No error \n");
	//	}


	//=============================================================================
	//=========================== PUT notify test =================================
	//=============================================================================
	glob_info.v_option = 0;
	if (grank == 0) {
		printf("test %3i: notify - PUT_notify\n", test_counter++);
	}
	test_mpi3rmaex_put_notify(MPI_COMM_WORLD);

	//=============================================================================
	//=========================== PUT notify test =================================
	//=============================================================================
	glob_info.v_option = 0;
	if (grank == 0) {
		printf("test %3i: notify - PUT_notify_2\n", test_counter++);
	}
	test_mpi3rmaex_put_notify_2(MPI_COMM_WORLD);
	glob_info.v_option = 0;

	//=============================================================================
	//=========================== PUT notify test =================================
	//=============================================================================
	glob_info.v_option = 0;
	if (grank == 0) {
		printf("test %3i: notify - PUT_notify_pingpong\n", test_counter++);
	}
	test_mpi3rmaex_put_notify_pingpong(MPI_COMM_WORLD);
	glob_info.v_option = 0;

	//=============================================================================
	//======================= PUT notify no data test =============================
	//=============================================================================

	if (grank == 0) {
		printf("test %3i: notify - PUT_notify no data\n", test_counter++);
	}
	test_mpi3rmaex_put_notify_no_data(MPI_COMM_WORLD);

	//=============================================================================
	//======================= PUT notify no data wildcard test =============================
	//=============================================================================
	glob_info.v_option = 0;
	if (grank == 0) {
		printf("test %3i: notify - PUT_notify no data wildcards \n", test_counter++);
	}
	test_mpi3rmaex_put_notify_no_data_wildcards(MPI_COMM_WORLD);
	glob_info.v_option = 0;
	//=============================================================================
	//=========================== GET notify test =================================
	//=============================================================================
	glob_info.v_option = 3;
	if (grank == 0) {
		printf("test %3i: notify - GET_notify\n", test_counter++);
	}
	test_mpi3rmaex_get_notify(MPI_COMM_WORLD);
	glob_info.v_option = 0;
	//=============================================================================
	//======================== Window attributes test =============================
	//=============================================================================

	if (grank == 0) {
		printf("test %3i: window attributes\n", test_counter++);
		test_mpi3rma_win_attr(MPI_COMM_SELF);
	}

	for (i = 0; i < 3; i++) {
		switch (i) {
		case 0:
			comm = MPI_COMM_WORLD;
			break;
		case 1:
			MPI_Comm_split(MPI_COMM_WORLD, grank % 2, 0, &comm);
			break;
		case 2:
			if (grank < gcommsize - 2) {
				MPI_Comm_split(MPI_COMM_WORLD, 0, 0, &comm);
			} else {
				MPI_Comm_split(MPI_COMM_WORLD, 1, 0, &comm);
			}
			break;
		}

		MPI_Comm_rank(comm, &rank);
		MPI_Comm_size(comm, &commsize);

		//=============================================================================
		//===================== bookkeeping exclusive locks test ======================
		//=============================================================================

		if (rank == 0) {
			printf("test %3i: bookkeeping exclusive locks\n", test_counter++);
		}
		test_mpi3rma_excl_lock(comm);

		for (j = 0; j < 3; j++) {
			time[j] = -MPI_Wtime();

			switch (j) {
			case 0:
				base = malloc(3 * DIM * sizeof(int));
				status = foMPI_Win_create(&base[5], DIM * sizeof(int), sizeof(int), MPI_INFO_NULL,
						MPI_COMM_WORLD, &win2);
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
				status = foMPI_Win_create(base, 3 * DIM * sizeof(int), sizeof(int), MPI_INFO_NULL,
						comm, &win);
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
				snprintf(create_flavor, 20, "create");
				break;
			case 1:
				status = foMPI_Win_allocate(3 * DIM * sizeof(int), sizeof(int), MPI_INFO_NULL, comm,
						&base, &win);
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
				status = foMPI_Win_allocate( DIM * sizeof(int), sizeof(int), MPI_INFO_NULL,
						MPI_COMM_WORLD, &base2, &win2);
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
				snprintf(create_flavor, 20, "allocate");
				break;
			case 2:
				status = foMPI_Win_create_dynamic( MPI_INFO_NULL, MPI_COMM_WORLD, &win2);
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
				status = foMPI_Win_create_dynamic( MPI_INFO_NULL, comm, &win);
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

				base = malloc(3 * DIM * sizeof(int));
				foMPI_Win_attach(win2, base, 2 * DIM * sizeof(int));
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

				MPI_Barrier( MPI_COMM_WORLD);

				snprintf(create_flavor, 20, "dynamic");
			}

			//=============================================================================
			//=========================== window name test ================================
			//=============================================================================

			if (rank == 0) {
				printf("test %3i: %s - win name\n", test_counter++, create_flavor);
				test_mpi3rma_win_name(win2);
			}

			if (j == 2) {
				//=============================================================================
				//========================= dynamic attach 1 test =============================
				//=============================================================================

				if (rank == 0) {
					printf("test %3i: %s - attach 1\n", test_counter++, create_flavor);
				}
				test_mpi3rma_dynamic_attach_1( DIM, win, comm);

				//=============================================================================
				//========================= dynamic attach 2 test =============================
				//=============================================================================

				if (rank == 0) {
					printf("test %3i: %s - attach 2\n", test_counter++, create_flavor);
				}
				test_mpi3rma_dynamic_attach_2( DIM, win, comm);

			}

			//=============================================================================
			//============================ put fence test =================================
			//=============================================================================

			if (rank == 0) {
				printf("test %3i: %s - Put (fence)\n", test_counter++, create_flavor);
			}
			if (j == 2) {
				test_mpi3rma_put_fence(base, DIM, win2, MPI_COMM_WORLD);
			} else {
				test_mpi3rma_put_fence(base, DIM, win, comm);
			}

			//=============================================================================
			//============================ get fence test =================================
			//=============================================================================

			if (j == 2) {
				status = foMPI_Win_detach(win2, base);
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
				status =  foMPI_Win_attach(win2, base, 3 * DIM * sizeof(int));
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
				MPI_Barrier( MPI_COMM_WORLD);
			}

			if (rank == 0) {
				printf("test %3i: %s - Get (fence)\n", test_counter++, create_flavor);
			}
			if (j == 2) {
				test_mpi3rma_get_fence(base, DIM, win2, MPI_COMM_WORLD);
			} else {
				test_mpi3rma_get_fence(base, DIM, win, comm);
			}

			//=============================================================================
			//============================ get lock test ==================================
			//=============================================================================

			if (j == 2) {
				status = foMPI_Win_detach(win2, base);
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

				status = foMPI_Win_attach(win, &base[0], DIM * sizeof(int));
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
				status = foMPI_Win_attach(win, &base[DIM], DIM * sizeof(int));
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
				status = foMPI_Win_attach(win, &base[2 * DIM], DIM * sizeof(int));
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

				MPI_Barrier(comm);
			}

			MPI_Barrier(comm); /* synchronisation mode change */

			if (rank == 0) {
				printf("test %3i: %s - Get (lock)\n", test_counter++, create_flavor);
			}
			test_mpi3rma_get_lock(base, DIM, win, comm);

			//=============================================================================
			//============================ put lock test ==================================
			//=============================================================================

			if (rank == 0) {
				printf("test %3i: %s - Put (lock)\n", test_counter++, create_flavor);
			}
			test_mpi3rma_put_lock(base, DIM, win, comm);

			//=============================================================================
			//=========== rput with mpi_test & rget with mpi_wait (lock) test =============
			//=============================================================================

			if (rank == 0) {
				printf("test %3i: %s - Rput/Test & Rget/Wait (lock)\n", test_counter++,
						create_flavor);
			}
			test_mpi3rma_rput_rget_lock(base, DIM, win, comm);

			//=============================================================================
			//==================== rget with mpi_test (lockall) test ======================
			//=============================================================================

			if (rank == 0) {
				printf("test %3i: %s - Rget/Test (lock_all)\n", test_counter++, create_flavor);
			}
			test_mpi3rma_rget_lockall(base, DIM, win, comm);

			//=============================================================================
			//===================== lock exclusive concurrency test =======================
			//=============================================================================

			if (rank == 0) {
				printf("test %3i: %s - Exclusive lock concurrency\n", test_counter++,
						create_flavor);
			}
			test_mpi3rma_lock_exclusive_concurrency(base, DIM, win, comm);

			//=============================================================================
			//============================ put pscw test ==================================
			//=============================================================================

			MPI_Barrier(comm); /* synchronisation mode change */

			if (rank == 0) {
				printf("test %3i: %s - Put (PCSW)\n", test_counter++, create_flavor);
			}
			test_mpi3rma_put_pscw(base, DIM, win, comm);

			//=============================================================================
			//============================ get pscw test ==================================
			//=============================================================================

			if (rank == 0) {
				printf("test %3i: %s - Get (PSCW)\n", test_counter++, create_flavor);
			}
			test_mpi3rma_get_pscw(base, DIM, win, comm);

			//=============================================================================
			//============================== pscw test ====================================
			//=============================================================================
			// This tests if a non-blocking FOMPI_Win_start is working.
			// We choose that FOMPI_Win_start is always blocking, so this test is removed for now.
			//
			//      if (rank == 0) {
			//        printf("test %3i: %s - PSCW 1\n", test_counter++, create_flavor);
			//      }
			//      test_mpi3rma_pscw1(win, comm);

			//=============================================================================
			//============================== pscw test ====================================
			//=============================================================================

			if (rank == 0) {
				printf("test %3i: %s - PSCW 2/Win_test\n", test_counter++, create_flavor);
			}
			test_mpi3rma_pscw2(win, comm);

			//=============================================================================
			//====================== accumulate sum fence test ============================
			//=============================================================================

			MPI_Barrier(comm); /* synchronisation mode change */

			if (rank == 0) {
				printf("test %3i: %s - Accumulate/SUM (Fence)\n", test_counter++, create_flavor);
			}
			test_mpi3rma_acc_sum_fence(base, DIM, win, comm);

			//=============================================================================
			//====================== accumulate sum pscw test =============================
			//=============================================================================

			MPI_Barrier(comm); /* synchronisation mode change */

			if (rank == 0) {
				printf("test %3i: %s - Accumulate/SUM (PSCW)\n", test_counter++, create_flavor);
			}
			test_mpi3rma_acc_sum_pscw(base, DIM, win, comm);

			//=============================================================================
			//====================== accumulate sum lock test =============================
			//=============================================================================

			MPI_Barrier(comm); /* synchronisation mode change */

			if (rank == 0) {
				printf("test %3i: %s - Accumulate/SUM (Lock)\n", test_counter++, create_flavor);
			}
			test_mpi3rma_acc_sum_lock(base, DIM, win, comm);

			//=============================================================================
			//==================== accumulate replace lock test ===========================
			//=============================================================================

			if (rank == 0) {
				printf("test %3i: %s - Accumulate/REPLACE (Lock)\n", test_counter++, create_flavor);
			}
			test_mpi3rma_acc_replace_lock(base, DIM, win, comm);

			//=============================================================================
			//====================== raccumulate sum lock test ============================
			//=============================================================================

			if (rank == 0) {
				printf("test %3i: %s - Raccumulate/SUM (lock)\n", test_counter++, create_flavor);
			}
			test_mpi3rma_racc_sum_lock(base, DIM, win, comm);

			//=============================================================================
			//==================== get_accumulate sum fence test ==========================
			//=============================================================================

			MPI_Barrier(comm); /* synchronisation mode change */

			if (rank == 0) {
				printf("test %3i: %s - Get_accumulate/SUM (fence)\n", test_counter++,
						create_flavor);
			}
			test_mpi3rma_get_acc_sum_fence(base, DIM, win, comm);

			//=============================================================================
			//==================== get_accumulate sum PSCW test ===========================
			//=============================================================================

			MPI_Barrier(comm); /* synchronisation mode change */

			if (rank == 0) {
				printf("test %3i: %s - Get_accumulate/SUM (PSCW)\n", test_counter++, create_flavor);
			}
			test_mpi3rma_get_acc_sum_pscw(base, DIM, win, comm);

			//=============================================================================
			//==================== get_accumulate sum lock test ===========================
			//=============================================================================

			MPI_Barrier(comm); /* synchronisation mode change */

			if (rank == 0) {
				printf("test %3i: %s - Get_accumulate/SUM (lock)\n", test_counter++, create_flavor);
			}
			test_mpi3rma_get_acc_sum_lock(base, DIM, win, comm);

			//=============================================================================
			//================== get_accumulate replace lock test =========================
			//=============================================================================

			if (rank == 0) {
				printf("test %3i: %s - Get_accumulate/REPLACE (lock)\n", test_counter++,
						create_flavor);
			}
			test_mpi3rma_get_acc_replace_lock(base, DIM, win, comm);

			//=============================================================================
			//=================== get_accumulate no_op lock test ==========================
			//=============================================================================

			if (rank == 0) {
				printf("test %3i: %s - Get_accumulate/NO_OP (lock)\n", test_counter++,
						create_flavor);
			}
			test_mpi3rma_get_acc_no_op_lock(base, DIM, win, comm);

			//=============================================================================
			//=================== rget_accumulate sum lock test ===========================
			//=============================================================================

			if (rank == 0) {
				printf("test %3i: %s - Rget_accumulate/SUM (lock)\n", test_counter++,
						create_flavor);
			}
			test_mpi3rma_rget_acc_sum_lock(base, DIM, win, comm);

			//=============================================================================
			//==================== compare_and_swap fence test ============================
			//=============================================================================

			MPI_Barrier(comm); /* synchronisation mode change */

			if (rank == 0) {
				printf("test %3i: %s - Compare_and_swap (fence)\n", test_counter++, create_flavor);
			}
			test_mpi3rma_cswap_fence(base, win, comm);

			//=============================================================================
			//==================== compare_and_swap PSCW test =============================
			//=============================================================================

			MPI_Barrier(comm); /* synchronisation mode change */

			if (rank == 0) {
				printf("test %3i: %s - Compare_and_swap (PSCW)\n", test_counter++, create_flavor);
			}
			test_mpi3rma_cswap_pscw(base, win, comm);

			//=============================================================================
			//==================== compare_and_swap lock test =============================
			//=============================================================================

			MPI_Barrier(comm); /* synchronisation mode change */

			if (rank == 0) {
				printf("test %3i: %s - Compare_and_swap (lock)\n", test_counter++, create_flavor);
			}
			test_mpi3rma_cswap_lock(base, win, comm);

			//=============================================================================
			//==================== fetch_and_op sum fence test ============================
			//=============================================================================

			MPI_Barrier(comm); /* synchronisation mode change */

			if (rank == 0) {
				printf("test %3i: %s - Fetch_and_op/SUM (fence)\n", test_counter++, create_flavor);
			}
			test_mpi3rma_fetch_and_op_sum_fence(base, win, comm);

			//=============================================================================
			//==================== fetch_and_op sum PSCW test =============================
			//=============================================================================

			MPI_Barrier(comm); /* synchronisation mode change */

			if (rank == 0) {
				printf("test %3i: %s - Fetch_and_op/SUM (PSCW)\n", test_counter++, create_flavor);
			}
			test_mpi3rma_fetch_and_op_sum_pscw(base, win, comm);

			//=============================================================================
			//==================== fetch_and_op sum lock test =============================
			//=============================================================================

			MPI_Barrier(comm); /* synchronisation mode change */

			if (rank == 0) {
				printf("test %3i: %s - Fetch_and_op/SUM (lock)\n", test_counter++, create_flavor);
			}
			test_mpi3rma_fetch_and_op_sum_lock(base, win, comm);

			//=============================================================================
			//======================== MPI Datatype test 1 ================================
			//=============================================================================

			//change the memory regions for all datatype tests
			if (j == 2) {
				//TODO: remove this DEBUG ONLY
				glob_info.v_option = 0;

				status = foMPI_Win_detach(win, &base[0]);
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
				status = foMPI_Win_detach(win, &base[DIM]);
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
				status = foMPI_Win_detach(win, &base[2 * DIM]);
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

				status = foMPI_Win_attach(win, &base[0], 3 * DIM * sizeof(int));
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

				MPI_Barrier(comm);
			}

			MPI_Barrier(comm); /* synchronisation mode change */

			if (rank == 0) {
				printf("test %3i: %s - Put with DDTs (in one memory region) (fence)\n",
						test_counter++, create_flavor);
			}

			if (DIM >= 5) {
				test_mpi3rma_ddt_1(base, win, comm);
			} else {
				printf("Memory region is too small for the test. DIM has to be at least 5.\n");
			}

			//=============================================================================
			//======================== MPI Datatype test 2 ================================
			//=============================================================================

			if (rank == 0) {
				printf("test %3i: %s - Accumulate/SUM with DDTs (in one memory region) (fence)\n",
						test_counter++, create_flavor);
			}

			if (DIM >= 5) {
				test_mpi3rma_ddt_2(base, win, comm);
			} else {
				printf("Memory region is too small for the test. DIM has to be at least 5.\n");
			}

			//=============================================================================
			//======================== MPI Datatype test 3 ================================
			//=============================================================================

			if (rank == 0) {
				printf("test %3i: %s - Get_accumulate/SUM with DDTs (fence)\n", test_counter++,
						create_flavor);
			}

			if (DIM >= 5) {
				test_mpi3rma_ddt_3(base, win, comm);
			} else {
				printf("Memory region is too small for the test. DIM has to be at least 5.\n");
			}

			//preparations for the next test
			if (j == 2) {
				status = foMPI_Win_detach(win, &base[0]);
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

				status = foMPI_Win_attach(win, &base[0], DIM * sizeof(int));
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
				status = foMPI_Win_attach(win, &base[2 * DIM], DIM * sizeof(int));
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

				MPI_Barrier(comm);
			}

			//=============================================================================
			//==================== dynamic MPI Datatype test 1 ============================
			//=============================================================================

			if (j == 2) {
				if (rank == 0) {
					printf(
							"test %3i: %s - Put with DDTs (in more than one memory region) (fence)\n",
							test_counter++, create_flavor);
				}

				if (DIM == 5) {
					test_mpi3rma_dynamic_ddt_1(base, win, comm);
				} else {
					printf("DIM has to be 5 to run this test.\n");
				}
				//=============================================================================
				//================ dynamic MPI Datatype test 1 notify =========================
				//=============================================================================

				if (rank == 0) {
					printf(
							"test %3i: %s - Put notify with DDTs (in more than one memory region) (notify)\n",
							test_counter++, create_flavor);
				}

				if (DIM == 5) {
					test_mpi3rma_dynamic_ddt_notify_1(base, win, comm);
				} else {
					printf("DIM has to be 5 to run this test.\n");
				}

				//=============================================================================
				//================ dynamic MPI Datatype test 1 notify =========================
				//=============================================================================

				if (rank == 0) {
					printf(
							"test %3i: %s - Get notify with DDTs (in more than one memory region) (notify)\n",
							test_counter++, create_flavor);
				}

				if (DIM == 5) {
					test_mpi3rma_dynamic_ddt_get_notify_1(base, win, comm);
				} else {
					printf("DIM has to be 5 to run this test.\n");
				}

				//=============================================================================
				//==================== dynamic MPI Datatype test 4 ============================
				//=============================================================================

				if (rank == 0) {
					printf(
							"test %3i: %s - Accumulate/SUM with DDTs (in more than one memory region) (fence)\n",
							test_counter++, create_flavor);
				}

				if (DIM == 5) {
					test_mpi3rma_dynamic_ddt_4(base, win, comm);
				} else {
					printf("DIM has to be 5 to run this test.\n");
				}

				/* revert back to the original state */
				status = foMPI_Win_attach(win, &base[DIM], DIM * sizeof(int));
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

				MPI_Barrier(comm);

				//=============================================================================
				//==================== dynamic MPI Datatype test 2 ============================
				//=============================================================================

				if (rank == 0) {
					printf(
							"test %3i: %s - Put with DDTs (in more than one memory region/contiguous blocks aren't aligned with memory regions boundaries) (fence)\n",
							test_counter++, create_flavor);
				}

				if (DIM == 5) {
					test_mpi3rma_dynamic_ddt_2(base, win, comm);
				} else {
					printf("DIM has to be 5 to run this test.\n");
				}

				//=============================================================================
				//================= dynamic MPI Datatype test 2 notify ========================
				//=============================================================================

				if (rank == 0) {
					printf(
							"test %3i: %s - Put notify with DDTs (in more than one memory region/contiguous blocks aren't aligned with memory regions boundaries) (notify)\n",
							test_counter++, create_flavor);
				}

				if (DIM == 5) {
					test_mpi3rma_dynamic_ddt_notify_2(base, win, comm);
				} else {
					printf("DIM has to be 5 to run this test.\n");
				}
				//=============================================================================
				//================= dynamic MPI Datatype test 2 notify ========================
				//=============================================================================

				if (rank == 0) {
					printf(
							"test %3i: %s - Get notify with DDTs (in more than one memory region/contiguous blocks aren't aligned with memory regions boundaries) (notify)\n",
							test_counter++, create_flavor);
				}

				if (DIM == 5) {
					test_mpi3rma_dynamic_ddt_get_notify_2(base, win, comm);
				} else {
					printf("DIM has to be 5 to run this test.\n");
				}

				//=============================================================================
				//==================== dynamic MPI Datatype test 3 ============================
				//=============================================================================

				if (rank == 0) {
					printf(
							"test %3i: %s - Put (contiguous over more than one memory regions) (fence)\n",
							test_counter++, create_flavor);
				}

				/* only useful, if there is more than one memory region */
				test_mpi3rma_dynamic_ddt_3(base, DIM, win, comm);

				//=============================================================================
				//================ dynamic MPI Datatype test 3  notify=========================
				//=============================================================================

				if (rank == 0) {
					printf(
							"test %3i: %s - Put notify (contiguous over more than one memory regions) (notify)\n",
							test_counter++, create_flavor);
				}

				/* only useful, if there is more than one memory region */
				test_mpi3rma_dynamic_ddt_notify_3(base, DIM, win, comm);

				//=============================================================================
				//==================== dynamic MPI Datatype test 5 ============================
				//=============================================================================

				if (rank == 0) {
					printf(
							"test %3i: %s - Accumulate/SUM with DDTs (in more than one memory region/contiguous blocks aren't aligned with memory regions boundaries) (fence)\n",
							test_counter++, create_flavor);
				}

				if (DIM == 5) {
					test_mpi3rma_dynamic_ddt_5(base, win, comm);
				} else {
					printf("DIM has to be 5 to run this test.\n");
				}

				//=============================================================================
				//==================== dynamic MPI Datatype test 6 ============================
				//=============================================================================

				if (rank == 0) {
					printf(
							"test %3i: %s - Accumulate/SUM (contiguous over more than one memory regions) (fence)\n",
							test_counter++, create_flavor);
				}

				/* only useful, if there is more than one memory region */
				test_mpi3rma_dynamic_ddt_6(base, DIM, win, comm);
			}

			if (j < 2) {
				status = foMPI_Win_free(&win);
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
			}
			status = foMPI_Win_free(&win2);
			_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

			if (j == 0) {
				free(base);
			}

			//=============================================================================
			//==================== get_accumulate maxloc lock test ========================
			//=============================================================================

			// for this kind of test we need a different kind of window
			switch (j) {
			case 0:
				base3 = malloc( DIM * sizeof(float_int_t));
				status = foMPI_Win_create(&base3[0], DIM * sizeof(float_int_t), sizeof(float_int_t),
						MPI_INFO_NULL, comm, &win);
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
				break;
			case 1:
				status = foMPI_Win_allocate( DIM * sizeof(float_int_t), sizeof(float_int_t),
						MPI_INFO_NULL, comm, &base3, &win);
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
				break;
			case 2:
				base3 = malloc( DIM * sizeof(float_int_t));
				status = foMPI_Win_attach(win, base3, DIM * sizeof(float_int_t));
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

				MPI_Barrier(comm);
				break;
			}

			if (rank == 0) {
				printf("test %3i: %s - Get_accumulate/MAXLOC (lock)\n", test_counter++,
						create_flavor);
			}
			test_mpi3rma_get_acc_maxloc_lock(base3, DIM, win, comm);

			if (j < 2) {
				status = foMPI_Win_free(&win);
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
			}

			if (j == 0) {
				free(base3);
			}

			//=============================================================================
			//================== compare_and_swap/special fence test ======================
			//=============================================================================

			MPI_Barrier(comm); /* synchronisation mode change */

			// for this kind of test we need a different kind of window
			switch (j) {
			case 0:
				base4 = malloc(sizeof(int64_t));
				status = foMPI_Win_create(&base4[0], sizeof(int64_t), sizeof(int64_t),
						MPI_INFO_NULL, comm, &win);
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
				break;
			case 1:
				status = foMPI_Win_allocate(sizeof(int64_t), sizeof(int64_t), MPI_INFO_NULL, comm,
						&base4, &win);
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);
				break;
			case 2:
				base4 = malloc(sizeof(int64_t));
				status = foMPI_Win_attach(win, base4, sizeof(int64_t));
				_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

				MPI_Barrier(comm);
				break;
			}

			if (rank == 0) {
				printf("test %3i: %s - Compare_and_swap/special (fence)\n", test_counter++,
						create_flavor);
			}
			test_mpi3rma_cswap_special_fence(base4, win, comm);

			//=============================================================================
			//================== compare_and_swap/special pscw test =======================
			//=============================================================================

			MPI_Barrier(comm); /* synchronisation mode change */

			if (rank == 0) {
				printf("test %3i: %s - Compare_and_swap/special (PSCW)\n", test_counter++,
						create_flavor);
			}
			test_mpi3rma_cswap_special_pscw(base4, win, comm);

			//=============================================================================
			//================== compare_and_swap/special lock test =======================
			//=============================================================================

			MPI_Barrier(comm); /* synchronisation mode change */

			if (rank == 0) {
				printf("test %3i: %s - Compare_and_swap/special (lock)\n", test_counter++,
						create_flavor);
			}
			test_mpi3rma_cswap_special_lock(base4, win, comm);

			status = foMPI_Win_free(&win);
			_check_fompi_status(status, MPI_SUCCESS, (char*) __FILE__, __LINE__);

			if (j != 1) {
				free(base4);
				if (j == 2) {
					free(base3);
					free(base);
				}
			}

			time[j] += MPI_Wtime();
		}

		if (grank == 0) {
			for (j = 0; j < 3; j++) {
				printf("%i,%i: %20.4f\n",i , j, time[j] * 1000000);
			}
		}

		if (i != 0) {
			MPI_Comm_free(&comm);
		}

	}

	foMPI_Finalize();
	return 0;
}
