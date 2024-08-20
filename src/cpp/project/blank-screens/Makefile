VPATH = src

CXX := g++

warnings = -Wall -Wextra -Werror \
           -Wswitch-default -Wfloat-equal \
           -Wdisabled-optimization -Wsign-promo
libs = -lX11 -lXrandr

ifdef includeprefix
	include = -I$(includeprefix)
else
	include =
endif

CXXFLAGS = $(warnings) -O3 -DNDEBUG -std=c++2c -pedantic -fpermissive $(include)
LDFLAGS = $(libs)

EXE = blank-screens

SRC = $(shell find src -name "*.cpp")
OBJ_ = $(SRC:src/%=%)
OBJ = $(OBJ_:.cpp=.o)

$(EXE): $(OBJ)
	$(CXX) $(LDFLAGS) $^ -o $(EXE)

$(OBJ): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm -f -- $(OBJ) $(EXE)

.PHONY: run clean
