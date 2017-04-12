case "$1"
in
all)
	echo "build genphyaddr"
	g++ genphyaddr.cpp  -o genphyaddr

	echo "build vchar"
	g++ vchar.cpp -I../../include/comserv -L../../_bin/lib -lxcomserv -o vchar

	echo "build clist"
	g++ clist.cpp -I../../include/comserv -o clist
	
	;;
clean)
	rm genphyaddr vchar clist
	;;
*)
	echo "build.sh all|clean"
esac
