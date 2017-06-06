CC := g++ -Wall
SRCDIR := src
BUILDDIR := build
TARGET := bin/libopenmach.so
LIBDIR := bin

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -fpic
LIB := -lcapstone
INC := -I include

build: $(TARGET) bin/poc

$(TARGET): $(OBJECTS)
	@mkdir -p $(LIBDIR)
	@echo " $(CC) $^ -o $(TARGET) $(LIB)"; $(CC) $^ -shared -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " $(RM) -r $(BUILDDIR) $(TARGET) bin/poc"; $(RM) -r $(BUILDDIR) $(TARGET) bin/poc

bin/poc: poc/poc.cpp
	@echo " $(CC) -L$(LIBDIR) -o $@ $< -lopenmach $(INC)"; $(CC) -L$(LIBDIR) -o $@ $< -lopenmach $(INC)

.PHONY: clean
