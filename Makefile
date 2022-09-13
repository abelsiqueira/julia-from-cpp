CARGS = -fPIC -g
JULIA_DIR = /opt/julias/julia-1.8.0
JLARGS = -I$(JULIA_DIR)/include/julia -L$(JULIA_DIR)/lib -Wl,-rpath,$(JULIA_DIR)/lib -ljulia
SRC = $(wildcard *.cpp)
EXE = $(SRC:cpp=exe)
LIBS = libmy_c_func.so libpoisson_mul.so

.PHONY: all clean purge default
default:
	@echo "targets: all, FILE.exe, clean, purge"

all: $(EXE)

%.exe: %.cpp $(LIBS) aux.o
	g++ $(CARGS) $< $(JLARGS) aux.o -o $@

%.o: %.cpp
	g++ $(CARGS) -c $< $(JLARGS) -o $@

lib%.so: %.o
	ld -shared $< -o $@

clean:
	rm -f $(EXE)

purge: clean
	rm -f *.png
