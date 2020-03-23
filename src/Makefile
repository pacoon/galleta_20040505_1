all: install

install:  galleta.c
	gcc -o galleta galleta.c -lm -lc;cp galleta ../bin

installwin:  galleta.c
	gcc -DCYGWIN -o galleta.exe galleta.c -lm -lc;cp galleta.exe ../bin

clean: 
	rm -f galleta.exe galleta
