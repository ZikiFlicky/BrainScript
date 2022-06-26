CXX=g++
LINK=-lm -lSDL2
CXXFLAGS=-Wall -std=c++11 -Wextra

MKDIR=mkdir -p
BUILDDIR=build
EXE_NAME=bf
RMDIR=rm -rf

OBJECTS=brainfuck.o \
		main.o

.PHONY: clean all debug

debug: CXXFLAGS+=-g
debug: clean $(BUILDDIR)/$(EXE_NAME)

all: CXXFLAGS+=-DNDEBUG
all: clean $(BUILDDIR)/$(EXE_NAME)

$(BUILDDIR):
	$(MKDIR) $@

$(BUILDDIR)/$(EXE_NAME): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(BUILDDIR)/* -o $@ $(LINK)

%.o: %.cpp $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c -o $(BUILDDIR)/$@ $<

clean:
	$(RMDIR) $(BUILDDIR)