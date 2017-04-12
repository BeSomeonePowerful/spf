UNAME_I=${shell uname -i}
ifeq ($(UNAME_I), x86_64)
X86_64DEF=-fPIC -DX86_64
endif

LIBDIR = ../../_bin/lib/facility
DLLDIR = ../../_bin/app
INSDIR = ${LIBDIR}/include
LIBNAME = ${LIBDIR}/libpsamqtt.a
DLLNAME = ${DLLDIR}/psamqtt.so

SRC_LIBNAME = \
	mqtt.C\
	mqttserver.C\
	mqttclient.C\
	mqttlink.C\
	mqttmsg.C\
	psamqtt.C

SRC_DLLNAME = \
	${SRC_LIBNAME}

SRC_ALL = \
	${SRC_COM}\
	${SRC_DLLNAME}

OBJ_LIBNAME = ${SRC_LIBNAME:.C=.o}
OBJ_DLLNAME = ${SRC_DLLNAME:.C=.o}

INC_XFRAME = ../../include
INCLUDES_XFRAME = \
	-I${INC_XFRAME}/comserv\
	-I${INC_XFRAME}/msg\
	-I${INC_XFRAME}/kernel\
	-I${INC_XFRAME}/task\
	-I${INC_XFRAME}/_compat

INCLUDES = \
	-I.\
	${INCLUDES_XFRAME}\
	-I../interface/mqtt
	
CFLAGS = ${MG}  ${INCLUDES} -Wno-deprecated ${X86_64DEF} -std=c++11

BASEVERSION="\"`cat ../../version| grep baseversion | cut -b13-`\""
SVNVERSION="\"`LANG=;svn info | grep Revision | cut -b11-`/`LANG=;svn info | grep 'Last Changed Rev' | cut -b19-`\""
BUILDBY="\"`cat ../../version| grep buildby | cut -b9-`\""
BUILDTIME="\"`date`\""

all: 
	echo "#ifndef _VERSION_H" > ./version.h
	echo "#define _VERSION_H" >> ./version.h
	echo "#define BASEVERSION "${BASEVERSION} >> ./version.h
	echo "#define BUILDVERSION "${SVNVERSION} >> ./version.h
	echo "#define BUILDBY "${BUILDBY} >> ./version.h
	echo "#define BUILDTIME "${BUILDTIME} >> ./version.h
	echo "#endif" >> ./version.h
	make ${LIBNAME}
	make ${DLLNAME}

lib: ${LIBNAME}

dll: ${DLLNAME}

${LIBNAME}: ${OBJ_LIBNAME}
	mkdir -p ${LIBDIR}
	ar crv ${LIBNAME} ${OBJ_LIBNAME}
	@echo

${DLLNAME}: ${OBJ_DLLNAME}
	mkdir -p ${DLLDIR}
	g++ -shared -o ${DLLNAME} ${OBJ_DLLNAME} -DDLL_EXPORTS -fPIC -std=c++11
#	refcheck .

.C.o:   ${SRC_ALL}
	g++ -o $@ -c ${CFLAGS}  $<

clean:
	rm -f ${LIBNAME} ${DLLNAME} ${OBJ_DLLNAME} version.h

install:
	mkdir -p ${INSDIR}/
	cp mqtt.h mqttclient.h mqttlink.h mqttmsg.h mqttmsg_arg.h mqttserver.h psamqtt.h ${INSDIR}/
