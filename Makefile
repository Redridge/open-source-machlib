build: header_cpp

header_cpp: MachHeader.o FileUtils.o  Segment.o Segment32.o Segment64.o Section.o Section32.o Section64.o MachO.o main.o
		g++  MachHeader.o FileUtils.o Segment.o Segment32.o Segment64.o Section.o Section32.o Section64.o MachO.o main.o -o header_cpp
MachHeader.o:	MachHeader.cpp
		g++ -c MachHeader.cpp

FileUtils.o: FileUtils.cpp
	g++ -c FileUtils.cpp

Segment.o : Segment.cpp
	g++ -c Segment.cpp

Segment32.o : Segment32.cpp
	g++ -c Segment32.cpp

Segment64.o : Segment64.cpp
	g++ -c Segment64.cpp

Section.o : Section.cpp
	g++ -c Section.cpp

Section32.o : Section32.cpp
	g++ -c Section32.cpp

Section64.o : Section64.cpp
	g++ -c Section64.cpp

MachO.o : MachO.cpp
	g++ -c MachO.cpp

main.o: main.cpp
	g++ -c main.cpp

clean:
	rm MachHeader.o FileUtils.o main.o Segment.o Segment32.o Segment64.o Section.o Section32.o Section64.o MachO.o  header_cpp
