include ../../Makefile.inc

LIBS=$(LDFLAGS_TESTS)
INC=-I../../ -I../../mpitypes/install/include/ -I../../libtopodisc/

CCFLAGS+=$(INC) $(FOMPIOPTS)

OBJS = 	acc_replace_lock.o \
				acc_sum_fence.o \
				acc_sum_lock.o \
				acc_sum_pscw.o \
				cswap_fence.o \
				cswap_lock.o \
				cswap_pscw.o \
				cswap_special_fence.o \
				cswap_special_lock.o \
				cswap_special_pscw.o \
				ddt_1.o \
				ddt_2.o \
				ddt_3.o \
				dynamic_attach_1.o \
				dynamic_attach_2.o  \
				dynamic_ddt_1.o \
				dynamic_ddt_2.o \
				dynamic_ddt_3.o \
				dynamic_ddt_4.o \
				dynamic_ddt_5.o \
				dynamic_ddt_6.o \
				excl_lock.o \
				fetch_and_op_sum_fence.o \
				fetch_and_op_sum_lock.o \
				fetch_and_op_sum_pscw.o \
				get_acc_maxloc_lock.o \
				get_acc_no_op_lock.o \
				get_acc_replace_lock.o \
				get_acc_sum_fence.o \
				get_acc_sum_lock.o \
				get_acc_sum_pscw.o \
				get_fence.o \
				get_lock.o \
				get_pscw.o \
				lock_exclusive_concurrency.o \
				main.o \
				null_pointer.o \
				pscw1.o \
				pscw2.o \
				put_fence.o \
				put_lock.o \
				put_pscw.o \
				racc_sum_lock.o \
				rget_acc_sum_lock.o \
				rget_lockall.o \
				rput_rget_lock.o \
				win_attr.o \
				win_name.o

all: main

clean:
	rm -f *.o main symmetric_heap time_win_create_free total_instructions

main: test_mpi3rma.h $(OBJS) ../../libfompi.a
	$(CC) ${CCFLAGS} ${LDFLAGS} -o $@ ${OBJS} ${LIBS}

symmetric_heap: symmetric_heap.o ../../libfompi.a
	$(CC) ${CCFLAGS} ${LDFLAGS} -o $@ symmetric_heap.o ${LIBS}

time_win_create_free: time_win_create_free.o ../../libfompi.a
	$(CC) ${CCFLAGS} ${LDFLAGS} -o $@ time_win_create_free.o ${LIBS}

total_instructions: total_instructions.o ../../libfompi.a
	$(CC) ${CCFLAGS} ${LDFLAGS} -o $@ total_instructions.o ${LIBS} -lpapi
