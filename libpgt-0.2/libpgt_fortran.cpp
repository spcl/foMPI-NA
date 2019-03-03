/*
 * libpgt_fortran.cpp
 *
 *  Created on: Dec 18, 2011
 *      Author: htor
 */

#include "libpgt_internal.hpp"

// TODO: should be auto-generated like in libnbc!
#define F77_FUNC_(name,NAME) name ## _

extern "C" { void F77_FUNC_(pgt_init,PGT_INIT)(const char *projname, int *length, int *autoprof_interval); }
void F77_FUNC_(pgt_init,PGT_INIT)(const char *projname, int *length, int *autoprof_interval) {

  char *c=(char*)calloc((*length+1)*sizeof(char), 1);
  memcpy(c, projname, *length);

  PGT_Init(c, *autoprof_interval);

  free(c);
}

extern "C" { void F77_FUNC_(pgt_rec,PGT_REC)(unsigned int *id); }
void F77_FUNC_(pgt_rec,PGT_REC)(unsigned int *id) {
   PGT_Rec(*id);
}

extern "C" { void F77_FUNC_(pgt_rec_ints,PGT_REC_INTS)(unsigned int *id, int *int1, int *int2); }
void F77_FUNC_(pgt_rec_ints,PGT_REC_INTS)(unsigned int *id, int *int1, int *int2) {
   PGT_Rec_ints(*id, *int1, *int2);
}

extern "C" { void F77_FUNC_(pgt_rec_intdbl,PGT_REC_INTDBL)(unsigned int *id, int *int1, double *dbl); }
void F77_FUNC_(pgt_rec_intdbl,PGT_REC_INTDBL)(unsigned int *id, int *int1, double *dbl) {
   PGT_Rec_intdbl(*id, *int1, *dbl);
}


extern "C" { void F77_FUNC_(pgt_reg_param_int,PGT_REG_PARAM_INT)(char *name, int *length, int *value); }
void F77_FUNC_(pgt_reg_param_int,PGT_REG_PARAM_INT)(char *name, int *length, int *value) {
  char *c=(char*)calloc((*length+1)*sizeof(char), 1);
  memcpy(c, name, *length);

  PGT_Reg_param("%s = %i", c, *value);

  free(c);

}

extern "C" { void F77_FUNC_(pgt_reg_id,PGT_REG_ID)(char *name, int *length, int *value); }
void F77_FUNC_(pgt_reg_id,PGT_REG_ID)(char *name, int *length, int *value) {
  char *c=(char*)calloc((*length+1)*sizeof(char), 1);
  memcpy(c, name, *length);

  PGT_Reg_id("%s = %i", c, *value);

  free(c);

}

extern "C" { void F77_FUNC_(pgt_flush,PGT_FLUSH)(void); }
void F77_FUNC_(pgt_flush,PGT_FLUSH)(void) { PGT_Flush(); }

extern "C" { void F77_FUNC_(pgt_finalize,PGT_FINALIZE)(void); }
void F77_FUNC_(pgt_finalize,PGT_FINALIZE)(void) { PGT_Finalize(); }

extern "C" { void F77_FUNC_(pgt_res,PGT_RES)(void); }
void F77_FUNC_(pgt_res,PGT_RES)(void) { PGT_Res(); }

extern "C" { void F77_FUNC_(pgt_rec_enable,PGT_REC_ENABLE)(void); }
void F77_FUNC_(pgt_rec_enable,PGT_REC_ENABLE)(void) { PGT_Rec_enable(); }

extern "C" { void F77_FUNC_(pgt_rec_disable,PGT_REC_DISABLE)(void); }
void F77_FUNC_(pgt_rec_disable,PGT_REC_DISABLE)(void) { PGT_Rec_disable(); }
