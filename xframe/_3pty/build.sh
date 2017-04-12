help()
{
   echo
   echo "Usage: build all [sip] [http]"
   echo "       build install [install dir]"
   echo
}

if [ $# -eq 0 ]; then
   help;
else
   case "$1"
   in
   all)
      for i in $*; do
         case "$i"
         in
         all|clean|install)
	   ;;
         sip)
	   cd resiprocate
	   ./buildall.sh reconfig
	   cd ..
	   ;;
	 http)
	   cd tbnet
           ./buildall.sh
	   cd ..
	   ;;
	 *)
	   help;
	   exit 1;;
         esac;
       done;;

   install)
      if [ $# -eq 2 ]; then
         echo
         echo Install 3pty lib to $2
         echo
         INSTALL_DIR=$2
      else
         INSTALL_DIR=../_bin
      fi
   
      if [ -e ../_lib/resip/lib/libresip.so ]; then
        cp ../_lib/resip/lib/libresip.so $INSTALL_DIR/lib/ -d
        cp ../_lib/resip/lib/libresip-1.10.so $INSTALL_DIR/lib/
        cp ../_lib/resip/lib/libresipares.so $INSTALL_DIR/lib/ -d
        cp ../_lib/resip/lib/libresipares-1.10.so $INSTALL_DIR/lib/
        cp ../_lib/resip/lib/librutil.so $INSTALL_DIR/lib/ -d
        cp ../_lib/resip/lib/librutil-1.10.so $INSTALL_DIR/lib/
        if [ -e ../_lib/resip/lib/libreTurnClient-1.10.so ]; then
          cp ../_lib/resip/lib/libreTurnClient-1.10.so $INSTALL_DIR/lib/
          cp ../_lib/resip/lib/libreTurnClient.so $INSTALL_DIR/lib/ -d
        fi
        if [ -e ../_lib/resip/sbin/reTurnServer ]; then
          cp ../_lib/resip/sbin/reTurnServer  $INSTALL_DIR/
        fi
      fi
      if [ -e ../_lib/tbnet/lib/libtbnet.so ]; then 
        cp ../_lib/tbnet/lib/libtbnet.so* $INSTALL_DIR/lib/ -d
        cp ../_lib/tbnet/lib/libtbsys.so* $INSTALL_DIR/lib/ -d
      fi
      ;;
   *)
      help;
      ;;
   esac;
fi

