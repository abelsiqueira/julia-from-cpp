CARGS = -fPIC -g
JULIA_DIR = /opt/julias/julia-1.8.0
JLARGS = -I$(JULIA_DIR)/include/julia -L$(JULIA_DIR)/lib -Wl,-rpath,$(JULIA_DIR)/lib -ljulia
SRC = $(wildcard *.cpp)
EXE = $(SRC:cpp=exe)

.PHONY: clean all
echo:
	echo $(EXE)

all: $(EXE)

%.exe: %.cpp libmy_c_func.so libpoisson_mul.so
	g++ $(CARGS) $< $(JLARGS) -o $@

libmy_c_func.so:
	g++ $(CARGS) -c my_c_func.c -o my_c_func.o
	ld -shared my_c_func.o -o libmy_c_func.so

libpoisson_mul.so:
	g++ $(CARGS) -c poisson_mul.c -o poisson_mul.o
	ld -shared poisson_mul.o -o libpoisson_mul.so

clean:
	rm -f $(EXE)

purge: clean
	rm -f *.png
