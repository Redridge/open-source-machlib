build: header_cpp

header_cpp: MachHeader.o FileUtils.o Segment32.o Segment64.o main.o
		g++  MachHeader.o FileUtils.o Segment32.o Segment64.o main.o -o header_cpp
MachHeader.o:	MachHeader.cpp
		g++ -c MachHeader.cpp

FileUtils.o: FileUtils.cpp
	g++ -c FileUtils.cpp

Segment32.o : Segment32.cpp
	g++ -c Segment32.cpp

Segment64.o : Segment64.cpp
	g++ -c Segment64.cpp

main.o: main.cpp
	g++ -c main.cpp

clean:
	rm MachHeader.o FileUtils.o main.o header_cpp
