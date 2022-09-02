CARGS = -fPIC -g
JULIA_DIR = /opt/julias/julia-1.8.0
JULIA_ARGS = -I$(JULIA_DIR)/include/julia -L$(JULIA_DIR)/lib -Wl,-rpath,$(JULIA_DIR)/lib -ljulia

libmy_c_func.so:
	g++ $(CARGS) -c my_c_func.c -o my_c_func.o
	ld -shared my_c_func.o -o libmy_c_func.so

libpoisson_mul.so:
	g++ $(CARGS) -c poisson_mul.c -o poisson_mul.o
	ld -shared poisson_mul.o -o libpoisson_mul.so

main:
	g++ $(CARGS) main.cpp $(JULIA_ARGS) -o main

simple-sqrt2-example:
	g++ $(CARGS) simple-sqrt2-example.cpp $(JULIA_ARGS) -o simple-sqrt2-example

better-sqrt2-example:
	g++ $(CARGS) better-sqrt2-example.cpp $(JULIA_ARGS) -o better-sqrt2-example

integrator: libmy_c_func.so
	g++ $(CARGS) integrator.cpp $(JULIA_ARGS) -o integrator

integrator-and-distributions: libmy_c_func.so
	g++ $(CARGS) integrator-and-distributions.cpp $(JULIA_ARGS) -o integrator-and-distributions

integrator-and-distributions2: libmy_c_func.so
	g++ $(CARGS) integrator-and-distributions2.cpp $(JULIA_ARGS) -o integrator-and-distributions2

linear-algebra1:
	g++ $(CARGS) linear-algebra1.cpp $(JULIA_ARGS) -o linear-algebra1

linear-algebra2:
	g++ $(CARGS) linear-algebra2.cpp $(JULIA_ARGS) -o linear-algebra2

linear-algebra3:
	g++ $(CARGS) linear-algebra3.cpp $(JULIA_ARGS) -o linear-algebra3

linear-algebra4: libpoisson_mul.so
	g++ $(CARGS) linear-algebra4.cpp $(JULIA_ARGS) -o linear-algebra4
