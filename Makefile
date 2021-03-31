all: lynx2wav

lynx2wav : lynx2wav.o
	gcc -o lynx2wav lynx2wav.o -lm

lynx2wav.o: lynx2wav.c
	gcc -c lynx2wav.c

clean :
	rm  lynx2wav.o
	rm  lynx2wav
