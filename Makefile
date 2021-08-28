EXE = 
OBJ = o
CXX = $(shell wx-config --cxx)

PROGRAM = NanoSim.$(EXE)

OBJECTS = SimMain.$(OBJ) NanoCpu.$(OBJ)

# implementation

.SUFFIXES:      .$(OBJ) .cpp .c

.cpp.$(OBJ) :
	$(CXX) -c `wx-config --cxxflags` -o $@ $<

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CXX) -o $(PROGRAM) $(OBJECTS) `wx-config --libs`

clean:
	rm -f *.$(OBJ) $(PROGRAM)
