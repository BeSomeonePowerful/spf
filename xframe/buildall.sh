
help()
{
   echo
   echo "Usage: buildall [3pty]"
   echo "       buildall clean"
   echo "       buildall install [install dir]"
   echo
}

BUILD_PWD=$PWD
PUBLISH_DIR=$PWD/_bin
UNAME_I=`uname -i`

if [ $# -eq 0 ]; then
	cd facility
	./build.sh all sip http
	cd ..
	./build.sh all
else
   case "$1"
   in
	3pty)
	   cd _3pty
	   ./build.sh all sip http
	   cd ..
	   ;;
	clean)
	   ./build.sh clean
	   cd facility
	   ./build.sh clean
	   cd ..
	   ;;
	install)
	   if [ -z $2 ]; then
	       INSTALL_DIR=$PUBLISH_DIR
	   else
	       echo
	       echo Install to $2
	       echo
	       INSTALL_DIR=$2
	   fi
	   cd _3pty
	   ./build.sh install $INSTALL_DIR
	   cd ..
	   cd facility
	   ./build.sh install $INSTALL_DIR
	   cd ..
	   ./build.sh install $INSTALL_DIR
	   ;;
	?)
	   help
	   ;;
	*)
	   ;;
   esac
fi
