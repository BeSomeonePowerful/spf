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
      cd xframe;
      echo;
      echo "======== build xframe ( "$MYSQLVER", "$ORACLEVER") ====================";
      make $M_G MYSQLVER=$MYSQLVER ORACLEVER=$ORACLEVER DBVER_STR=$MYSQLVER_B"_"$ORACLEVER_B REDISVER=$REDISVER INSTALL_DIR=../_lib/xframe;
      if [ $? -ne 0 ]
         then
            exit 1;
         fi
      make install MYSQLVER=$MYSQLVER ORACLEVER=$ORACLEVER REDISVER=$REDISVER INSTALL_DIR=../_lib/xframe;
      cd ..;

      echo;
      echo "======== build psarsc ====================";
      cd application_test/psarsc;
      make all;
      if [ $? -ne 0 ]
      then
         exit 1;
      fi
      make install;
      cd -;

      echo;
      echo "======== build test task ====================";
      cd application_test/testtask;
      make all;
      if [ $? -ne 0 ]
      then
         exit 1;
      fi
      make install;
      cd -;
      
      echo;
      echo "======== build testlogical task ====================";
      cd application_test/testlogicaltask;
      make all;
      if [ $? -ne 0 ]
      then
         exit 1;
      fi
      make install;
      cd -;

      echo;
      echo "======== build log task ====================";
      cd application_test/logtask;
      make all;
      if [ $? -ne 0 ]
      then
         exit 1;
      fi
      make install;
      cd -;

      echo;
      echo "======== build application_test all apps  ====================";
      cd application_test
      make all;
         if [ $? -ne 0 ]
      then
         exit 1;
      fi
      make install;
      cd -;

      echo;
      ;;

   clean|clean.all)
      cd xframe;
      make clean;
      cd ..;

      cd application_test/psarsc;
      make clean;
      cd -;

      cd application_test/testtask;
      make clean;
      cd -;
      
      cd application_test/testlogicaltask;
      make clean;
      cd -;

      cd application_test/logtask;
      make clean;
      cd -;
                 
      cd application_test;
      make clean;
      cd -;

      rm -Rf $PUBLISH_DIR/app/*
      rm -Rf $PUBLISH_DIR/lib/*
      rm -Rf $PUBLISH_DIR/bin/*
      rm -Rf $PUBLISH_DIR/log/*
      rm -Rf $PUBLISH_DIR/shell/*
      rm -f $PUBLISH_DIR/spf
      rm -f $PUBLISH_DIR/spflogical
      rm -f $PUBLISH_DIR/install.sh
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
         if [ -e $PUBLISH_DIR/spf ]; then
            cp $PUBLISH_DIR/spf  $INSTALL_DIR/
         fi
         if [ -e $PUBLISH_DIR/spflogical ]; then
            cp $PUBLISH_DIR/spflogical  $INSTALL_DIR/
         fi
      fi
 
      cp ./_lib/xframe/lib/libxframe.so $INSTALL_DIR/lib/
      cp ./_lib/xframe/lib/libxcomserv.so $INSTALL_DIR/lib/
      cd $INSTALL_DIR/lib/
         rm -f libssl.so* libcrypto.so*
         cp `g++ -print-file-name=libssl.so` `g++ -print-file-name=libcrypto.so` .
         ln -s libssl.so `objdump libssl.so -p | grep SONAME | awk '{print $2}'`
         ln -s libcrypto.so `objdump libcrypto.so -p | grep SONAME | awk '{print $2}'`
      cd -
      cp ./_lib/xframe/lib/libmysqlclient_r.so  $INSTALL_DIR/lib/ -d
      cp ./_lib/xframe/lib/libhiredis.so $INSTALL_DIR/lib/ -d
      cd $INSTALL_DIR/lib/
      ln -s libhiredis.so `objdump libhiredis.so -p | grep SONAME | awk '{print $2}'`
      ln -s libmysqlclient_r.so `objdump libmysqlclient_r.so -p | grep SONAME | awk '{print $2}'`
	  cd -
	  cp ./_lib/xframe/bin/xframev        $INSTALL_DIR/bin/
      cp ./_lib/xframe/bin/xclient    $INSTALL_DIR/bin/
      cp ./_lib/xframe/bin/refcheck        $INSTALL_DIR/bin/
     
      SVNVERSION=`LANG=;svn info | grep Revision | cut -b11-`
      OSNAME=`uname -s`
      FILENAME="spf_"$SVNVERSION"_"$OSNAME"_"`$INSTALL_DIR/bin/xframev b`
      
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
         tar cvfz $FILENAME.tgz  bin lib app $ETCDIR log shell spf spflogical install.sh reTurnServer --exclude=.svn;
      else
         tar cvfz $FILENAME.tgz  bin lib app $ETCDIR log shell spf spflogical install.sh --exclude=.svn;
      fi
      cd -
      echo install and pack ok;
      ;;
   *)
      help;
      ;;
   esac
fi
