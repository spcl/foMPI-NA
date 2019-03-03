/*
 * test_dscqueue.c
 *
 *  Created on: Jul 14, 2014
 *      Author: Roberto Belli
 */
#include "fompi.h"
#include "test_mpi3rma.h"

void test_mpi3rmaex_xpmemqueue() {

	/*init*/
	fompi_not_oset_t * dscqueue = _fompi_not_oset_init();
	uint16_t source;
	uint16_t tag;
	int error = 0;
	int i = 0, k = 0;
	/*add*/
	for (i = 0; i < 10; i++) {
		for (k = 0; k < 3; k++) {
			_fompi_not_oset_push(dscqueue, i, k);
		}
	}

	/*remove ordered ANY source any tag*/
	for (i = 0; i < 10; i++) {
		for (k = 0; k < 3; k++) {
			if (_fompi_notif_uq_pop(dscqueue, &source, &tag) != _foMPI_NO_MATCH) {
				if (k != tag || i != source) {
					fprintf(stderr,
							"ANY TAG and SOURCE: error ordering : exp: (%d,%d) act: (%"PRIu16",%"PRIu16") \n",
							i, k, source, tag);
					error++;
				}
			} else {
				fprintf(stderr, "ANY TAG and SOURCE: error no match 1 \n");
				error++;
			}
		}
	}
	if (error) {
		abort();
	}
	/*queue empty*/
	if (_fompi_notif_uq_pop(dscqueue, &source, &tag) != _foMPI_NO_MATCH) {
		fprintf(stderr, "ANY TAG and SOURCE: error no match 2 \n");
		abort();
	}

	/*fill the queue again*/
	for (i = 0; i < 10; i++) {
		for (k = 0; k < 3; k++) {
			_fompi_notif_uq_push(dscqueue, i, k);
		}
	}

	/*specifying only the source*/
	i = 5;
	source = 5;
	for (k = 0; k < 3; k++) {
		if (_fompi_notif_uq_search_remove_src(dscqueue, source, &tag) != _foMPI_NO_MATCH) {
			if (k != tag || i != source) {
				fprintf(stderr,
						"ANY TAG : error ordering : exp: (%d,%d) act: (%"PRIu16",%"PRIu16") \n", i,
						k, source, tag);
				error++;
			}
		} else {
			fprintf(stderr, "ANY TAG: error no match 1 \n");
			error++;
		}
	}
	if (error) {
		abort();
	}
	/*queue empty*/
	if (_fompi_notif_uq_search_remove_src(dscqueue, source, &tag) != _foMPI_NO_MATCH) {
		fprintf(stderr, "ANY TAG: error no match 2 \n");
		abort();
	}

	/*specifying only the tag*/
	k = 2;
	tag = 2;
	for (i = 0; i < 10; i++) {
		if (_fompi_notif_uq_search_remove_tag(dscqueue, tag, &source) != _foMPI_NO_MATCH) {
			if (i == 5) {
				/*i==5 already removed in test before*/
				i++;
			}
			if (k != tag || i != source) {
					fprintf(stderr,
							"ANY SOUCDE : error ordering : exp: (%d,%d) act: (%"PRIu16",%"PRIu16") \n",
							i, k, source, tag);
					error++;
			}
		} else {
			fprintf(stderr, "ANY SOUCDE : error _foMPI_NO_MATCH : exp: (%d,%d)  \n", i, k);

		}
	}
	if (error) {
		abort();
	}

	/*specifying tag and source HEAD */
		source = 9;
		tag = 1;
		if (_fompi_notif_uq_search_remove(dscqueue, source, tag) == _foMPI_NO_MATCH) {
			fprintf(stderr, " error find: (%"PRIu16",%"PRIu16") \n", source, tag);
			error++;
		}
		/*now is not present anymore*/
		if (_fompi_notif_uq_search_remove(dscqueue, source, tag) != _foMPI_NO_MATCH) {
			fprintf(stderr, " error find 2: (%"PRIu16",%"PRIu16") \n", source, tag);
			error++;
		}

	/*specifying tag and source MID*/
	source = 1;
	tag = 1;
	if (_fompi_notif_uq_search_remove(dscqueue, source, tag) == _foMPI_NO_MATCH) {
		fprintf(stderr, " error find: (%"PRIu16",%"PRIu16") \n", source, tag);
		error++;
	}
	/*now is not present anymore*/
	if (_fompi_notif_uq_search_remove(dscqueue, source, tag) != _foMPI_NO_MATCH) {
		fprintf(stderr, " error find 2: (%"PRIu16",%"PRIu16") \n", source, tag);
		error++;
	}

	source = 6;
	tag = 0;
	if (_fompi_notif_uq_search_remove(dscqueue, source, tag) == _foMPI_NO_MATCH) {
		fprintf(stderr, " error find: (%"PRIu16",%"PRIu16") \n", source, tag);
		error++;
	}
	/*now is not present anymore*/
	if (_fompi_notif_uq_search_remove(dscqueue, source, tag) != _foMPI_NO_MATCH) {
		fprintf(stderr, " error find 2: (%"PRIu16",%"PRIu16") \n", source, tag);
		error++;
	}

	/*specifying tag and source TAIL*/
		source = 0;
		tag = 0;
		if (_fompi_notif_uq_search_remove(dscqueue, source, tag) == _foMPI_NO_MATCH) {
			fprintf(stderr, " error find: (%"PRIu16",%"PRIu16") \n", source, tag);
			error++;
		}
		/*now is not present anymore*/
		if (_fompi_notif_uq_search_remove(dscqueue, source, tag) != _foMPI_NO_MATCH) {
			fprintf(stderr, " error find 2: (%"PRIu16",%"PRIu16") \n", source, tag);
			error++;
		}

	if (error) {
		abort();
	}

	/*finalize*/
	_fompi_notif_uq_finalize(&dscqueue);
	if (dscqueue != NULL) {
		fprintf(stderr, "Error finalize\n");
		abort();
	}

}
