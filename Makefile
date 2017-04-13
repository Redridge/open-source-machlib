build: header_cpp

header_cpp: MachHeader.o FileUtils.o main.o
		g++  MachHeader.o FileUtils.o main.o -o header_cpp
MachHeader.o:	MachHeader.cpp
		g++ -c MachHeader.cpp

FileUtils.o: FileUtils.cpp
	g++ -c FileUtils.cpp

main.o: main.cpp
	g++ -c main.cpp

clean:
	rm MachHeader.o FileUtils.o main.o header_cpp
