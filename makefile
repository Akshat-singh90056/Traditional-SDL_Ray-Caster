all:
	g++ main.cpp tracer.cpp map.c -o main -Iinclude -L/usr/local/lib -I/usr/local/lib -I/usr/local/include/SDL3 -lSDL3

run:
	g++ main.cpp tracer.cpp map.c -o main -Iinclude -L/usr/local/lib -I/usr/local/lib -I/usr/local/include/SDL3 -lSDL3
	./main
	
clean:
	rm main