include ../../Makefile.inc

LIBS=$(LDFLAGS_TESTS)
INC=-I../../

CCFLAGS+=$(INC)

EXE = accfence2 \
			accfence2_am \
			contention_put \
			contention_putget \
			fetchandadd \
			fetchandadd_am \
			fetchandadd_tree \
			fetchandadd_tree_am \
			getgroup \
			lockcontention \
			lockcontention2 \
			lockcontention3 \
			locknull \
			lockopts \
			mixedsync \
			nullpscw \
			put_base \
			put_bottom \
			selfrma \
			strided_acc_indexed \
			strided_acc_onelock \
			strided_acc_subarray \
			strided_get_indexed \
			strided_putget_indexed \
			test1 \
			test1_am \
			test1_dt \
			test2 \
			test2_am \
			test3 \
			test3_am \
			test4 \
			test4_am \
			test5 \
			test5_am \
			transpose1 \
			transpose2 \
			transpose3 \
			transpose4 \
			transpose5 \
			transpose6 \
			transpose7 \
			window_creation \
			wintest
#			ircpi 

# some general rules
all: ${EXE}

clean:
	rm -f *.o ${EXE} ircpi

run: ${EXE}
	./run.sh

accfence2: accfence2.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ accfence2.o mtest.o ${LIBS}

accfence2_am: accfence2_am.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ accfence2_am.o mtest.o ${LIBS}

contention_put: contention_put.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ contention_put.o mtest.o ${LIBS}

contention_putget: contention_putget.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ contention_putget.o mtest.o ${LIBS}

fetchandadd: fetchandadd.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ fetchandadd.o mtest.o ${LIBS}

fetchandadd_am: fetchandadd_am.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ fetchandadd_am.o mtest.o ${LIBS}

fetchandadd_tree: fetchandadd_tree.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ fetchandadd_tree.o mtest.o ${LIBS}

fetchandadd_tree_am: fetchandadd_tree_am.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ fetchandadd_tree_am.o mtest.o ${LIBS}

getgroup: getgroup.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ getgroup.o mtest.o ${LIBS}

ircpi: ircpi.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ ircpi.o mtest.o ${LIBS}

lockcontention: lockcontention.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ lockcontention.o mtest.o ${LIBS}

lockcontention2: lockcontention2.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ lockcontention2.o mtest.o ${LIBS}

lockcontention3: lockcontention3.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ lockcontention3.o mtest.o ${LIBS}

locknull: locknull.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ locknull.o mtest.o ${LIBS}

lockopts: lockopts.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ lockopts.o mtest.o ${LIBS}

mixedsync: mixedsync.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ mixedsync.o mtest.o ${LIBS}

nullpscw: nullpscw.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ nullpscw.o mtest.o ${LIBS}

put_base: put_base.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ put_base.o mtest.o ${LIBS}

put_bottom: put_bottom.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ put_bottom.o mtest.o ${LIBS}

selfrma: selfrma.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ selfrma.o mtest.o ${LIBS}

strided_acc_indexed: strided_acc_indexed.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ strided_acc_indexed.o mtest.o ${LIBS}

strided_acc_onelock: strided_acc_onelock.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ strided_acc_onelock.o mtest.o ${LIBS}

strided_acc_subarray: strided_acc_subarray.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ strided_acc_subarray.o mtest.o ${LIBS}

strided_get_indexed: strided_get_indexed.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ strided_get_indexed.o mtest.o ${LIBS}

strided_putget_indexed: strided_putget_indexed.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ strided_putget_indexed.o mtest.o ${LIBS}

test1: test1.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ test1.o mtest.o ${LIBS}

test1_dt: test1_dt.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ test1_dt.o mtest.o ${LIBS}

test1_am: test1_am.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ test1_am.o mtest.o ${LIBS}

test2: test2.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ test2.o mtest.o ${LIBS}

test2_am: test2_am.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ test2_am.o mtest.o ${LIBS}

test3: test3.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ test3.o mtest.o ${LIBS}

test3_am: test3_am.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ test3_am.o mtest.o ${LIBS}

test4: test4.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ test4.o mtest.o ${LIBS}

test4_am: test4_am.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ test4_am.o mtest.o ${LIBS}

test5: test5.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ test5.o mtest.o ${LIBS}

test5_am: test5_am.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ test5_am.o mtest.o ${LIBS}

transpose1: transpose1.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ transpose1.o mtest.o ${LIBS}

transpose2: transpose2.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ transpose2.o mtest.o ${LIBS}

transpose3: transpose3.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ transpose3.o mtest.o ${LIBS}

transpose4: transpose4.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ transpose4.o mtest.o ${LIBS}

transpose5: transpose5.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ transpose5.o mtest.o ${LIBS}

transpose6: transpose6.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ transpose6.o mtest.o ${LIBS}

transpose7: transpose7.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ transpose7.o mtest.o ${LIBS}

window_creation: window_creation.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ window_creation.o mtest.o ${LIBS}

wintest: wintest.o mtest.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ wintest.o mtest.o ${LIBS}

../../fompi.ar: 
	make -C ../../ fompi.ar
