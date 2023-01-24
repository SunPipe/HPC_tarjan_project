# CommonAssignmentOpenMP

## Dependencies

* MPICH
* Python3
* OMP

## How to run
Premise: The code for generating directories, tables and plots requires the python interpreter and a library to be installed, matplotlib, with the following command:
pip3 install matplotlib

1.	Navigate to the folder containing the makefile
2.	To clear previously obtained achievements and previous builds, enter the command
make clean
3.	To generate the necessary directories and compile and linking the various source codes, enter the command
make all
4.	To run the algorithm for making tests, producing results, measurements, graphs and tables, enter the command
make test
5.	To clear previously obtained achievements and previous builds, enter the command
make clean

The results of the algorithms can be viewed in the "ResultSCC" folder, divided by optimization and type of graph and size.
The execution times of the algorithms and their average values can be viewed respectively in the "Informations" and "Results" folders, divided by optimization and type of graph and size.
The results in graphical and tabular format can be viewed respectively in the "Plots" and "Tables" folders, divided by optimization and type of graph and size.

