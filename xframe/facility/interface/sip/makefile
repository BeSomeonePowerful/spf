UNAME_I=${shell uname -i}
ifeq ($(UNAME_I), x86_64)
X86_64DEF=-fPIC -DX86_64
endif

LIBDIR = ../../../_bin/lib/facility
DLLDIR = ../../../_bin/app
INSDIR = ${LIBDIR}/include
LIBNAME = ${LIBDIR}/libmsglib_psa.a
DLLNAME = ${DLLDIR}/msglib_psa.so

SRC_LIBNAME = \
	msgdef_sip.C\
	msghelper_sip.C

SRC_DLLNAME = \
	${SRC_LIBNAME}

SRC_ALL = \
	${SRC_COM}\
	${SRC_DLLNAME}

OBJ_LIBNAME = ${SRC_LIBNAME:.C=.o}
OBJ_DLLNAME = ${SRC_DLLNAME:.C=.o}

INC_XFRAME = ../../../include
INCLUDES_XFRAME = \
	-I${INC_XFRAME}/comserv\
	-I${INC_XFRAME}/msg\
	-I${INC_XFRAME}/kernel\
	-I${INC_XFRAME}/task\
	-I${INC_XFRAME}/_compat
	
INC_RESIP = ../../../_lib/resip/include
LIB_RESIP = ../../../_lib/resip/lib
INCLUDES_RESIP = \
    -I${INC_RESIP}\
    -I${INC_RESIP}/rutil\
	-I${INC_RESIP}/rutil/ssl\
    -I${INC_RESIP}/rutil/dns\
    -I${INC_RESIP}/rutil/stun\
    -I${INC_RESIP}/resip/stack\
    -I${INC_RESIP}/resip/stack/ssl\
    -I${INC_RESIP}/resip/dum\
    -I${INC_RESIP}/contrib\
    -I${INC_RESIP}/contrib/ares
	
INCLUDES = \
	-I.\
	${INCLUDES_XFRAME}\
    ${INCLUDES_RESIP}

CFLAGS = ${MG} ${INCLUDES} -Wno-deprecated ${X86_64DEF}

lib: ${LIBNAME}

dll: ${DLLNAME}

all: ${LIBNAME} ${DLLNAME}

${LIBNAME}: ${OBJ_LIBNAME}
	mkdir -p ${LIBDIR}
	ar crv ${LIBNAME} ${OBJ_LIBNAME}
	@echo

${DLLNAME}: ${OBJ_DLLNAME}
	mkdir -p ${DLLDIR}
	g++ -shared -o ${DLLNAME} ${OBJ_DLLNAME} -DDLL_EXPORTS -L${LIB_RESIP} -lrutil -lresip -fPIC
#	refcheck .

.C.o:   ${SRC_ALL}
	g++ -o $@ -c ${CFLAGS}  $<

clean:
	rm -f ${LIBNAME} ${DLLNAME} ${OBJ_DLLNAME} 

install:
	mkdir -p ${INSDIR}/
	cp msgdef_sip.h msgconstdef_sip.h msghelper_sip.h ${INSDIR}/
