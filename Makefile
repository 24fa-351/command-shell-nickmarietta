build:
	gcc -o main main.c myshellfunctions.c myshellfunctions.h

test:
	./main

clean:
	rm main