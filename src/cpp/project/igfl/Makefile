VPATH = src

CXX := g++

warnings = -Wall -Wextra -Werror \
           -Wswitch-default -Wfloat-equal \
           -Wdisabled-optimization -Wsign-promo
libs = -lm -lncurses

CXXFLAGS_LIB = -Wno-deprecated-enum-enum-conversion
CXXFLAGS_NOWARN = -O3 -DNDEBUG -std=c++23 -pedantic -Ilib -I$(includeprefix)
CXXFLAGS = $(warnings) $(CXXFLAGS_NOWARN)
LDFLAGS = $(libs)

EXE = igfl

SRC = $(shell find src -name "*.cpp" -not -path "*/lib/*")
OBJ_ = $(SRC:src/%=%)
OBJ = $(OBJ_:.cpp=.o)

LIB_SRC = $(shell find src -name "*.cpp" -path "*/lib/*")
LIB_OBJ_ = $(LIB_SRC:src/%=%)
LIB_OBJ = $(LIB_OBJ_:.cpp=.o)

run: $(EXE)
	./igfl

$(EXE): $(OBJ) $(LIB_OBJ)
	$(CXX) $(LDFLAGS) $^ -o $(EXE)

$(OBJ): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(LIB_OBJ): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS_NOWARN) $(CXXFLAGS_LIB) $< -o $@

clean: ### clean up objects
	rm -f -- $(OBJ) $(LIB_OBJ) $(EXE)

.PHONY: clean
