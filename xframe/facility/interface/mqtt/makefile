UNAME_I=${shell uname -i}
ifeq ($(UNAME_I), x86_64)
X86_64DEF=-fPIC -DX86_64
endif

LIBDIR = ../../../_bin/lib/facility
DLLDIR = ../../../_bin/app
INSDIR = ${LIBDIR}/include
LIBNAME = ${LIBDIR}/libmsglib_mqtt.a
DLLNAME = ${DLLDIR}/msglib_mqtt.so

SRC_LIBNAME = \
	msgdef_mqtt.C

SRC_DLLNAME = \
	${SRC_LIBNAME}

SRC_ALL = \
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

INCLUDES = \
	-I.\
	${INCLUDES_XFRAME}\
	
CFLAGS = ${MG}  -DDLL_EXPORTS ${INCLUDES} -Wno-deprecated ${X86_64DEF}

all: ${LIBNAME} ${DLLNAME}

lib: ${LIBNAME}

dll: ${DLLNAME}

all: ${LIBNAME} ${DLLNAME}

${LIBNAME}: ${OBJ_LIBNAME}
	mkdir -p ${LIBDIR}
	ar crv ${LIBNAME} ${OBJ_LIBNAME}
	@echo

${DLLNAME}: ${OBJ_DLLNAME}
	mkdir -p ${DLLDIR}
	g++ -shared -o ${DLLNAME} ${OBJ_DLLNAME} -DDLL_EXPORTS -fPIC
#	refcheck -f ${DLLNAME}

.C.o:   ${SRC_ALL}
	g++ -o $@ -c ${CFLAGS}  $<

clean:
	rm -f ${LIBNAME} ${DLLNAME} ${OBJ_DLLNAME}

install:
	mkdir -p ${INSDIR}
	cp msgdef_mqtt.h ${INSDIR}
