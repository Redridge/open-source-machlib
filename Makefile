build: machlib

machlib: MachHeader.o FileUtils.o  Segment.o Segment32.o Segment64.o Section.o Section32.o Section64.o MachO.o SymbolTableHeader.o StringTable.o SymbolTableEntry.o SymbolTableEntry32.o SymbolTableEntry64.o SimpleLoadCommands.o LibraryInfo.o FileReader.o main.o
		g++  MachHeader.o FileUtils.o Segment.o Segment32.o Segment64.o Section.o Section32.o Section64.o MachO.o SymbolTableHeader.o StringTable.o SymbolTableEntry.o SymbolTableEntry32.o SymbolTableEntry64.o  main.o SimpleLoadCommands.o LibraryInfo.o FileReader.o -lcapstone -o machlib
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

SymbolTableHeader.o : SymbolTableHeader.cpp
	g++ -c SymbolTableHeader.cpp

StringTable.o : StringTable.cpp
	g++ -c StringTable.cpp

SymbolTableEntry.o : SymbolTableEntry.cpp
	g++ -c SymbolTableEntry.cpp

SymbolTableEntry32.o : SymbolTableEntry32.cpp
	g++ -c SymbolTableEntry32.cpp

SymbolTableEntry64.o : SymbolTableEntry64.cpp
	g++ -c SymbolTableEntry64.cpp


SimpleLoadCommands.o : SimpleLoadCommands.cpp
	g++ -c SimpleLoadCommands.cpp

LibraryInfo.o : LibraryInfo.cpp
	g++ -c LibraryInfo.cpp

FileReader.o : FileReader.cpp
	g++ -c FileReader.cpp

MachO.o : MachO.cpp
	g++ -c MachO.cpp

main.o: main.cpp
	g++ -c main.cpp

clean:
	rm MachHeader.o FileUtils.o main.o Segment.o Segment32.o Segment64.o Section.o Section32.o Section64.o MachO.o SymbolTableHeader.o StringTable.o SymbolTableEntry.o SymbolTableEntry32.o SymbolTableEntry64.o machlib
	rm SimpleLoadCommands.o LibraryInfo.o FileReader.o
