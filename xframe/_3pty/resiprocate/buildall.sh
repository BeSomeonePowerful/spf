help()
{
	echo "Usage: build all/clean/reconfig ..."
}

CURPATH=`pwd`
RTCPATH=`dirname $PWD`
RESIPPATH=$RTCPATH/../_lib/resip
if [ $# -eq 0 ]; then
   help;
   exit
fi
if [ "`uname -i`" = "x86_64" ]; then
   X8664FLAG=--with-pic
fi
if [ "$1" = "clean" ]; then
   make distclean
   rm -rf $RESIPPATH/*
   rm -rf resip/stack/gen/*
   exit
fi
if [ "$1" = "reconfig" ]; then
	make distclean
	rm -rf $RESIPPATH/*
	rm -rf resip/stack/gen/*
	echo 
	echo "./configure --prefix=$RESIPPATH --exec-prefix=$RESIPPATH --with-ssl $X8664FLAG"
	echo
	./configure --prefix=$RESIPPATH --exec-prefix=$RESIPPATH --with-ssl $X8664FLAG
fi

make all
make install

