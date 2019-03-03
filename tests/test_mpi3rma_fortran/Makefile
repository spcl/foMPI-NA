include ../../Makefile.inc

LIBS=$(LDFLAGS_TESTS)
INC=-I../../

INCLUDE_TYPE = 2 # 1 = f77 style: include 'fompif.h'
								 # 2 = f90 style: use fompi

FCFLAGS+= $(INC) $(FOMPIOPTS) -DINCLUDE_TYPE=$(INCLUDE_TYPE) -e Z
# Intel: -cpp
# Cray: -e Z
# Gnu: -cpp -fcray-pointer


OBJS = 	acc_replace_lock.o \
				acc_sum_fence.o \
				acc_sum_lock.o \
				acc_sum_pscw.o \
				allocate_c_intrinisic.o \
				allocate_cray_ptr.o \
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
				dynamic_attach_2.o \
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
				main.o \
				module_utilities.o \
				lock_exclusive_concurrency.o \
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
#				null_pointer.o \

all: main

clean:
	rm -f *.o *.i main utilities.mod

main: $(OBJS) ../../fompi.ar
	$(FC) ${FCFLAGS} ${LDFLAGS} -o $@ ${OBJS} ${LIBS}

utilities.mod module_utilities.o: module_utilities.f90
	$(FC) $(FCFLAGS) -c $<

#little hack: all other also depend on the utilities module
acc_replace_lock.o: module_utilities.o

excl_lock.o: module_utilities.o
