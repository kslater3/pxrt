
build: src/pxrt.c
	gcc -Wall -o pxrt src/pxrt.c -lm -lGL -lSDL2


debug: main.csrc/pxrt.c
	gcc -g -Wall -o pxrt.g src/pxrt.c -lm -lGL -lSDL2


clean:
	rm pxrt.g
