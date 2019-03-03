include Makefile.inc

LIBS=-L. -lfompi -ldmapp
INC=-Impitypes/install/include -Ilibtopodisc


CCFLAGS+=$(FOMPIOPTS) $(INC)
FCFLAGS+=$(FOMPIOPTS) $(INC)
CXXFLAGS+=$(FOMPIOPTS) $(INC)

NEXT_RELEASE=0.2.2

OBJS = \
	fompi_fortran.o \
	fompi_helper.o \
	fompi_op.o \
	fompi_overloaded.o \
	fompi_win_allocate.o \
	fompi_win_attr.o \
	fompi_win_create.o \
	fompi_win_dynamic_create.o \
	fompi_win_fence.o \
	fompi_win_free.o \
	fompi_win_group.o \
	fompi_win_lock.o \
	fompi_win_name.o \
	fompi_win_pscw.o \
	fompi_win_rma.o \
	module_fompi.o \
	fompi_comm.o \
	module_fompi.o \
	fompi_req.o \
	fompi_notif.o \
	fompi_seg.o \
	fompi_notif_uq.o \
	fompi_notif_xpmem.o  

EXE = \
	test_release \
	fortran_test_f77 \
	fortran_test_f90 \
	htor_test

# some general rules

all: $(EXE) libfompi.a fompi.mod

#rules to make targeting Piz Daint (CSCS) (workaround for a bug in cray modules)
daint:
	module load cray-ga && make
#	export CPLUS_INCLUDE_PATH=${CPLUS_INCLUDE_PATH}:"/opt/cray/dmapp/7.0.1-1.0501.7793.12.516.ari/include/"  && \
#	export C_INCLUDE_PATH=${C_INCLUDE_PATH}:"/opt/cray/dmapp/7.0.1-1.0501.7793.12.516.ari/include/" && \
#	make

clean:
	rm -f $(OBJS) $(EXE) *.o fompi_op.c libfompi.a fompi.mod
	echo "invoke make recursive-clean if you want to clean tests and sub-libraries"

recursive-clean: clean
	make -C tests/test_mpich2-1.5b2 clean
	make -C tests/test_mpich-3.0rc1 clean
	make -C tests/test_foMPI clean
	make -C tests/test_foMPI-NA clean
	if test -d mpitypes ; then make -C mpitypes clean ; fi
	rm -f mpitypes/install/lib/libmpitypes.a mpitypes/install/include/mpitypes.h
	make -C libtopodisc clean

distclean: clean recursive-clean
	rm -rf mpitypes


# rules to build foMPI

libfompi.a: $(OBJS) libtopodisc/findcliques.o libtopodisc/meshmap2d.o libtopodisc/libtopodisc.o mpitypes/install/lib/libmpitypes.a
	cp mpitypes/install/lib/libmpitypes.a libfompi.a
	ar -r libfompi.a $(OBJS) libtopodisc/findcliques.o libtopodisc/meshmap2d.o libtopodisc/libtopodisc.o
	ranlib libfompi.a

fompi_fortran.o: fompi_fortran.c fompi.h

fompi_helper.o: fompi_helper.c fompi.h

fompi_op.o: fompi_op.c fompi.h

fompi_op.c: fompi_op.c.m4
	m4 fompi_op.c.m4 > fompi_op.c

fompi_overloaded.o: fompi_overloaded.c fompi.h libtopodisc/libtopodisc.h

fompi_win_allocate.o: fompi_win_allocate.c fompi.h

fompi_win_attr.o: fompi_win_attr.c fompi.h

fompi_win_create.o: fompi_win_create.c fompi.h

fompi_win_dynamic_create.o: fompi_win_dynamic_create.c fompi.h

fompi_win_fence.o: fompi_win_fence.c fompi.h

fompi_win_free.o: fompi_win_free.c fompi.h

fompi_win_group.o: fompi_win_group.c fompi.h

fompi_win_lock.o: fompi_win_lock.c fompi.h

fompi_win_name.o: fompi_win_name.c fompi.h

fompi_win_pscw.o: fompi_win_pscw.c fompi.h

fompi_win_rma.o: fompi_win_rma.c fompi.h mpitypes/install/include/mpitypes.h mpitypes/install/lib/libmpitypes.a

fompi.h: fompi_internal.h

fompi_internal.h: fompi_internal_ex.h ugni_config.h xpmem_config.h fompi_notif_uq.h

fompi_req.o: fompi_req.c fompi.h

fompi_comm.o: fompi_comm.c fompi.h

fompi_notif.o: fompi_notif.c fompi.h

fompi_notif_uq.o: fompi_notif_uq.c  fompi_notif_uq.h fompi.h

fompi_notif_xpmem.o: fompi_notif_xpmem.c fompi.h

fompi_seg.o: fompi_seg.c fompi.h

fompi.mod module_fompi.o: module_fompi.f90
	$(FC) $(FCFLAGS) -c $<


# some simple test executables
test_release.o: test_release.c fompi.h

test_release: test_release.o libfompi.a
	${CC} ${CCFLAGS} ${LDFLAGS} $< -o $@ ${LIBS}

fortran_test_f77.o: fortran_test_f77.f90 fompif.h

fortran_test_f77: fortran_test_f77.o libfompi.a
	$(FC) $(FCFLAGS) ${LDFLAGS} $< -o $@ $(LIBS) 

fortran_test_f90.o: fortran_test_f90.f90 fompi.mod

fortran_test_f90: fortran_test_f90.o libfompi.a
	$(FC) $(FCFLAGS) ${LDFLAGS} $< -o $@ $(LIBS)

htor_test.o: htor_test.cpp fompi.h 

htor_test: htor_test.o libfompi.a
	${CXX} ${CXXFLAGS} ${LDFLAGS} -o $@ htor_test.o ${LIBS}


# rules to build dependencies for foMPI
mpitypes/install/include/mpitypes.h mpitypes/install/lib/libmpitypes.a: mpitypes.tar.bz2
	tar xfj mpitypes.tar.bz2
	find mpitypes/configure.ac -type f -print0 | xargs -0 sed -i 's/mpicc/cc/g'
	cd mpitypes ; \
	./prepare ; \
	./configure --prefix=$(CURDIR)/mpitypes/install
	make -C mpitypes
	make -C mpitypes install
	cp mpitypes/mpitypes-config.h mpitypes/install/include
	cp mpitypes/src/dataloop/dataloop_create.h mpitypes/install/include

libtopodisc/findcliques.o: libtopodisc/findcliques.c
	make -C libtopodisc findcliques.o

libtopodisc/meshmap2d.o: libtopodisc/meshmap2d.c
	make -C libtopodisc meshmap2d.o

libtopodisc/libtopodisc.o: libtopodisc/libtopodisc.c
	make -C libtopodisc libtopodisc.o

# pack a release
pack: clean
	rm -fr foMPI-$(NEXT_RELEASE) #dangerous but well ;)
	mkdir foMPI-$(NEXT_RELEASE)
	cp module_fompi.f90 fompi_*.c fompi_op.c.m4 fompif.h fompi.h fompi_internal.h fompi_internal_ex.h ugni_config.h xpmem_config.h fompi_notif_uq.h foMPI-$(NEXT_RELEASE)
	cp Makefile_release foMPI-$(NEXT_RELEASE)/Makefile
	cp Makefile.inc_release foMPI-$(NEXT_RELEASE)/Makefile.inc
	cp mpitypes.tar.bz2 foMPI-$(NEXT_RELEASE)
	make -C libtopodisc clean
	mkdir foMPI-$(NEXT_RELEASE)/libtopodisc
	cp libtopodisc/*.c libtopodisc/*.h libtopodisc/Makefile libtopodisc/README libtopodisc/*.cpp foMPI-$(NEXT_RELEASE)/libtopodisc
	cp test_release.c foMPI-$(NEXT_RELEASE)/c_test.c
	cp test_release_NA.c foMPI-$(NEXT_RELEASE)/c_test_NA.c
	cp fortran_test_*.f90 foMPI-$(NEXT_RELEASE)/
	mkdir foMPI-$(NEXT_RELEASE)/test_address
	cp test_address/Makefile test_address/test_address.f90 test_address/test_address_c.c foMPI-$(NEXT_RELEASE)/test_address
	cp AUTHORS README.md LICENSE CHANGES foMPI-$(NEXT_RELEASE)
	tar czf foMPI-$(NEXT_RELEASE).tar.gz foMPI-$(NEXT_RELEASE)
	echo "voila: foMPI-$(NEXT_RELEASE).tgz"
	rm -r foMPI-$(NEXT_RELEASE) #dangerous but well ;)

# some legacy stuff

build_mpich2_tests: libfompi.a mpitypes/install/include/mpitypes.h mpitypes/install/lib/libmpitypes.a

build_mpich3_tests: libfompi.a mpitypes/install/include/mpitypes.h mpitypes/install/lib/libmpitypes.a
	make -C test_mpich-3.0rc1 all

run_mpich2_tests: build_mpich2_tests
	make -C test_mpich2-1.5b2 run

run_mpich3_tests: build_mpich3_tests
	make -C test_mpich-3.0rc1 run

build_fompi_tests: libfompi.a mpitypes/install/include/mpitypes.h mpitypes/install/lib/libmpitypes.a
	make -C test_foMPI main
	
build_fompi-na_tests: libfompi.a mpitypes/install/include/mpitypes.h mpitypes/install/lib/libmpitypes.a
	make -C test_foMPI-NA main

