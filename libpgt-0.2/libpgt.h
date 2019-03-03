/*
 * libpgt.h
 *
 *  Created on: Dec 21, 2011
 *      Author: htor
 */

#ifndef LIBPGT_H_
#define LIBPGT_H_

#ifdef __cplusplus
extern "C" {
#endif

void PGT_Init(const char *projname, int autoprof_interval);
void PGT_Finalize();
void PGT_Reg_param(const char *format, ...);
void PGT_Reg_id(const char *format, ...);
void PGT_Rec(unsigned int id);
void PGT_Rec_ints(unsigned int id, int int1, int int2);
void PGT_Rec_intdbl(unsigned int id, int int1, double dbl);
void PGT_Flush();
void PGT_Res();
void PGT_Rec_enable();
void PGT_Rec_disable();


#ifdef __cplusplus
}
#endif
#endif /* LIBPGT_H_ */
