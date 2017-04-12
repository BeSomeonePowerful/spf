BIN = ./kernaltest

SRC_BIN = \
	testselftask.C\
	testrecvtask.C\
	testsendtask.C\
	main.C

SRC_ALL = \
	${SRC_COM}\
	${SRC_BIN}

OBJ_BIN = ${SRC_BIN:.C=.o}

UNIFRAMEINC = \
	-I.\
	-I../../include\
	-I../../include/comserv\
	-I../../include/msg\
	-I../../include/kernel\
	-I../../include/task\
	-I../../include/_compat\
	-I../../include

CFLAGS = -g ${UNIFRAMEINC} -Wno-deprecated

BASEVERSION="\"`cat ../../version| grep baseversion | cut -b13-`\""
SVNVERSION="\"`LANG=;svn info | grep Revision | cut -b11-`/`LANG=;svn info | grep 'Last Changed Rev' | cut -b19-`\""
BUILDBY="\"`cat ../../version| grep buildby | cut -b9-`\""
BUILDTIME="\"`date`\""

exe: ${BIN}

all: 
	echo "#ifndef _VERSION_H" > ./version.h
	echo "#define _VERSION_H" >> ./version.h
	echo "#define BASEVERSION "${BASEVERSION} >> ./version.h
	echo "#define BUILDVERSION "${SVNVERSION} >> ./version.h
	echo "#define BUILDBY "${BUILDBY} >> ./version.h
	echo "#define BUILDTIME "${BUILDTIME} >> ./version.h
	echo "#endif" >> ./version.h
	make ${BIN}

${BIN}: ${OBJ_BIN}
	g++ -o ${BIN} ${OBJ_BIN} -L../../_bin/lib -lxframe -lcrypto
#	refcheck .

.C.o:   ${SRC_ALL}
	g++ -o $@ -c ${CFLAGS}  $<

clean:
	rm -f ${BIN} ${OBJ_BIN} 

DLLDIR = ../../_bin
install:
	mkdir -p ${DLLDIR}/
	cp ${BIN} ${DLLDIR}/
