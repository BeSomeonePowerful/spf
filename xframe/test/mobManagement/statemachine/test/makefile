BIN = ./uaregtest

SRC_BIN = \
	main.C\
	microsatmock.C\
	mainsatmock.C\
	rscpsamock.C\
	nmemock.C\
	../microsat/addrresolvesm.C\
	../mainsat/uaidresolvesm.C\
	../mainsat/uaipresolvesm.C\
	../mobutility.C\
	../mobmsgtype.C\
	# CommonTask.C\
	# idletask.C\
	# subtask.C\
	# subselftask.C\
	# clientselftask.C\
	# serverselftask.C\
	# mqttclienttask.C\
	# mqttservertask.C\
	# statemachine.C\
	# ../../task/psatask.C\
	# ../../task/psathread.C\
	# ../../facility/psamqtt-wangpan/mqtt.C\
	# ../../facility/psamqtt-wangpan/mqttclient.C\
	# ../../facility/psamqtt-wangpan/mqttlink.C\
	# ../../facility/psamqtt-wangpan/mqttmsg.C\
	# ../../facility/psamqtt-wangpan/mqttserver.C\
	#../../facility/interface/mqtt/msgdef_mqtt.C

SRC_ALL = \
	${SRC_COM}\
	${SRC_BIN}

HEAD_ALL = ${SRC_BIN:.C:.h}

OBJ_BIN = ${SRC_BIN:.C=.o}

UNIFRAMEINC = \
	-I.\
	-I../../../../include\
	-I../../../../include/_compat\
	-I../../../../include/comserv\
	-I../../../../include/msg\
	-I../../../../include/kernel\
	-I../../../../include/task\
	-I../../../../include\
	-I../../../../facility/psamqtt-wangpan\
	-I../../../../facility/interface/mqtt\
	-I../../

CFLAGS = -g ${UNIFRAMEINC} -Wno-deprecated -std=c++11


exe: ${BIN}

all: ${BIN}

${BIN}: ${OBJ_BIN}
	g++ -o ${BIN} ${OBJ_BIN} -L../../../../_bin/lib  -lcrypto \
	-lmysqlclient_r -lhiredis  -lxcomserv -lxframe ${SYSLIB}
	#	refcheck .

.C.o:   ${SRC_ALL} ${HEAD_ALL}
	g++ -o $@ -c ${CFLAGS}  $<

clean:
	rm -f ${BIN} ${OBJ_BIN}

DLLDIR = ../../_bin
install:
	mkdir -p ${DLLDIR}/
	cp ${BIN} ${DLLDIR}/
