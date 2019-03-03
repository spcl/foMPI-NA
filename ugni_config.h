// Copyright (c) 2014 ETH-Zurich. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UGNI_CONFIG_H_
#define UGNI_CONFIG_H_


#define _foMPI_UGNI_MEMORY_ALIGNMENT 64
#define _foMPI_UGNI_GET_ALIGN 4 //sizeof(void*)
#define _foMPI_BIND_ID_MULTIPLIER       100

/*init configurations*/
/*default PTAG index used for creating uGNI CDM (communication Domain Manager) */
#define _foMPI_PTAG_INDEX 		1
/*device id used for binding a CDM to a specific NIC, -1 to use the nearest NIC*/
#define _foMPI_DEFAULT_DEV_ID 	-1



/*completion queue configurations*/
/*Size of completion queues SRC (used for check remote completion of locally initiated RMA operations)*/
#define _foMPI_NUM_SRC_CQ_ENTRIES 	1000
/*DST completion queue (used for checking for completion of remotely initiated RMA operations)*/
#define _foMPI_NUM_DST_CQ_ENTRIES 	10000

#define _foMPI_WAIT_EVENT_TIMEOUT 	1000
/*Completion queues parameters*/
#define _foMPI_SRC_CQ_MODE 			GNI_CQ_BLOCKING
#define _foMPI_DST_CQ_MODE  		GNI_CQ_NOBLOCK

/*notification system*/
//#define _FOMPI_REORDER_NOTIFICATIONS
//#define _FOMPI_NOTIFICATION_SOFTWARE_AGENT //not implemented yet


/*RDMA configurations*/
/*TODO: try what combination of parameters is the fastest */
/* CMDCreate: communication domain configuration*/
#define _foMPI_DEFAULT_RDMA_MODES		( GNI_CDM_MODE_DUAL_EVENTS )
/*
 * GNI_CDM_MODE_DUAL_EVENTS enables the inserction of a completion queue entry at both SRC and DST
 * GNI_CDM_MODE_FMA_SHARED enables the sharing of FMA descriptors (Usefule if more than one process is allocated on the same node)
 *
 * */

/*RDMA operations conf*/
#define _foMPI_PUT_RDMA_MODE 	 GNI_RDMAMODE_PHYS_ADDR
/*
 * GNI_RDMAMODE_PHYS_ADDR
 * or GNI_RDMAMODE_FENCE
 * */
/*threshold to switch from using FMA to BTE*/
#define _foMPI_RDMA_THRESHOLD 	( 1024 )
/*max transfer length using uGNI BTE*/
#define _foMPI_MAX_RDMA_TRANFER_LENGHT ( 4294967295 ) /*2^32-1*/


#endif /* CONFIG_H_ */
