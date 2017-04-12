makefiles=test.makefile testlogical.makefile

all: 
	for makefile in ${makefiles} ; do\
		make all -f $$makefile;\
	done

clean:
	for makefile in ${makefiles} ; do\
		make clean -f $$makefile;\
	done

install:
	for makefile in ${makefiles} ; do\
		make install -f $$makefile;\
	done

