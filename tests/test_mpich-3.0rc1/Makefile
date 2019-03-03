include ../../Makefile.inc

LIBS=$(LDFLAGS_TESTS)
INC=-I../../mpitypes/install/include -I../../

CCFLAGS+=$(INC)

EXE = compare_and_swap \
			flush \
			get_acc_local \
			get_accumulate_short \
			get_accumulate_long \
			get_accumulate_double \
			get_accumulate_int \
			get_accumulate_short_derived \
			get_accumulate_long_derived \
			get_accumulate_double_derived \
			get_accumulate_int_derived \
			linked_list \
			linked_list_fop \
			linked_list_lockall \
			manyrma2 \
			reqops \
			win_dynamic_acc

# some general rules
all: ${EXE}

clean:
	rm -f *.o ${EXE}

run: ${EXE}
	./run.sh

compare_and_swap: compare_and_swap.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ compare_and_swap.o ${LIBS}

flush: flush.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ flush.o ${LIBS}

get_acc_local: get_acc_local.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ get_acc_local.o ${LIBS}

get_accumulate_short: get_accumulate_short.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ get_accumulate_short.o ${LIBS}

get_accumulate_short.o: get_accumulate.c
	$(CC) $(CCFLAGS) $(INC) -DGACC_TYPE_SHORT -c $< -o $@

get_accumulate_long: get_accumulate_long.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ get_accumulate_long.o ${LIBS}

get_accumulate_long.o: get_accumulate.c
	$(CC) $(CCFLAGS) $(INC) -DGACC_TYPE_LONG -c $< -o $@

get_accumulate_double: get_accumulate_double.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CXX} ${CCFLAGS} ${LDFLAGS} -o $@ get_accumulate_double.o ${LIBS}

get_accumulate_double.o: get_accumulate.c
	$(CC) $(CCFLAGS) $(INC) -DGACC_TYPE_DOUBLE -c $< -o $@

get_accumulate_int: get_accumulate_int.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CXX} ${CCFLAGS} ${LDFLAGS} -o $@ get_accumulate_int.o ${LIBS}

get_accumulate_int.o: get_accumulate.c
	$(CC) $(CCFLAGS) $(INC) -DGACC_TYPE_INT -c $< -o $@

get_accumulate_short_derived: get_accumulate_short_derived.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ get_accumulate_short_derived.o ${LIBS}

get_accumulate_short_derived.o: get_accumulate.c
	$(CC) $(CCFLAGS) $(INC) -DGACC_TYPE_SHORT -DGACC_TYPE_DERIVED -c $< -o $@

get_accumulate_long_derived: get_accumulate_long_derived.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ get_accumulate_long_derived.o ${LIBS}

get_accumulate_long_derived.o: get_accumulate.c
	$(CC) $(CCFLAGS) $(INC) -DGACC_TYPE_LONG -DGACC_TYPE_DERIVED -c $< -o $@

get_accumulate_double_derived: get_accumulate_double_derived.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ get_accumulate_double_derived.o ${LIBS}

get_accumulate_double_derived.o: get_accumulate.c
	$(CC) $(CCFLAGS) $(INC) -DGACC_TYPE_DOUBLE -DGACC_TYPE_DERIVED -c $< -o $@

get_accumulate_int_derived: get_accumulate_int_derived.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ get_accumulate_int_derived.o ${LIBS}

get_accumulate_int_derived.o: get_accumulate.c
	$(CC) $(CCFLAGS) $(INC) -DGACC_TYPE_INT -DGACC_TYPE_DERIVED -c $< -o $@

linked_list: linked_list.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ linked_list.o ${LIBS}

linked_list_fop: linked_list_fop.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ linked_list_fop.o ${LIBS}

linked_list_lockall: linked_list_lockall.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ linked_list_lockall.o ${LIBS}

manyrma2: manyrma2.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ manyrma2.o ${LIBS}

reqops: reqops.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ reqops.o ${LIBS}

win_dynamic_acc: win_dynamic_acc.o ../../mpitypes/install/lib/libmpitypes.a ../../fompi.ar
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ win_dynamic_acc.o ${LIBS}

../../fompi.ar:
	make -C ../../ fompi.ar
