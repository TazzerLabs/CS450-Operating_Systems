all: p1.x p2.x p3.x p3.s p4.x 

# Get the .o and asm files for the problem set

p1.x:   
	gcc p1.c -o p1.x

p2.x:
	gcc p2.c -o p2.x

p3.x:
	g++ -std=c++11 p3.cpp -o p3.x

p3.s:
	g++ -std=c++11 p3.cpp -S

p4.x:
	g++ p4.cpp -o p4.x

run:
	./p2.o
	./p3.o
	./p4.o

clean:
	rm *.x
