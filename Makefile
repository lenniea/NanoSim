EXE = 
OBJ = o
CXX = $(shell wx-config --cxx)
CC = $(shell wx-config --cc)

PROGRAM = NanoSim$(EXE)

OBJECTS = SimMain.$(OBJ) NanoCpu.$(OBJ) NanoMem.$(OBJ)

# implementation

.SUFFIXES:      .$(OBJ) .cpp .c

.cpp.$(OBJ) :
	$(CXX) -c `wx-config --cxxflags` -o $@ $<

.c.$(OBJ) :
	$(CC) -c `wx-config --cxxflags` -o $@ $<

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CXX) -o $(PROGRAM)$(EXE) $(OBJECTS) `wx-config --libs`

clean:
	rm -f *.$(OBJ) $(PROGRAM)
