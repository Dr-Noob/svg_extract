CXX=gcc

SANITY_FLAGS=-Wall -Wextra -Werror -fstack-protector-all -pedantic -Wno-unused -Wfloat-equal -Wshadow -Wpointer-arith -Wformat=2 -std=c99

ifeq ($(DEBUG), 1)
  CXXFLAGS=-g -O0 $(SANITY_FLAGS)
else
  CXXFLAGS=-O2 $(SANITY_FLAGS)
endif

svg_extract: svg_extract.c Makefile
	$(CXX) $(CXXFLAGS) svg_extract.c -lm -o $@

clean:
	rm svg_extract


