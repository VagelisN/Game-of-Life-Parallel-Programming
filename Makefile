all: MPI_gameoflife Hybrid_gameoflife Serial_gameoflife CUDA_gameoflife

CUDA_gameoflife:
	nvcc -g CUDA_game_of_life.cu -o CUDA_gameoflife

funcs.o: funcs.c funcs.h
	gcc -c funcs.c

MPI_gameoflife: MPI_game_of_life.o funcs.o
	mpicc -g3 MPI_game_of_life.o funcs.o -o MPI_gameoflife


Hybrid_gameoflife: Hybrid_game_of_life.o funcs.o
	mpicc -g3 -fopenmp Hybrid_game_of_life.o funcs.o -o Hybrid_gameoflife

Serial_gameoflife: Serial_game_of_life.o
	gcc -g3 Serial_game_of_life.o -o Serial_gameoflife

MPI_game_of_life.o: MPI_game_of_life.c
	mpicc -c MPI_game_of_life.c

Hybrid_game_of_life.o: Hybrid_game_of_life.c
	mpicc -fopenmp -c Hybrid_game_of_life.c

Serial_game_of_life.o: Serial_game_of_life.c
	gcc -c Serial_game_of_life.c



clean:
	rm *.o MPI_gameoflife Hybrid_gameoflife Serial_gameoflife CUDA_gameoflife
