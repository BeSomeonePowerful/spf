UNAME_I=${shell uname -i}
ifeq ($(UNAME_I), x86_64)
X86_64DEF=-fPIC -DX86_64
endif

EXE = ../_bin/psamqtt.so

SRC = \
	psamqtt.C\
	mqtt.C\
	mqttserver.C\
	mqttclient.C\
	mqttlink.C\
	mqttmsg.C\

OBJ = ${SRC:.C=.o}

ifeq ($(ILD),uni)
INCDIR = ${UNIDIR}/include
INC_INTERFACE = ${UNIDIR}/include/interface
LIB_DIR=${UNIDIR}/lib
else
INCDIR = ../../include
INC_INTERFACE = ../interface
LIB_DIR=../interface/mqtt
endif

INC = \
        -I${INCDIR}/comserv\
        -I${INCDIR}/kernel\
        -I${INCDIR}/component\
        -I${INCDIR}/msg\
        -I${INC_INTERFACE}/mqtt

CFLAGS = -Wno-deprecated -DDLL_EXPORTS -g ${X86_64DEF}

${EXE} : ${OBJ}
	g++ -shared -o ${EXE} ${CFLAGS} ${OBJ} -L${LIB_DIR} -lmsglib_mqtt

.C.o:   ${SRC}
	${CC} -o $@ -c ${INC} ${CFLAGS} $<

clean:
	rm -f ${EXE} ${OBJ}

install:
	rm -rf $(UNIDIR)/app/psamqtt.so
	cp ../_bin/psamqtt.so $(UNIDIR)/app
