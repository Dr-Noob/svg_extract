CXX=gcc

SANITY_FLAGS=-Wall -Wextra -Werror -fstack-protector-all -pedantic -Wno-unused -Wfloat-equal -Wshadow -Wpointer-arith -Wformat=2 -std=c99 -lm

ifeq ($(DEBUG), 1)
  CXXFLAGS=-g -O0 $(SANITY_FLAGS)
else
  CXXFLAGS=-O2 $(SANITY_FLAGS)
endif

all: svg_extract tsp2svg

svg_extract: svg_extract.c Makefile
	$(CXX) $(CXXFLAGS) svg_extract.c -lm -o $@

tsp2svg: tsp2svg.c Makefile
	$(CXX) $(CXXFLAGS) tsp2svg.c -lm -o $@

run: tsp2svg
	./tsp2svg indecisos/indecisos.svg indecisos/indecisos.cyc

clean:
	rm tsp2svg


