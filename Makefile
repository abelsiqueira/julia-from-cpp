CARGS = -fPIC -g
JULIA_DIR = /opt/julias/julia-1.8.0
JULIA_ARGS = -I$(JULIA_DIR)/include/julia -L$(JULIA_DIR)/lib -Wl,-rpath,$(JULIA_DIR)/lib -ljulia

libmy_c_func.so:
	g++ $(CARGS) -c my_c_func.c -o my_c_func.o
	ld -shared my_c_func.o -o libmy_c_func.so

libpoisson_mul.so:
	g++ $(CARGS) -c poisson_mul.c -o poisson_mul.o
	ld -shared poisson_mul.o -o libpoisson_mul.so


main.exe: main.cpp
	g++ $(CARGS) $< $(JULIA_ARGS) -o $@

simple-sqrt2-example.exe: simple-sqrt2-example.cpp
	g++ $(CARGS) $< $(JULIA_ARGS) -o $@

better-sqrt2-example.exe: better-sqrt2-example.cpp
	g++ $(CARGS) $< $(JULIA_ARGS) -o $@

integrator.exe: integrator.cpp libmy_c_func.so
	g++ $(CARGS) $< $(JULIA_ARGS) -o $@

integrator-and-distributions.exe: integrator-and-distributions.cpp libmy_c_func.so
	g++ $(CARGS) $< $(JULIA_ARGS) -o $@

integrator-and-distributions2.exe: integrator-and-distributions2.cpp libmy_c_func.so
	g++ $(CARGS) $< $(JULIA_ARGS) -o $@

linear-algebra1.exe: linear-algebra1.cpp
	g++ $(CARGS) $< $(JULIA_ARGS) -o $@

linear-algebra2.exe: linear-algebra2.cpp
	g++ $(CARGS) $< $(JULIA_ARGS) -o $@

linear-algebra3.exe: linear-algebra3.cpp
	g++ $(CARGS) $< $(JULIA_ARGS) -o $@

linear-algebra4.exe: linear-algebra4.cpp libpoisson_mul.so
	g++ $(CARGS) $< $(JULIA_ARGS) -o $@
