BIN = ./psahttptest

SRC_BIN = \
	testhttptask.C\
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
	-I../../include\
	-I../../_bin/lib/facility/include\
	-I../../_bin/lib/facility\
	-I../../_lib/tbnet/include/tbnet\
	-I../../_lib/tbnet/include/tbsys

CFLAGS = -g ${UNIFRAMEINC} -Wno-deprecated


exe: ${BIN}

all: ${BIN}

${BIN}: ${OBJ_BIN}
	g++ -o ${BIN} ${OBJ_BIN} -L../../_bin/lib/facility -lpsahttp -lmsglib_http -L../../_lib/tbnet/lib/ -ltbsys -ltbnet -L../../_bin/lib -lxframe -lcrypto 
#	refcheck .

.C.o:   ${SRC_ALL}
	g++ -o $@ -c ${CFLAGS}  $<

clean:
	rm -f ${BIN} ${OBJ_BIN} 

DLLDIR = ../../_bin
install:
	mkdir -p ${DLLDIR}/
	cp ${BIN} ${DLLDIR}/
