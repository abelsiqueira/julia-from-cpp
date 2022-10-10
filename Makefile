CARGS = -fPIC -g
JULIA_DIR = /opt/julias/julia-1.8.0
JLARGS = -I$(JULIA_DIR)/include/julia -L$(JULIA_DIR)/lib -Wl,-rpath,$(JULIA_DIR)/lib -ljulia
SRC = $(filter-out aux.cpp my_c_func.cpp poisson_mul.cpp Normal.cpp, $(wildcard *.cpp))
EXE = $(SRC:cpp=exe)
LIBS = libmy_c_func.so libpoisson_mul.so
OBJS = aux.o Normal.o

.PRECIOUS: $(LIBS)

.PHONY: all clean purge default debug
default:
	@echo "targets: all, FILE.exe, clean, purge"

debug:
	@echo "SRC: " $(SRC)

all: $(EXE)

%.exe: %.cpp $(LIBS) $(OBJS)
	g++ $(CARGS) $< $(JLARGS) $(OBJS) -o $@

%.o: %.cpp
	g++ $(CARGS) -c $< $(JLARGS) -o $@

lib%.so: %.o
	ld -shared $< -o $@

clean:
	rm -f $(EXE) *.o *.so

purge: clean
	rm -f *.png
