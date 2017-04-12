help()
{
   echo
   echo "Usage: build all [g] [sip] [http] [mqtt]"
   echo "       build clean"
   echo "       build install [install dir]"
   echo
}

BUILD_PWD=$PWD
PUBLISH_DIR=$PWD/../_bin/app
UNAME_I=`uname -i`
INSTALL_DIR=$PUBLISH_DIR
BUILD_SIP=0
BUILD_HTTP=0
BUILD_MQTT=0

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
	       BUILD_SIP=1;
		BUILD_HTTP=1;
		BUILD_MQTT=1;
		;;
            g)
               M_G="MG=-g";;
	    sip)
	       BUILD_SIP=1;;
	    http)
	       BUILD_HTTP=1;;
	    mqtt)
	       BUILD_MQTT=1;;
            *)
               help;
               exit 1;;
         esac;
      done
      echo;

      if [ $BUILD_SIP -eq 1 ]; then
      	echo "======== build interface sip ====================";
      	cd interface/sip;
	if [ -e .needignore ]; then
	    rm .needignore
	fi
      	make all $M_G;
      	if [ $? -ne 0 ]
      	then
        	 exit 1;
      	fi
      	cd -;
      else
        cd interface/sip;
        if [ ! -e .needignore ]; then
	   echo "ignore">>.needignore
	fi
	cd -;
      fi

      if [ $BUILD_HTTP -eq 1 ]; then
      	echo;
      	echo "======== build interface http ====================";
      	cd interface/http;
	if [ -e .needignore ]; then
	    rm .needignore
	fi
      	make all $M_G;
      	if [ $? -ne 0 ]
      	then
         	exit 1;
      	fi
      	cd -;
      else
        cd interface/http;
	if [ ! -e .needignore ]; then
	   echo "ignore">>.needignore
	fi
	cd -;
      fi

      if [ $BUILD_MQTT -eq 1 ]; then
        echo;
        echo "======== build interface mqtt ====================";
        cd interface/mqtt;
	if [ -e .needignore ]; then
	    rm .needignore
	fi
        make all $M_G;
        if [ $? -ne 0 ]
        then
           exit 1;
        fi
        cd -;
      else
        cd interface/mqtt;
        if [ ! -e .needignore ]; then
	   echo "ignore">>.needignore
	fi
	cd -;
      fi

      if [ $BUILD_SIP -eq 1 ]; then
        echo;
        echo "======== build psasip ====================";
        cd psasip;
        make all $M_G;
	if [ -e .needignore ]; then
	    rm .needignore
	fi
        if [ $? -ne 0 ]
        then
           exit 1;
        fi
        cd -;
      else
        cd psasip;
        if [ ! -e .needignore ]; then
	   echo "ignore">>.needignore
	fi
	cd -;
      fi

      if [ $BUILD_HTTP -eq 1 ]; then
        echo;
        echo "======== build psahttp ====================";
        cd psahttp;
	if [ -e .needignore ]; then
	    rm .needignore
	fi
        make all $M_G;
        if [ $? -ne 0 ]
        then
           exit 1;
        fi
        cd -;
      else
        cd psahttp;
        if [ ! -e .needignore ]; then
	   echo "ignore">>.needignore
	fi
	cd -;
      fi

      if [ $BUILD_MQTT -eq 1 ]; then
        echo;
        echo "======== build psamqtt ====================";
        cd psamqtt;
	if [ -e .needignore ]; then
	    rm .needignore
	fi
        make all $M_G;
        if [ $? -ne 0 ]
        then
           exit 1;
        fi
        cd -;
      else
        cd psamqtt;
        if [ ! -e .needignore ]; then
	   echo "ignore">>.needignore
	fi
	cd -;
      fi

      echo;
      echo "======== build psarsc ======================";
      cd psarsc;
      make all $M_G;
      if [ $? -ne 0 ]
      then
           exit 1;
      fi;
      cd -;
      
      echo;
      echo "======== build log task ====================";
      cd logtask;
      make all $M_G;
      if [ $? -ne 0 ]
      then
         exit 1;
      fi
      cd -;
      ;;

   clean|clean.all)
      cd interface/sip;
      if [ -e .needignore ]; then
        make clean;
      fi
      cd -;

      cd interface/http;
      if [ -e .needignore ]; then
        make clean;
      fi
      cd -;

      cd interface/mqtt;
      if [ -e .needignore ]; then
        make clean;
      fi
      cd -;

      cd psasip;
      if [ -e .needignore ]; then
        make clean;
      fi
      cd -;

      cd psahttp;
      if [ -e .needignore ]; then
        make clean;
      fi
      cd -;

      cd psamqtt;
      if [ -e .needignore ]; then
        make clean;
      fi
      cd -;

      cd psarsc;
      make clean;
      cd -;

      cd logtask;
      make clean;
      cd -;
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

      cd interface/sip;
      if [ -e .needignore ]; then
        make install $INSTALL_DIR;
      fi
      cd -;

      cd interface/http;
      if [ -e .needignore ]; then
        make install $INSTALL_DIR;
      fi
      cd -;

      cd interface/mqtt;
      if [ -e .needignore ]; then
        make install $INSTALL_DIR;
      fi
      cd -;

      cd psasip;
      if [ -e .needignore ]; then
        make install $INSTALL_DIR;
      fi
      cd -;

      cd psahttp;
      if [ -e .needignore ]; then
        make install $INSTALL_DIR;
      fi
      cd -;

      cd psamqtt;
      if [ -e .needignore ]; then
        make install $INSTALL_DIR;
      fi
      cd -;

      cd psarsc;
      make install $INSTALL_DIR;
      cd -;

      cd logtask;
      make install $INSTALL_DIR;
      cd -;
      ;;

   *)
      help;
      ;;
   esac
fi 
