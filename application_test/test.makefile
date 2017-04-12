filename = test.makefile

BIN = ./spf

SRC_BIN = main_spf.C

OBJ_BIN = ${SRC_BIN:.C=.o}

UNIFRAMEINC = \
	-I.\
	-I../_lib/xframe/include/comserv\
	-I../_lib/xframe/include/_compat\
	-I../_lib/xframe/include/msg\
	-I../_lib/xframe/include/kernel\
	-I../_lib/xframe/include/task\
	-I../xframe/include/transport\
	-I../_lib/xframe/include\
	-I../_lib/application_test/include\
	-I./psarsc\
	-I./testtask\
	-I./logtask\

CFLAGS = -g ${UNIFRAMEINC} -Wno-deprecated

BASEVERSION="\"`cat ../version| grep baseversion | cut -b13-`\""
SVNVERSION="\"`LANG=;svn info | grep Revision | cut -b11-`/`LANG=;svn info | grep 'Last Changed Rev' | cut -b19-`\""
BUILDBY="\"`cat ../version| grep buildby | cut -b9-`\""
BUILDTIME="\"`date`\""

${BIN}: ${OBJ_BIN}
	g++ -o ${BIN} ${OBJ_BIN} -lssl -lcrypto -L../_lib/application_test/lib -lpsarsc -ltesttask -llogtask -L../_lib/xframe/lib -lmysqlclient_r -lhiredis -lxframe

all: 
	echo "#ifndef _VERSION_H" > ./version.h
	echo "#define _VERSION_H" >> ./version.h
	echo "#define BASEVERSION "${BASEVERSION} >> ./version.h
	echo "#define BUILDVERSION "${SVNVERSION} >> ./version.h
	echo "#define BUILDBY "${BUILDBY} >> ./version.h
	echo "#define BUILDTIME "${BUILDTIME} >> ./version.h
	echo "#endif" >> ./version.h
	make ${BIN} -f ${filename}


.C.o:   ${SRC_BIN}
	g++ -o $@ -c ${CFLAGS}  $<

clean:
	rm -f ${BIN} ${OBJ_BIN} version.h

DLLDIR = ../_bin
install:
	mkdir -p ${DLLDIR}/
	cp ${BIN} ${DLLDIR}/
