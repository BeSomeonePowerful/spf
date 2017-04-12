CURPATH=`pwd`
RTCPATH=`dirname $PWD`
export TBLIB_ROOT=$RTCPATH/../_lib/tbnet
if [ "$1" = "clean" ]; then
   ./build.sh clean
   rm -rf $TBLIB_ROOT/*
else
   ./build.sh make
   ./build.sh install 
fi
