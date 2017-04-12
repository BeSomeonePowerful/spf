help()
{
   echo
   echo "Usage: build all [m51] [o10] [ssl101] [g]"
   echo "                 [m50] [o9]  [ssl100]"
   echo "                             [ssl102]"
   echo "       build clean"
   echo "       build install [install dir]"
   echo
}

BUILD_PWD=$PWD
PUBLISH_DIR=$PWD/_bin
UNAME_I=`uname -i`

if [ $# -eq 0 ]; then
   help;
else
   MYSQLVER=mysql5.1
   MYSQLVER_B=m51
   ORACLEVER=oracle10
   ORACLEVER_B=o10
   REDISVER=hiredis
   case "$1"
   in
   all)
      for i in $*; do
         case "$i"
         in
            all|clean|clean.all|install|pack)
               ;;
            o9)
               ORACLEVER=oracle9;
               ORACLEVER_B=o9;;
            o10)
               ORACLEVER=oracle10;
               ORACLEVER_B=o10;;
            m50)
               MYSQLVER=mysql5.0;
               MYSQLVER_B=m50;;
            m51)
               MYSQLVER=mysql5.1;
               MYSQLVER_B=m51;;
            g)
               M_G="MG=-g";;
            *)
               help;
               exit 1;;
         esac;
      done
      echo;
      echo "======== build xframe ( "$MYSQLVER", "$ORACLEVER") ====================";
      make $M_G MYSQLVER=$MYSQLVER ORACLEVER=$ORACLEVER DBVER_STR=$MYSQLVER_B"_"$ORACLEVER_B REDISVER=$REDISVER BUILD_SIP=$BUILD_SIP_B BUILD_HTTP=$BUILD_HTTP_B INSTALL_DIR=../_lib/xframe;
      if [ $? -ne 0 ]
         then
            exit 1;
         fi
      make install MYSQLVER=$MYSQLVER ORACLEVER=$ORACLEVER REDISVER=$REDISVER BUILD_SIP=$BUILD_SIP_B BUILD_HTTP=$BUILD_HTTP_B INSTALL_DIR=../_lib/xframe;
      cd ..;
      ;;

   clean|clean.all)
      make clean;
      ;;
     
   install)
      if [ $# -eq 2 ]; then
         echo
         echo Install to $2
         echo
         INSTALL_DIR=$2
      else
         INSTALL_DIR=$PUBLISH_DIR
      fi

      ETCDIR=etc._demo_

      mkdir -p $INSTALL_DIR/
      mkdir -p $INSTALL_DIR/$ETCDIR/
      mkdir -p $INSTALL_DIR/lib/
      mkdir -p $INSTALL_DIR/log/
      rm -Rf $INSTALL_DIR/log/*
      mkdir -p $INSTALL_DIR/bin/
      mkdir -p $INSTALL_DIR/app/
      mkdir -p $INSTALL_DIR/shell/

      if [ $INSTALL_DIR != $PUBLISH_DIR ]
      then
         mkdir -p $INSTALL_DIR/
         rm -Rf $INSTALL_DIR/*
         mkdir -p $INSTALL_DIR/$ETCDIR/
         mkdir -p $INSTALL_DIR/lib/
         mkdir -p $INSTALL_DIR/log/
         mkdir -p $INSTALL_DIR/bin/
         mkdir -p $INSTALL_DIR/app/
         mkdir -p $INSTALL_DIR/shell/
         cp $PUBLISH_DIR/app/* $INSTALL_DIR/app/
         cp $PUBLISH_DIR/$ETCDIR/* $INSTALL_DIR/$ETCDIR/
         cp $PUBLISH_DIR/lib/* $INSTALL_DIR/lib/
      fi
 
      cp ./_lib/xframe/lib/libxframe.so $INSTALL_DIR/lib/
      cp ./_lib/xframe/lib/libxcomserv.so $INSTALL_DIR/lib/
      cd $INSTALL_DIR/lib/
         rm -f libssl.so* libcrypto.so*
         cp `g++ -print-file-name=libssl.so` `g++ -print-file-name=libcrypto.so` .
         ln -s libssl.so `objdump libssl.so -p | grep SONAME | awk '{print $2}'`
         ln -s libcrypto.so `objdump libcrypto.so -p | grep SONAME | awk '{print $2}'`
      cd -
      cp ./_lib/xframe/lib/libmysqlclient_r.so*  $INSTALL_DIR/lib/ -d
      cp ./_lib/xframe/lib/libhiredis.so* $INSTALL_DIR/lib/ -d
      cd $INSTALL_DIR/lib/
	 ln -s libhiredis.so `objdump libhiredis.so -p | grep SONAME | awk '{print $2}'`
      cd -
      cp ./_lib/xframe/bin/xframev        $INSTALL_DIR/bin/
      cp ./_lib/xframe/bin/xclient    $INSTALL_DIR/bin/
      cp ./_lib/xframe/bin/refcheck        $INSTALL_DIR/bin/
      if [ -e ./_lib/resip/lib/libresip.so ]; then
        cp ./_lib/resip/lib/libresip.so $INSTALL_DIR/lib/ -d
        cp ./_lib/resip/lib/libresip-1.10.so $INSTALL_DIR/lib/
        cp ./_lib/resip/lib/libresipares.so $INSTALL_DIR/lib/ -d
        cp ./_lib/resip/lib/libresipares-1.10.so $INSTALL_DIR/lib/
        cp ./_lib/resip/lib/librutil.so $INSTALL_DIR/lib/ -d
        cp ./_lib/resip/lib/librutil-1.10.so $INSTALL_DIR/lib/
        if [ -e ./_lib/resip/lib/libreTurnClient-1.10.so ]; then
          cp ./_lib/resip/lib/libreTurnClient-1.10.so $INSTALL_DIR/lib/
          cp ./_lib/resip/lib/libreTurnClient.so $INSTALL_DIR/lib/ -d
        fi
        if [ -e ./_lib/resip/sbin/reTurnServer ]; then
          cp ./_lib/resip/sbin/reTurnServer  $INSTALL_DIR/
        fi
      fi
      if [ -e ./_lib/tbnet/lib/libtbnet.so ]; then 
        cp ./_lib/tbnet/lib/libtbnet.so* $INSTALL_DIR/lib/ -d
        cp ./_lib/tbnet/lib/libtbsys.so* $INSTALL_DIR/lib/ -d
      fi

      SVNVERSION=`LANG=;svn info | grep Revision | cut -b11-`
      OSNAME=`uname -s`
      FILENAME="xframe_"$SVNVERSION"_"$OSNAME"_"`$INSTALL_DIR/bin/xframev b`
      
      echo "FILENAME=$FILENAME" >> $INSTALL_DIR/install.sh
      cat ./_shell/install.m >> $INSTALL_DIR/install.sh
      chmod 755 $INSTALL_DIR/install.sh
       
      cp \
         _shell/watcher.m \
         _shell/useetc \
         $INSTALL_DIR/shell/
     
      cd ${INSTALL_DIR};
      rm -f $FILENAME.tgz
      if [ -e reTurnServer ]; then 
         tar cvfz $FILENAME.tgz  bin lib app $ETCDIR log shell install.sh reTurnServer --exclude=.svn;
      else
         tar cvfz $FILENAME.tgz  bin lib app $ETCDIR log shell install.sh --exclude=.svn;
      fi
      cd -
      echo install and pack ok;
      ;;
   *)
      help;
      ;;
   esac
fi 
