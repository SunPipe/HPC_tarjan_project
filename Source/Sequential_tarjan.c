
/* 
 * Course: High Performance Computing 2022/2023 
 * 
 * Lecturer: Francesco Moscato	fmoscato@unisa.it 
 *
 * Group: 
 * Intonti Giovanni	0622701994	g.intonti@studenti.unisa.it
 * Venditti Francesca	0622701989  f.venditti1@studenti.unisa.it
 * Vitale Antonio	0622701988  a.vitale112@studenti.unisa.it
 *
 * Provide a parallell version of the Tarjan's algorithm to find Strongly Connected Components in a Graph. 
 * The implementation MUST use an hibrid  message passing / shared memory paradigm. and has to be implemented by using MPI and openMP.  
 * Students MUST provide parallel processes on different nodes, and each process has to be parallelized by using Open MP (i.e.: MPI will spawn OPENMP-compiled processes). 
 * Students can choose the graph allocation method the prefer. They can eventually produce the graph directly in distributed memory (without store anything).
 * 
 *  This file is the sequential version of Tarjan algorithm to find strongly connected components 
 * 
 * Copyright (C) 2023 - All Rights Reserved 
 *
 * This file is part of CommonAssignmentMPIOpenMP 
 *
 * CommonAssignmentMPIOpenMP   is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation, either version 3 of the License, or 
 * (at your option) any later version. 
 *
 * CommonAssignmentMPIOpenMP   is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details. 
 *
 * You should have received a copy of the GNU General Public License 
 * along with CommonAssignmentMPIOpenMP . If not, see <http://www.gnu.org/licenses/>. 
 */

#include "../Headers/TStack.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include "../Headers/TArray.h"
#include <sys/time.h>
#include "../Headers/TStack.h"
#include "../Headers/graphGenerator.h"

/** @brief Function that returns the minimum of two integers
* @param a is the first integer
* @param b is the second integer 
* @return the minimum number between a and b */
int Min(int a, int b)
{

    return a < b ? a : b;
}

/** @brief Function that finds the strongly connected components (SCCs) of a given graph and save them on a file
* @param v is the current vertex being processed
* @param graph is the array of vertices representing the graph
* @param stack is the stack used for Tarjan's algorithm
* @param curr_index is a pointer to the current index being used for Tarjan's algorithm
* @param fp is the file pointer for printing the SCCs to a file
* This function uses Tarjan's algorithm to find the strongly connected components (SCCs) of a given graph and writes the result to a file. 
* It sets the current vertex's index and low_link values and pushes it to the stack. 
* Then it iterates through the vertex's neighbors, updating their index and low_link values recursively if they haven't been visited yet
* or updating the current vertex's low_link if they are in the stack. If the current vertex's index is equal to its low_link, it pops elements from the stack and prints them
* to the file until it reaches the current vertex.*/
void scc(int v, vertex *graph, TStack *stack, int *curr_index, FILE *fp)
{
    int i, c, n;

    graph[v].index = *curr_index;
    graph[v].low_link = *curr_index;
    ++(*curr_index);
    stackPush(stack, v);

    graph[v].in_stack = 1;

    for (i = 0, c = graph[v].deg; i < c; ++i)

        if (arrayGet(&graph[v].array, i) > 0)
        {
            n = arrayGet(&graph[v].array, i) - 1;

            if (graph[n].index == -1)
            {
                scc(n, graph, stack, curr_index, fp);
                graph[v].low_link = Min(graph[v].low_link, graph[n].low_link);
            }
            else if (graph[n].in_stack)
            {

                graph[v].low_link = Min(graph[v].low_link, graph[n].index);
            }
        }

    if (graph[v].index == graph[v].low_link)
    {
        fprintf(fp, "scc: ");
        while ((n = stackPop(stack)) != -1)
        {

            graph[n].in_stack = 0;
            fprintf(fp, "%d ", n + 1);
            if (n == v)
            {
                fprintf(fp, "\n");
                break;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    
    if(atoi(argv[1])<=0){ // check if the number of vertices in input is coherent
        printf("Number of vertices is wrong!\n");
        exit(1);
    }
    if(atoi(argv[2])<0){ // check if the edges number or replica number is coherent
        printf("Number of edges or replicas is wrong!\n");
        exit(1);
    }
    if(atoi(argv[3])<0 || atoi(argv[3])>7){ // check if the type of graph is coherent
        printf("Type of graph is wrong!\n");
        exit(1);
    }
    if(atoi(argv[5])<0 || atoi(argv[5])>3){ // check if the optimization is coherent
        printf("Optimization choice is wrong!\n");
        exit(1);
    }

    struct timeval all_start, all_end, start, end, total_time, graph_creation_time, tarjan_time;
    gettimeofday(&all_start, NULL); // all_start is used to record the start time of the entire program
    
    int numberOfVertices = atoi(argv[1]); // numberOfVertices is the number of vertices in the graph
    int EdgesOrReplicas = atoi(argv[2]); // EdgesOrReplicas are the number of edges for each vertex or the number of replicas depending on the type of graph
    int type = atoi(argv[3]); // type is the type of graph to be created (e.g. random, complete, etc.)
    int seed = atoi(argv[4]); // seed is the random seed generator used to guarantee the same graph in the tests


    vertex *graph;  // graph is an array of vertices representing the graph
    graph = (vertex *)malloc(numberOfVertices * sizeof(vertex)); // the graph is allocated as the numberOfVertices * size of the vertex struct
    // the graph is created with the makeGraphGeneral function declared in the graphGenerator library
    int edges = makeGraphGeneral(type, graph, numberOfVertices, EdgesOrReplicas, seed); // edges is the total number of edges in the graph
    assert(graph != NULL); // check if the graph is created correctly
    gettimeofday(&end, NULL); // stop the timer to measure the graph creation time
    timersub(&end, &all_start, &graph_creation_time);
    gettimeofday(&start, NULL); // start the timer to measure the Tarjan algorithm time

    FILE *fp;
    char path[200];
    sprintf(path, "ResultSCC/opt%d/type%d/Sequential%d_%d.txt", atoi(argv[5]), atoi(argv[3]), numberOfVertices, edges); // define the path where to store the SCCs

    char *filename = path;
    fp = fopen(filename, "w"); // open a file to write the SCCs founded by the algorithm on it

    TStack stack = stackCreate(); // stack used for the Tarjan algorithm

    int num = 0; // variable used as count_index in the Tarjan algorithm

    for (int j = 0; j < numberOfVertices; j++) // this for loop is used to find the SCCs of the graph, it calls the Tarjan algorithm from each vertex if that vertex is not already been founded by the algorithm itself
    {
        if (graph[j].index == -1)
        {
            scc(j, graph, &stack, &num, fp);
        }
    }
    
    fprintf(fp, "------------------------------------------------------\n");
    fclose(fp);
    stackDestroy(&stack); // after the computation we can destroy the stack, freeing the memory

    for (int vertexCounter = 0; vertexCounter < numberOfVertices; vertexCounter=vertexCounter+2) // for each vertex we then free the memory for the adiacency lists of each vertex 
    {
        arrayDestroy(&graph[vertexCounter].array);

        if(vertexCounter!=numberOfVertices-1)
            arrayDestroy(&graph[vertexCounter+1].array);
    }
    free(graph); // now we can free the graph itself
    gettimeofday(&end, NULL); // after all the computation and free of the memory allocated we can stop the timer and calculate the tarjan execution time
    timersub(&end, &start, &tarjan_time);
    gettimeofday(&all_end, NULL);
    timersub(&all_end, &all_start, &total_time);
    FILE *fp2; 
    char path2[200];
    sprintf(path2, "Informations/opt%d/type%d/%d_%d.csv", atoi(argv[5]), atoi(argv[3]), numberOfVertices, edges); // define the path where to store the times calculated for each phase, also for the communicational that in this case in 0.0 because is the sequential algorithm on just one processor
    char *filename2 = path2;
    fp2 = fopen(filename2, "a+");
    if (fp2 == NULL)
    {
        perror("Errore durante l'apertura del file");
    }
    fprintf(fp2, "Sequential;0;0; %ld.%06ld; %ld.%06ld; 0.0; %ld.%06ld;\n", (long int)total_time.tv_sec, (long int)total_time.tv_usec, (long int)graph_creation_time.tv_sec, (long int)graph_creation_time.tv_usec, (long int)tarjan_time.tv_sec, (long int)tarjan_time.tv_usec);
    fclose(fp2);
}
