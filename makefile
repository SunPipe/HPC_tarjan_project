.PHONY: all clean test compile0 compile1 compile2 compile3 test0 test1 test2 test3

all: directories compile0 compile1 compile2 compile3

clean:
	rm -rf ./Build/*
	rm -rf ./Informations
	rm -rf ./Results
	rm -rf ./ResultSCC
	rm -rf ./Tables
	rm -rf ./Plots

test: test0 test1 test2 test3 statistics

types=7
it=75
mpiMax=9
ompMax=4
dimensions=3
seed=93822

directories:
	python3 ./Source/directories.py

compile0:
	gcc -c -o ./Build/TArray.o ./Source/TArray.c -O0
	gcc -c -o ./Build/THT.o ./Source/THT.c -O0
	gcc -c -o ./Build/TInfo.o ./Source/TInfo.c -O0
	gcc -c -o ./Build/TList.o ./Source/TList.c -O0
	gcc -c -o ./Build/TStack.o ./Source/TStack.c -O0
	gcc -c -o ./Build/graphGenerator.o ./Source/graphGenerator.c -O0

	gcc -c -o ./Build/Sequential_tarjan.o ./Source/Sequential_tarjan.c -O0
	mpicc -c -o ./Build/MPI_tarjan.o ./Source/MPI_tarjan.c -O0
	mpicc -c -o ./Build/MPI_OMP_tarjan.o ./Source/MPI_OMP_tarjan.c -fopenmp -O0

	gcc -o ./Build/Sequential_tarjan0.exe ./Build/Sequential_tarjan.o ./Build/TArray.o ./Build/TStack.o ./Build/graphGenerator.o -O0
	mpicc -o ./Build/MPI_tarjan0.exe ./Build/MPI_tarjan.o ./Build/TArray.o ./Build/THT.o ./Build/TInfo.o ./Build/TList.o ./Build/TStack.o ./Build/graphGenerator.o -O0
	mpicc -o ./Build/MPI_OMP_tarjan0.exe ./Build/MPI_OMP_tarjan.o ./Build/TArray.o ./Build/THT.o ./Build/TInfo.o ./Build/TList.o ./Build/TStack.o ./Build/graphGenerator.o -fopenmp -O0

compile1:
	gcc -c -o ./Build/TArray.o ./Source/TArray.c -O1
	gcc -c -o ./Build/THT.o ./Source/THT.c -O1
	gcc -c -o ./Build/TInfo.o ./Source/TInfo.c -O1
	gcc -c -o ./Build/TList.o ./Source/TList.c -O1
	gcc -c -o ./Build/TStack.o ./Source/TStack.c -O1
	gcc -c -o ./Build/graphGenerator.o ./Source/graphGenerator.c -O1

	gcc -c -o ./Build/Sequential_tarjan.o ./Source/Sequential_tarjan.c -O1
	mpicc -c -o ./Build/MPI_tarjan.o ./Source/MPI_tarjan.c -O1
	mpicc -c -o ./Build/MPI_OMP_tarjan.o ./Source/MPI_OMP_tarjan.c -fopenmp -O1

	gcc -o ./Build/Sequential_tarjan1.exe ./Build/Sequential_tarjan.o ./Build/TArray.o ./Build/TStack.o ./Build/graphGenerator.o -O1
	mpicc -o ./Build/MPI_tarjan1.exe ./Build/MPI_tarjan.o ./Build/TArray.o ./Build/THT.o ./Build/TInfo.o ./Build/TList.o ./Build/TStack.o ./Build/graphGenerator.o -O1
	mpicc -o ./Build/MPI_OMP_tarjan1.exe ./Build/MPI_OMP_tarjan.o ./Build/TArray.o ./Build/THT.o ./Build/TInfo.o ./Build/TList.o ./Build/TStack.o ./Build/graphGenerator.o -fopenmp -O1

compile2:
	gcc -c -o ./Build/TArray.o ./Source/TArray.c -O2
	gcc -c -o ./Build/THT.o ./Source/THT.c -O2
	gcc -c -o ./Build/TInfo.o ./Source/TInfo.c -O2
	gcc -c -o ./Build/TList.o ./Source/TList.c -O2
	gcc -c -o ./Build/TStack.o ./Source/TStack.c -O2
	gcc -c -o ./Build/graphGenerator.o ./Source/graphGenerator.c -O2

	gcc -c -o ./Build/Sequential_tarjan.o ./Source/Sequential_tarjan.c -O2
	mpicc -c -o ./Build/MPI_tarjan.o ./Source/MPI_tarjan.c -O2
	mpicc -c -o ./Build/MPI_OMP_tarjan.o ./Source/MPI_OMP_tarjan.c -fopenmp -O2

	gcc -o ./Build/Sequential_tarjan2.exe ./Build/Sequential_tarjan.o ./Build/TArray.o ./Build/TStack.o ./Build/graphGenerator.o -O2
	mpicc -o ./Build/MPI_tarjan2.exe ./Build/MPI_tarjan.o ./Build/TArray.o ./Build/THT.o ./Build/TInfo.o ./Build/TList.o ./Build/TStack.o ./Build/graphGenerator.o -O2
	mpicc -o ./Build/MPI_OMP_tarjan2.exe ./Build/MPI_OMP_tarjan.o ./Build/TArray.o ./Build/THT.o ./Build/TInfo.o ./Build/TList.o ./Build/TStack.o ./Build/graphGenerator.o -fopenmp -O2

compile3:
	gcc -c -o ./Build/TArray.o ./Source/TArray.c -O3
	gcc -c -o ./Build/THT.o ./Source/THT.c -O3
	gcc -c -o ./Build/TInfo.o ./Source/TInfo.c -O3
	gcc -c -o ./Build/TList.o ./Source/TList.c -O3
	gcc -c -o ./Build/TStack.o ./Source/TStack.c -O3
	gcc -c -o ./Build/graphGenerator.o ./Source/graphGenerator.c -O3

	gcc -c -o ./Build/Sequential_tarjan.o ./Source/Sequential_tarjan.c -O3
	mpicc -c -o ./Build/MPI_tarjan.o ./Source/MPI_tarjan.c -O3
	mpicc -c -o ./Build/MPI_OMP_tarjan.o ./Source/MPI_OMP_tarjan.c -fopenmp -O3

	gcc -o ./Build/Sequential_tarjan3.exe ./Build/Sequential_tarjan.o ./Build/TArray.o ./Build/TStack.o ./Build/graphGenerator.o -O3
	mpicc -o ./Build/MPI_tarjan3.exe ./Build/MPI_tarjan.o ./Build/TArray.o ./Build/THT.o ./Build/TInfo.o ./Build/TList.o ./Build/TStack.o ./Build/graphGenerator.o -O3
	mpicc -o ./Build/MPI_OMP_tarjan3.exe ./Build/MPI_OMP_tarjan.o ./Build/TArray.o ./Build/THT.o ./Build/TInfo.o ./Build/TList.o ./Build/TStack.o ./Build/graphGenerator.o -fopenmp -O3

statistics:
	python3 ./Source/analize.py
	python3 ./Source/tableCreator.py
	python3 ./Source/plotCreator.py

test0:
	opt=0 ; \
	nodes=500 ; \
	dimension=1; while [ $$dimension -le $(dimensions) ] ; do \
		type=0; while [ $$type -le $$(($(types)-5)) ] ; do \
			omp=1; mpi=0; i=1 ; while [ $$i -le $(it) ] ; do \
				edges=$$((nodes/50)) ; \
				./Build/Sequential_tarjan0.exe $$nodes $$edges $$type $(seed) $$opt ; \
				while [ $$mpi -lt $$(($(mpiMax)-1)) ] ; do \
					mpi=$$((mpi*2)); \
					var=$$((mpi+1)); \
					mpirun -np $$var ./Build/MPI_tarjan0.exe $$nodes $$edges $$type $(seed) $$opt ; \
					while [ $$omp -le $(ompMax) ] ; do \
						export OMP_NUM_THREADS=$$omp && \
						mpirun -np $$var ./Build/MPI_OMP_tarjan0.exe $$nodes $$edges $$type $(seed) $$opt -fopenmp; \
						omp=$$((omp+1)); \
					done; \
					omp=1; \
					if [ $$mpi -eq 0 ] ; then \
						mpi=1; \
					fi ; \
				done; \
				i=$$((i+1)); \
				mpi=1; \
				omp=1; \
			done; \
			type=$$((type+1)); \
		done; \
		nodes=$$((nodes+10000)); \
		dimension=$$((dimension+1)); \
	done; \
	nodes=2000 ; \
	dimension=1; while [ $$dimension -le $(dimensions) ] ; do \
		type=3; while [ $$type -le $(types) ] ; do \
			omp=1; mpi=1; i=1 ; while [ $$i -le $(it) ] ; do \
				edges=$$((nodes/1000)) ; \
				./Build/Sequential_tarjan0.exe $$nodes $$edges $$type $(seed) $$opt ; \
				while [ $$mpi -le $(mpiMax) ] ; do \
					mpi=$$((mpi*2)); \
					var=$$((mpi+1)); \
					mpirun -np $$var ./Build/MPI_tarjan0.exe $$nodes $$edges $$type $(seed) $$opt ; \
					while [ $$omp -le $(ompMax) ] ; do \
						export OMP_NUM_THREADS=$$omp && \
						mpirun -np $$var ./Build/MPI_OMP_tarjan0.exe $$nodes $$edges $$type $(seed) $$opt -fopenmp; \
						omp=$$((omp+1)); \
					done; \
					omp=1; \
					if [ $$mpi -eq 0 ] ; then \
						mpi=1; \
					fi ; \
				done; \
				i=$$((i+1)); \
				mpi=1; \
				omp=1; \
			done; \
			type=$$((type+1)); \
		done; \
		nodes=$$((nodes+35000)); \
		dimension=$$((dimension+1)); \
	done;

test1:
	opt=1 ; \
	nodes=500 ; \
	dimension=1; while [ $$dimension -le $(dimensions) ] ; do \
		type=0; while [ $$type -le $$(($(types)-5)) ] ; do \
			omp=1; mpi=0; i=1 ; while [ $$i -le $(it) ] ; do \
				edges=$$((nodes/50)) ; \
				./Build/Sequential_tarjan1.exe $$nodes $$edges $$type $(seed) $$opt ; \
				while [ $$mpi -lt $$(($(mpiMax)-1)) ] ; do \
					mpi=$$((mpi*2)); \
					var=$$((mpi+1)); \
					mpirun -np $$var ./Build/MPI_tarjan1.exe $$nodes $$edges $$type $(seed) $$opt ; \
					while [ $$omp -le $(ompMax) ] ; do \
						export OMP_NUM_THREADS=$$omp && \
						mpirun -np $$var ./Build/MPI_OMP_tarjan1.exe $$nodes $$edges $$type $(seed) $$opt -fopenmp; \
						omp=$$((omp+1)); \
					done; \
					omp=1; \
					if [ $$mpi -eq 0 ] ; then \
						mpi=1; \
					fi ; \
				done; \
				i=$$((i+1)); \
				mpi=1; \
				omp=1; \
			done; \
			type=$$((type+1)); \
		done; \
		nodes=$$((nodes+10000)); \
		dimension=$$((dimension+1)); \
	done; \
	nodes=2000 ; \
	dimension=1; while [ $$dimension -le $(dimensions) ] ; do \
		type=3; while [ $$type -le $(types) ] ; do \
			omp=1; mpi=1; i=1 ; while [ $$i -le $(it) ] ; do \
				edges=$$((nodes/1000)) ; \
				./Build/Sequential_tarjan1.exe $$nodes $$edges $$type $(seed) $$opt ; \
				while [ $$mpi -le $(mpiMax) ] ; do \
					mpi=$$((mpi*2)); \
					var=$$((mpi+1)); \
					mpirun -np $$var ./Build/MPI_tarjan1.exe $$nodes $$edges $$type $(seed) $$opt ; \
					while [ $$omp -le $(ompMax) ] ; do \
						export OMP_NUM_THREADS=$$omp && \
						mpirun -np $$var ./Build/MPI_OMP_tarjan1.exe $$nodes $$edges $$type $(seed) $$opt -fopenmp; \
						omp=$$((omp+1)); \
					done; \
					omp=1; \
					if [ $$mpi -eq 0 ] ; then \
						mpi=1; \
					fi ; \
				done; \
				i=$$((i+1)); \
				mpi=1; \
				omp=1; \
			done; \
			type=$$((type+1)); \
		done; \
		nodes=$$((nodes+35000)); \
		dimension=$$((dimension+1)); \
	done;

test2:
	opt=2 ; \
	nodes=500 ; \
	dimension=1; while [ $$dimension -le $(dimensions) ] ; do \
		type=0; while [ $$type -le $$(($(types)-5)) ] ; do \
			omp=1; mpi=0; i=1 ; while [ $$i -le $(it) ] ; do \
				edges=$$((nodes/50)) ; \
				./Build/Sequential_tarjan2.exe $$nodes $$edges $$type $(seed) $$opt ; \
				while [ $$mpi -lt $$(($(mpiMax)-1)) ] ; do \
					mpi=$$((mpi*2)); \
					var=$$((mpi+1)); \
					mpirun -np $$var ./Build/MPI_tarjan2.exe $$nodes $$edges $$type $(seed) $$opt ; \
					while [ $$omp -le $(ompMax) ] ; do \
						export OMP_NUM_THREADS=$$omp && \
						mpirun -np $$var ./Build/MPI_OMP_tarjan2.exe $$nodes $$edges $$type $(seed) $$opt -fopenmp; \
						omp=$$((omp+1)); \
					done; \
					omp=1; \
					if [ $$mpi -eq 0 ] ; then \
						mpi=1; \
					fi ; \
				done; \
				i=$$((i+1)); \
				mpi=1; \
				omp=1; \
			done; \
			type=$$((type+1)); \
		done; \
		nodes=$$((nodes+10000)); \
		dimension=$$((dimension+1)); \
	done; \
	nodes=2000 ; \
	dimension=1; while [ $$dimension -le $(dimensions) ] ; do \
		type=3; while [ $$type -le $(types) ] ; do \
			omp=1; mpi=1; i=1 ; while [ $$i -le $(it) ] ; do \
				edges=$$((nodes/1000)) ; \
				./Build/Sequential_tarjan2.exe $$nodes $$edges $$type $(seed) $$opt ; \
				while [ $$mpi -le $(mpiMax) ] ; do \
					mpi=$$((mpi*2)); \
					var=$$((mpi+1)); \
					mpirun -np $$var ./Build/MPI_tarjan2.exe $$nodes $$edges $$type $(seed) $$opt ; \
					while [ $$omp -le $(ompMax) ] ; do \
						export OMP_NUM_THREADS=$$omp && \
						mpirun -np $$var ./Build/MPI_OMP_tarjan2.exe $$nodes $$edges $$type $(seed) $$opt -fopenmp; \
						omp=$$((omp+1)); \
					done; \
					omp=1; \
					if [ $$mpi -eq 0 ] ; then \
						mpi=1; \
					fi ; \
				done; \
				i=$$((i+1)); \
				mpi=1; \
				omp=1; \
			done; \
			type=$$((type+1)); \
		done; \
		nodes=$$((nodes+35000)); \
		dimension=$$((dimension+1)); \
	done;


test3:
	opt=3 ; \
	nodes=500 ; \
	dimension=1; while [ $$dimension -le $(dimensions) ] ; do \
		type=0; while [ $$type -le $$(($(types)-5)) ] ; do \
			omp=1; mpi=0; i=1 ; while [ $$i -le $(it) ] ; do \
				edges=$$((nodes/50)) ; \
				./Build/Sequential_tarjan3.exe $$nodes $$edges $$type $(seed) $$opt ; \
				while [ $$mpi -lt $$(($(mpiMax)-1)) ] ; do \
					mpi=$$((mpi*2)); \
					var=$$((mpi+1)); \
					mpirun -np $$var ./Build/MPI_tarjan3.exe $$nodes $$edges $$type $(seed) $$opt ; \
					while [ $$omp -le $(ompMax) ] ; do \
						export OMP_NUM_THREADS=$$omp && \
						mpirun -np $$var ./Build/MPI_OMP_tarjan3.exe $$nodes $$edges $$type $(seed) $$opt -fopenmp; \
						omp=$$((omp+1)); \
					done; \
					omp=1; \
					if [ $$mpi -eq 0 ] ; then \
						mpi=1; \
					fi ; \
				done; \
				i=$$((i+1)); \
				mpi=1; \
				omp=1; \
			done; \
			type=$$((type+1)); \
		done; \
		nodes=$$((nodes+10000)); \
		dimension=$$((dimension+1)); \
	done; \
	nodes=2000 ; \
	dimension=1; while [ $$dimension -le $(dimensions) ] ; do \
		type=3; while [ $$type -le $(types) ] ; do \
			omp=1; mpi=1; i=1 ; while [ $$i -le $(it) ] ; do \
				edges=$$((nodes/1000)) ; \
				./Build/Sequential_tarjan3.exe $$nodes $$edges $$type $(seed) $$opt ; \
				while [ $$mpi -le $(mpiMax) ] ; do \
					mpi=$$((mpi*2)); \
					var=$$((mpi+1)); \
					mpirun -np $$var ./Build/MPI_tarjan3.exe $$nodes $$edges $$type $(seed) $$opt ; \
					while [ $$omp -le $(ompMax) ] ; do \
						export OMP_NUM_THREADS=$$omp && \
						mpirun -np $$var ./Build/MPI_OMP_tarjan3.exe $$nodes $$edges $$type $(seed) $$opt -fopenmp; \
						omp=$$((omp+1)); \
					done; \
					omp=1; \
					if [ $$mpi -eq 0 ] ; then \
						mpi=1; \
					fi ; \
				done; \
				i=$$((i+1)); \
				mpi=1; \
				omp=1; \
			done; \
			type=$$((type+1)); \
		done; \
		nodes=$$((nodes+35000)); \
		dimension=$$((dimension+1)); \
	done;

