// Copyright (c) 2014 ETH-Zurich. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef XPMEM_CONFIG_H_
#define XPMEM_CONFIG_H_

/*init configurations*/

/*completion queue configurations*/

/*MAX Number of entries in the XPMEM notification queue (CQ) */
#define _foMPI_XPMEM_NUM_DST_CQ_ENTRIES 	1000
/* Cache line size in bytes*/
#define _foMPI_CACHE_LINE_IN_BYTES 64
/*Pages size*/
#define _foMPI_PAGE_SIZE_IN_BYTES 4096

#define _foMPI_NOTIF_FIXED_FIELDS_SIZE ( sizeof(fompi_xpmem_notif_t) )

#define _foMPI_XPMEM_NOTIF_INLINE_BUFF_SIZE ( _foMPI_CACHE_LINE_IN_BYTES - _foMPI_NOTIF_FIXED_FIELDS_SIZE )

/*notification system*/


#endif /* XPMEM_CONFIG_H_ */
