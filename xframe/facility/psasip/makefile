UNAME_I=${shell uname -i}
ifeq ($(UNAME_I), x86_64)
X86_64DEF=-fPIC -DX86_64
endif

LIBDIR = ../../_bin/lib/facility
DLLDIR = ../../_bin/app
INSDIR = ${LIBDIR}/include
LIBNAME = ${LIBDIR}/libpsasip.a
DLLNAME = ${DLLDIR}/psasip.so

SRC_LIBNAME = \
	sipdialogmng.C\
	sipstackpsa.C

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
	
INC_RESIP = ../../_lib/resip/include
LIB_RESIP = ../../_lib/resip/lib
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
    ${INCLUDES_RESIP}\
	-I../interface/sip
	
CFLAGS = ${MG}  ${INCLUDES} -Wno-deprecated ${X86_64DEF}

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
	g++ -shared -o ${DLLNAME} ${OBJ_DLLNAME} -DDLL_EXPORTS ${LIB_RESIP}/librutil.a  ${LIB_RESIP}/libresip.a -fPIC
#	refcheck .

.C.o:   ${SRC_ALL}
	g++ -o $@ -c ${CFLAGS}  $<

#msgdatadef_sip.C : msgdatadef_sip.def  
#	../../_bin/msc -c . -h . -x . -i ../../../include/msg:../../../include/comserv  -m ../../../include/msg  msgdatadef.def
#	../../_bin/msc -c . msgdatadef_sip.def
#
#msgdef_sip.C : msgdef_sip.def 
#	../../_bin/msc -c . -i ../../../include/msg:../../../include/comserv:../../../include/_compat  msgdef_sip.def
#
#msglib_inter_sip.C : msglib_inter_sip.xml  msg_MSGTYPE_SIP.match\
#	msgdef_sip.def 
#	../../_bin/mlibc -f msglib_inter_sip.xml

clean:
	rm -f ${LIBNAME} ${DLLNAME} ${OBJ_DLLNAME} version.h

install:
	mkdir -p ${INSDIR}/
	cp sipdialogmng.h sipstackpsa.h ${INSDIR}/

