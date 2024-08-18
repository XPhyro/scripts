VPATH = src

CXX := g++

warnings = -Wall -Wextra -Werror \
           -Wswitch-default -Wfloat-equal \
           -Wdisabled-optimization -Wsign-promo
libs = 

ifdef includeprefix
	include = -I$(includeprefix)
else
	include =
endif

CXXFLAGS_NOWARN = -O3 -DNDEBUG -std=c++2c -pedantic $(include)
CXXFLAGS = $(warnings) $(CXXFLAGS_NOWARN)
LDFLAGS = $(libs)

EXE = pathfinder

SRC = $(shell find src -name "*.cpp")
OBJ_ = $(SRC:src/%=%)
OBJ = $(OBJ_:.cpp=.o)

run: $(EXE)
	./pathfinder

$(EXE): $(OBJ)
	$(CXX) $(LDFLAGS) $^ -o $(EXE)

$(OBJ): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm -f -- $(OBJ) $(EXE)

.PHONY: run clean
