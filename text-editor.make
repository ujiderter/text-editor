# Makefile for Text Editor program
# Works on both Linux and Windows (with MinGW or MSVC)

ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
    ifeq ($(CXX),cl)
        RM := del /Q
        EXE := text_editor.exe
        CXXFLAGS := /EHsc /std:c++17 /W4
        LDFLAGS :=
    else
        RM := del /Q
        EXE := text_editor.exe
        CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic
        LDFLAGS :=
    endif
else
    DETECTED_OS := $(shell uname -s)
    RM := rm -f
    EXE := text_editor
    CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic
    LDFLAGS :=
endif

CXX ?= g++

SRC := text_editor.cpp

all: $(EXE)

$(EXE): $(SRC)
ifeq ($(CXX),cl)
	$(CXX) $(CXXFLAGS) $(SRC) /Fe:$(EXE) $(LDFLAGS)
else
	$(CXX) $(CXXFLAGS) -o $(EXE) $(SRC) $(LDFLAGS)
endif

clean:
	$(RM) $(EXE)

ifneq ($(CXX),cl)
debug: CXXFLAGS += -g -O0 -DDEBUG
debug: all
endif

release: CXXFLAGS += -O3 -DNDEBUG
release: all

.PHONY: all clean debug release
