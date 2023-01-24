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
 * This file is the hybrid MPI / OpenMP version of Tarjan algorithm to find strongly connected components
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <mpi.h>
#include <omp.h>
#include "../Headers/TArray.h"
#include "../Headers/THT.h"
#include <sys/time.h>
#include "../Headers/TStack.h"
#include "../Headers/graphGenerator.h"

/** @brief Function that calculates the log base 2 of a given number
 * @param num is the number to calculate the log base 2 of
 * @return the log base 2 of the input number*/
int login2(int num)
{
    if (num > 1)
    {
        return 1 + login2(num / 2);
    }
    return 0;
}

/** @brief Function that returns the minimum of two integers
 * @param a is the first integer
 * @param b is the second integer */
int Min(int a, int b)
{

    return a < b ? a : b;
}

/** @brief Function that finds the strongly connected components (SCCs) of a given graph, and store the result in a stack (used just for the last iteration without openMP)
 * @param v is the current vertex being processed
 * @param graph is the array of vertices representing the graph
 * @param stack is the stack used for Tarjan's algorithm to keep track of the current SCC
 * @param temp is the stack to store the resulting SCCs
 * @param curr_index is a pointer to the current index being used for Tarjan's algorithm
 * @This function uses Tarjan's algorithm to find the strongly connected components (SCCs) of a given graph, and store the result in a stack called temp.
 * It performs some additional logic to handle special cases where the vertex is a supernodo and its name is not -1
 * It also not consider the case where an SCC is composed by just one node, in that case the algorithm not consider it as an SCC.*/
void sccFinale(int v, vertex *graph, TStack *stack, TStack *temp, int *curr_index)
{
    int i, c, n;

    if (graph[v].supernodo == 0) // if the vertex has the flag supernodo equals to 1, it's ignored by the algorithm
    {
        graph[v].index = *curr_index;
        graph[v].low_link = *curr_index;
        ++(*curr_index);
        stackPush(stack, v);
        graph[v].in_stack = 1;

        for (i = 0, c = graph[v].deg; i < c; ++i) /* it iterates through the vertex's neighbors, updating their index and low_link values recursively if they haven't been visited yet
                                                   * or updating the current vertex's low_link if they are in the stack. If the current vertex's index is equal to its low_link, it pops elements from the stack and save it in a stack
                                                   * until it reaches the current vertex. Then it add a -1 value to the stack as a convention to ensure division between the SCCs founded.*/
        {

            n = arrayGet(&graph[v].array, i);
            if (graph[n - 1].supernodo == 1 && graph[n - 1].nome != -1) /* if one vertex have in his adiacency list a vertex that is marked with the flag supernodo=1, and it must
            be considered by the algorithm ( because it's name it's not equal to -1 ), we, as a convention, consider the only vertex contained in the adiacency list of this vertex
            This is because when a new vertex, founded as an SCC, is inserted in the graph, all the vertices that compose this SCC are marked with the flag supernodo=1, it means that
            they compose another vertex, and their adiacency list is cleaned up, and updated with just a link to this new vertex, so that when, in the algorithm, some vertex found in his
            adiacency list this vertex knows that he has to consider the link in his adiacency list then the vertex itself*/
            {
                n = arrayGet(&graph[n - 1].array, 0); // this is made taking from the vertex that has supernodo=1, the vertex founded in his adiacency list, and considering it instead of this vertex
            }
            n = n - 1;
            if (graph[n].nome != -1)
            {
                if (graph[n].index == -1)
                {
                    sccFinale(n, graph, stack, temp, curr_index);
                    graph[v].low_link = Min(graph[v].low_link, graph[n].low_link);
                }
                else if (graph[n].in_stack)
                {
                    graph[v].low_link = Min(graph[v].low_link, graph[n].index);
                }
            }
        }

        if (graph[v].index == graph[v].low_link)
        {

            int length = stackLen(temp);
            while ((n = stackPop(stack)) != -1)
            {
                graph[n].in_stack = 0;

                stackPush(temp, n + 1);

                if (n == v)
                {
                    break;
                }
            }

            if (stackLen(temp) == length + 1)
            {
                stackPop(temp);
            }
            else
            {
                stackPush(temp, -1);
            }
        }
    }
}

/** @brief Function that finds the strongly connected components (SCCs) of a given graph, and store the result in a stack
 * @param v is the current vertex being processed
 * @param graph is the array of vertices representing the graph
 * @param stack is the stack used for Tarjan's algorithm to keep track of the current SCC
 * @param temp is the stack to store the resulting SCCs
 * @param curr_index is a pointer to the current index being used for Tarjan's algorithm
 * @param divisions is an array that store the range of vertex that each OMP thread should consider
 * @This function uses Tarjan's algorithm to find the strongly connected components (SCCs) of a given graph, and store the result in a stack called temp.
 * It performs some additional logic to handle special cases where the vertex is a supernodo and its name is not -1
 * It also not consider the case where an SCC is composed by just one node, in that case the algorithm not consider it as an SCC.
 * Every OMP thread has his part of the current partition of the graph, so to store the SCC founded from each thread in the temp stack, given that is a shared resource,
 * each thread needs mutual exclusion to guarantee consistency */
void scc(int v, vertex *graph, TStack *stack, TStack *temp, int *curr_index, int *divisions)
{
    int i, c, n;

    if (graph[v].supernodo == 0) // if the vertex has the flag supernodo equals to 1, it's ignored by the algorithm
    {
        graph[v].index = *curr_index;
        graph[v].low_link = *curr_index;
        ++(*curr_index);
        stackPush(stack, v);
        graph[v].in_stack = 1;

        for (i = 0, c = graph[v].deg; i < c; ++i) /* it iterates through the vertex's neighbors, updating their index and low_link values recursively if they haven't been visited yet
                                                   * or updating the current vertex's low_link if they are in the stack. If the current vertex's index is equal to its low_link, it pops elements from the stack and save it in a stack
                                                   * until it reaches the current vertex. Then it add a -1 value to the stack as a convention to ensure division between the SCCs founded.*/
        {

            n = arrayGet(&graph[v].array, i);
            if (graph[n - 1].supernodo == 1 && graph[n - 1].nome != -1) /* if one vertex have in his adiacency list a vertex that is marked with the flag supernodo =1, and it must
             not be ignorated by the algorithm ( because it's name it's not equal to -1 ), we, as a convention, consider the only vertex contained in the adiacency list of this vertex
             This is before when a new vertex, founded as an SCC, is inserted in the graph, all the vertices that compose this SCC are marked with the flag supernodo=1, it means that
             they compose another vertex, and their adiacency list is cleaned up, and updated with just a link to this new vertex, so that when, in the algorithm, some vertex found in his
             adiacency list this vertex knows that he has to consider the link in his adiacency list then the vertex itself*/
            {
                n = arrayGet(&graph[n - 1].array, 0); // this is made taking from the vertex that has supernodo = 1, the vertex founded in his link, and considering it instead of this vertex
            }
            n = n - 1;
            if (graph[n].nome != -1 && (n >= divisions[omp_get_thread_num()] && n < divisions[omp_get_thread_num() + 1])) // each OMP threads check if the current vertex is in the thread range,
            // so if it has to be ignorated or not
            {
                if (graph[n].index == -1)
                {
                    scc(n, graph, stack, temp, curr_index, divisions);
                    graph[v].low_link = Min(graph[v].low_link, graph[n].low_link);
                }
                else if (graph[n].in_stack)
                {
                    graph[v].low_link = Min(graph[v].low_link, graph[n].index);
                }
            }
        }

        if (graph[v].index == graph[v].low_link)
        {
#pragma omp critical // start of the critical region where each thread write his SCC in the stack
            {
                int length = stackLen(temp);
                while ((n = stackPop(stack)) != -1)
                {
                    graph[n].in_stack = 0;

                    stackPush(temp, n + 1);

                    if (n == v)
                    {
                        break;
                    }
                }

                if (stackLen(temp) == length + 1)
                {
                    stackPop(temp);
                }
                else
                {
                    stackPush(temp, -1);
                }
            }
        }
    }
}

/** @brief Function that update the graph with new vertex that are SCCs founded in the Tarjan algorithm
 * @param graph is the array of vertices representing the graph
 * @param sccFounded is an array containing the SCC founded from the Tarjan algorithm written as follow: for example if the SCC is made of vertex 1,2 and 3 => the sccFounded
 * array will contain [1,2,3,-1] where the -1 is a convention used to guarantee division between SCCs founded
 * @param num is the length of the sccFounded array
 * @param hashtable is an hashtable mapping each SCC founded, so new vertex added to the graph, with the vertices that compose that SCC.
 * @param numberOfVertices is the max number of vertices that could be in the graph
 * @param rank is the index given to the MPI process that calls this function
 * @param iterazione is the algorithm step in which a process calls this function
 * @param size is the number of MPI processes alive at the current run, except the master scheduler
 * @param initialVertices is the number of vertex created originally in the graph without any update done yet
 * @return the number of new vertices created
 * @This function take in input the SCCs founded at one iteration of the program and by one rank of the MPI process and update the graph adding this new node at it, updating his
 * adiacency list combinating the ones of the "children" nodes and adding it on the hashtable. Moreover is updated, for the vertex that compose that SCC, the adjacency list, cleaning it
 * and adding just the SCC node that they compose and updating the "supernodo" flag at 1, so that it is known that finding this node in the algorithm means to consider the only node
 * that is in his adiacency list as if it was the node itself.
 * Please note that if an SCC is composed by 1,2 and 3, and his name is 20, in the hashtable you'll find 20 -> 1,2,3 and updating vertex 1,2,3 with supernodo=1 and add just
 * the link to 20. Then if is founded another SCC composed by 20 and 4 named 21, you'll find in the hashtable 21 -> 1,2,3,4 because for each vertex composing the SCC the function first check if itself
 * is composed by more vertex (so was an SCC founded in previous step of the algorithm), and if is true his children are taken and added to the key 21 in the hashtable instead of the
 * vertex itself that it is also removed from the hashtable and are updated his children links, other then the link of the vertex 4, with the link to 21.
 * The function than set the flag supernodo=1 to the vertex 4 and not update it to the others because it was already setted to 1, because they was already part of another SCC.*/
int updateGraph(vertex *graph, int *sccFounded, int num, THT *hashtable, int numberOfVertices, int rank, int iterazione, int size, int initialVertices, int *max)
{
    TArray currentSCC = arrayCreate(0); // array for fetching the vertices belonging to an SCC
    TArray temporaneo = arrayCreate(0); // array for storing the simple vertices that compose an SCC
    // For simple vertex we mean a vertex that belong to the original graph, so was not created in a step of the Tarjan algorithm and doesn't contain any simple vertex
    int *allAdiancentList = (int *)calloc(sizeof(int), numberOfVertices); // array used to store the adiacency lists of the vertices composing an SCC:
    // this array is used as follows: for every vertex in the adiacency list of the vertices composing one SCC, it's setted to 1 the index in the array related to that vertex,
    // when the SCC is complete and it has to be added the new vertex, are first resetted to 0, so ignorated, the links to the vertex itself, so the links pointing at his children.
    // After that it is added to this new vertex his adiacency list, made up by combining the children ones, and resetting to 0 the array so it's ready for the next iteration,
    // and the next SCC. This decision was taken because using a List or a dinamic array to store the adiacency lists of the children of an SCC, since there is the needs to add and delete often from this structure, was onerous
    // a better solution was to use this ploy to manage the adiacency list.

    TArray newSccList;

    int numberOfScc = 0; // variable to store the number of new vertices created by the function

    int startingValue = initialVertices + (iterazione * initialVertices / 2) + ((rank - 1) * ((((float)initialVertices) / (size * 2)))); // variable used to guarantee that each
    // node at each iteration of the program creates unique name for new vertices added to the graph

    int storedValue = startingValue;

    for (int j = 0; j < num; j++)
    {
        if (sccFounded[j] == -1)
        {
            /* when the number -1 is founded in the array, it means that all the vertices composing one SCC are founded and added to the array. Now it can be created a new vertex
            made up by this vertices.
            */
            numberOfScc++; // first its updated the number of vertex created
            // it is cleaned up the array for storing the simple vertices that made up the SCC
            arrayDestroy(&temporaneo);
            temporaneo = arrayCreate(0);
            for (int i = 0; i < arrayLength(&currentSCC); i++) // here it is checked if one, or more than one, of the vertex that compose the SCC is itself an SCC founded in previous
            {                                                  // steps of the algorithm, taking his simple nodes from the hashtable and adding them temporarily to an array. If the node is a simple vertex it's just added to the array
                int link = arrayGet(&currentSCC, i);

                TArray *arrayHash = HTsearch(hashtable, link);

                if (arrayHash != NULL)
                {
                    for (int j = 0; j < arrayLength(arrayHash); j++)
                    {
                        arrayAdd(&temporaneo, arrayGet(arrayHash, j));
                    }
                    HTdelete(hashtable, arrayGet(&currentSCC, i));
                }
                else
                {
                    arrayAdd(&temporaneo, arrayGet(&currentSCC, i));
                }
            }

            HTinsert(hashtable, startingValue + 1, temporaneo); // at the end of the operation, it can be now added the new vertex, that will be created later in the function, to the hashtable
            // with his name as the key, and with all the simple nodes that compose it as the value
            int i;

#pragma omp parallel for                                         // here each OMP threads can update the shared resources, there is no need of mutual exclusivity because each thread has access to different indices
            for (i = 0; i < arrayLength(&temporaneo); i = i + 2) // in this step are updated the links of the vertex setting the adiacency list as just the new vertex name
            {                                                    // this will update also the simple nodes that were part of a previous SCC and so not present in the currentSCC array
                graph[arrayGet(&temporaneo, i) - 1].array.items[0] = startingValue + 1;
                allAdiancentList[arrayGet(&temporaneo, i) - 1] = 0; // it's also cleaned up the array used to keep track of the adiacency lists of the vertices that compose this vertex
                //  because it's useless to maintain them in the adiacency list as it would be a link to the new node itself
                if (i != arrayLength(&temporaneo) - 1)
                {
                    graph[arrayGet(&temporaneo, i + 1) - 1].array.items[0] = startingValue + 1;
                    allAdiancentList[arrayGet(&temporaneo, i + 1) - 1] = 0;
                }
            }
            graph[startingValue].nome = startingValue + 1; // here it's created the new vertex initializing his name
            if (*max < startingValue)                      // update of the max variable (used to calculate range of the OMP threads)
            {
                *max = startingValue;
            }
            graph[startingValue].array = arrayCreate(0);     // here it's initialized the adiacency list of the new vertex
            for (int i = 0; i < numberOfVertices; i = i + 2) // and then it's populated and cleaned up too the "support" array
            {
                if (allAdiancentList[i] == 1)
                {
                    arrayAdd(&(graph[startingValue].array), i + 1);
                    allAdiancentList[i] = 0;
                }
                if (i != numberOfVertices - 1)
                {
                    if (allAdiancentList[i + 1] == 1)
                    {
                        arrayAdd(&(graph[startingValue].array), i + 2);
                        allAdiancentList[i + 1] = 0;
                    }
                }
            }
            graph[startingValue].index = -1; // are initialized the other variables used for tarjan
            graph[startingValue].low_link = -1;
            graph[startingValue].in_stack = 0;

            graph[startingValue].deg = arrayLength(&(graph[startingValue].array)); // it's setted the degree of the vertex
            graph[startingValue].supernodo = 0;

            startingValue++;
            // it's cleaned up the array used for store the vertex that compose the current SCC for other iterations
            arrayDestroy(&currentSCC);
            currentSCC = arrayCreate(0);
        }
        else
        {
            /* for each vertex founded in the sccFounded array, it's added to the currentSCC array, it's setted its supernodo flag at 1 and his adiacency list is taken and added it to
            the temporary array: allAdiacentList as mentioned before*/
            arrayAdd(&currentSCC, sccFounded[j]);
            graph[sccFounded[j] - 1].supernodo = 1;
            TArray sccArray = graph[sccFounded[j] - 1].array;
            int k = 0;
#pragma omp parallel for // here each OMP threads can update the shared resources, there is no need of mutual exclusivity because each thread has access to different indices
            for (k = 0; k < arrayLength(&sccArray); k = k + 2)
            {
                int link = arrayGet(&sccArray, k);
                allAdiancentList[link - 1] = 1;
                if (k != arrayLength(&sccArray) - 1)
                {
                    link = arrayGet(&sccArray, k + 1);
                    allAdiancentList[link - 1] = 1;
                }
            }
            /* after that, it's destroyed the adiacency list of this vertex and replace it with a new adiacency list where there is just the name of the new vertex that will be added by
             the algorithm, so the SCC composed by the current vertex and others that have already been read or still have to be in next iterations. It's also updated the degree to 1 */
            arrayDestroy(&graph[sccFounded[j] - 1].array);
            graph[sccFounded[j] - 1].array = arrayCreate(0);
            arrayAdd(&(graph[sccFounded[j] - 1].array), startingValue + 1);
            graph[sccFounded[j] - 1].deg = 1;
        }
    }

    for (int j = 0; j < numberOfScc; j++) // after adding all the new vertices, for every adiacency list the function checks for duplicates, or better, for links that point to the same
    // vertex
    {

        TArray *adiacency = &graph[storedValue + j].array;
        newSccList = arrayCreate(0);
        for (int k = 0; k < arrayLength(adiacency); k++)
        {
            int link = arrayGet(adiacency, k);
            if (graph[link - 1].supernodo == 1 && graph[link - 1].nome != -1)
            {
                if (allAdiancentList[arrayGet(&graph[link - 1].array, 0) - 1] == 0)
                {
                    arrayAdd(&newSccList, link);
                    allAdiancentList[arrayGet(&graph[link - 1].array, 0) - 1] = 1;
                }
            }
            else
            {
                arrayAdd(&newSccList, link);
            }
        }

        arrayDestroy(adiacency);
        graph[storedValue + j].deg = arrayLength(&newSccList);
        graph[storedValue + j].array = arrayCreate(0);
        for (int elem = 0; elem < graph[storedValue + j].deg; elem = elem + 2)
        {
            arrayAdd(&(graph[storedValue + j].array), arrayGet(&newSccList, elem));
            if (graph[arrayGet(&newSccList, elem) - 1].supernodo == 1 && graph[arrayGet(&newSccList, elem) - 1].nome != -1)
            {
                int link = arrayGet(&newSccList, elem) - 1;
                int toDelete = arrayGet(&graph[link].array, 0);
                allAdiancentList[toDelete - 1] = 0;
            }
            if (elem != graph[storedValue + j].deg - 1)
            {
                arrayAdd(&(graph[storedValue + j].array), arrayGet(&newSccList, elem + 1));
                if (graph[arrayGet(&newSccList, elem + 1) - 1].supernodo == 1 && graph[arrayGet(&newSccList, elem + 1) - 1].nome != -1)
                {
                    int link = arrayGet(&newSccList, elem + 1) - 1;
                    int toDelete = arrayGet(&graph[link].array, 0);
                    allAdiancentList[toDelete - 1] = 0;
                }
            }
        }

        arrayDestroy(&newSccList);
    }

    arrayDestroy(&currentSCC); // at the end the memory used for the various structs is freed
    arrayDestroy(&temporaneo);
    free(allAdiancentList);
    return numberOfScc;
}

/**
 * @brief This function resets the `index` and `low_link` properties of each vertex in the graph used for the Tarjan algorithm.
 * @param graph  an array of vertices representing the graph
 * @param numberOfVertices  the number of vertices in the graph
 */
void resetGraph(vertex *graph, int numberOfVertices)
{
    int i = 0;
#pragma omp parallel for // here each OMP threads can update the shared resources, there is no need of mutual exclusivity because each thread has access to different indices
    for (i = 0; i < numberOfVertices; i = i + 2)
    {
        graph[i].index = -1;
        graph[i].low_link = -1;
        if (i != numberOfVertices - 1)
        {
            graph[i + 1].index = -1;
            graph[i + 1].low_link = -1;
        }
    }
}

/** @brief This function match two process that have completed the computation and are at the same level of the algorithm
 * @param level is an integer representing the current level of the algorithm
 * @param requests is an array of MPI requests containing the requests made by the master scheduler to the slave processes to know when a process complete the current step of the algorithm
 * @param contatore is a the number of MPI process of the program and also the size of the requests array
 * @param array is an array of integers, each index correspond to the rank of the process, and the value is setted at the current level of the process, used to couple only processes at the same level
 * @param flag is a character flag used to store the answer of the process at the request of the master, is just used as convention
 * @return number of pairs that have been matched
 * @Please note the level 1 is mapped as 4, the level 2 as 6 and so on, that because if two process at the level 2 are matched toghether(so they both have 6 in
 * the array), it's setted the array value to 3 so in next calls to this function they will not be erroneously matched with other processes*/
int couplingProcesses(int level, MPI_Request *requests, int contatore, int *array, char *flag, int *indices)
{
    int rank = -1; // variable used to track the first rank that should be coupled
    int otherrank; // variable used to track the second rank that should be coupled
    int num = 0;   // variable to store the number of pairs that are coupled by the function
    int i = 0;
    int informationsFirst[2]; // this two arrays used to store two informations: the other MPI process of the pair coupled, and the index, information used in the updateGraph to guarantee
    // exclusive numeration of the nodes
    int informationsSecond[2];

    for (i = 0; i < contatore; i++)
    {
        if (array[i] == level * 2 + 2 && rank == -1) // the first process founded
        {
            rank = i + 1;
        }
        else if (array[i] == level * 2 + 2) // the second process founded
        {
            num++; // it's updated the number of pairs coupled
            otherrank = i + 1;
            (indices[level]) += 1;
            informationsFirst[0] = rank;
            informationsFirst[1] = indices[level];
            informationsSecond[0] = otherrank;
            informationsSecond[1] = indices[level];

            MPI_Send(informationsFirst, 2, MPI_INT, i + 1, 0, MPI_COMM_WORLD); // it's sent to the slave processes their matching and their index

            MPI_Send(informationsSecond, 2, MPI_INT, rank, 0, MPI_COMM_WORLD);
            // the policy is: if i have a rank greater then my matching, i should send to him my part of graph and die,
            // instead if my rank is lower, i will receive from the other his parth of graph and i should continue the algorithm with the next step

            array[rank - 1] -= 1;
            array[i] -= 1;

            if (rank > otherrank) // based on the previous policy the scheduler continue to wait the completation of the rank who survives with a request, instead it's setted
            // to MPI_REQUEST_NULL in the requests array at the index of the rank that will die in the algorithm and not make him the request, so that the scheduler will just ignore him
            {
                MPI_Irecv(flag, 1, MPI_CHAR, otherrank, 0, MPI_COMM_WORLD, &requests[otherrank - 1]);
                requests[rank - 1] = MPI_REQUEST_NULL;
            }
            else
            {
                MPI_Irecv(flag, 1, MPI_CHAR, rank, 0, MPI_COMM_WORLD, &requests[rank - 1]);
                requests[otherrank - 1] = MPI_REQUEST_NULL;
            }
            rank = -1;
        }
    }
    return num;
}

/** @brief This function check if the MPI processes setted are consistent with the algorithm, so if there is one rank that is the scheduler and 2^n slaves
 * @param size is an integer representing the slave processes 
 * @return true is the size is coherent, otherwise it returns false*/
bool checkRanks(int size)
{
    if (size == 0)
    {
        return true;
    }
    if (size == 1)
    {
        return true;
    }
    if (size % 2 != 0)
    {
        return false;
    }
    else
    {
        return checkRanks(size / 2);
    }
}

int main(int argc, char *argv[])
{

    struct timeval all_start; // the variable all_start is used to measure the run time of the program
    gettimeofday(&all_start, NULL);
    int rank, size;
    MPI_Init(&argc, &argv);               // Initialization of the MPI execution environment
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // initialization of the rank variable
    MPI_Comm_size(MPI_COMM_WORLD, &size); // inizialization of the size variable

    if (size <= 0 || !checkRanks(size - 1) || size == 2) // check if the MPI_processes number is coherent
    {
        printf("Number of MPI process is wrong!\n");
        exit(1);
    }
    if (atoi(argv[1]) <= 0) // check if the number of vertices in input is coherent
    {
        printf("Number of vertices is wrong!\n");
        exit(1);
    }
    if (atoi(argv[2]) < 0) // check if the edges number or replica number is coherent
    {
        printf("Number of edges or replicas is wrong!\n");
        exit(1);
    }
    if (atoi(argv[3]) < 0 || atoi(argv[3]) > 7) // check if the type of graph is coherent
    {
        printf("Type of graph is wrong!\n");
        exit(1);
    }
    if (atoi(argv[5]) < 0 || atoi(argv[5]) > 3) // check if the optimization is coherent
    {
        printf("Optimization choice is wrong!\n");
        exit(1);
    }
    else
    {
        if (rank == 0) // The MPI process with rank 0 is the scheduler and creator of the graph (if the program is start with just one MPI process, this rank it's not used as a scheduler but as a SCC founder)
        {

            int numberOfVertices = atoi(argv[1]); // numberOfVertices is the number of vertices in the graph
            int EdgesOrReplicas = atoi(argv[2]);  // EdgesOrReplicas are the number of edges for each vertex or the number of replicas depending on the type of graph
            int type = atoi(argv[3]);             // type is the type of graph to be created (e.g. random, complete, etc.)
            int seed = atoi(argv[4]);             // seed is the random seed generator used to guarantee the same graph in the tests

            vertex *graph; // graph is an array of vertices representing the graph
            // the graph is created with the makeGraphGeneral function declared in the graphGenerator library

            int val = size - 1; // variable to store the number of processes excluding the master one

            if (val != 0)
            {
                graph = (vertex *)malloc(numberOfVertices * sizeof(vertex)); // the graph is allocated as the numberOfVertices * size of the vertex struct

                int edges = makeGraphGeneral(type, graph, numberOfVertices, EdgesOrReplicas, seed); // edges is the total number of edges in the graph
                assert(graph != NULL);                                                              // check if the graph is created correctly

                MPI_Send(&edges, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);          // MPI_Send to the slave process with rank 1 to send to it the number of edges in the graph, this is because
                                                                             // the file name is linked to this value, and it's known that the last process that will survive is the number 1 because of the policy of matching processes that promotes the processes
                                                                             // with low value, so the rank with 1 will always survive and it would be the one to save on file all the times
                MPI_Bcast(&numberOfVertices, 1, MPI_INT, 0, MPI_COMM_WORLD); // MPI_Bcast to all the slave processes about the number of vertices in the graph
                int vertexCounter;
                int i;
                for (vertexCounter = 0; vertexCounter < numberOfVertices / val; vertexCounter++) // with this for loop it's sent a part of the graph to each slave process
                {
                    for (i = 1; i < size; i++) // send of each vertex degree and adiacency list, there is no need to send the name of the vertex because they are ordered at the beginning, so
                    // if the slave process receive a vertex at the n iteration, he knows that the vertex name is n+1, there is no need to send also the index,low_link and in_stack value, because they
                    // are variable used in the Tarjan algorithm and default setted as -1 -1 and 0, so the slaves can just initialize them. Same for the supernodo flag, that at the beginning will
                    // be 0, so the slave can initialize it. There is the need to send the degree because the slave need to allocate memory for the adiacency list.
                    {

                        MPI_Send(&graph[vertexCounter + (i - 1) * (numberOfVertices / val)].deg, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                        MPI_Send(graph[vertexCounter + (i - 1) * (numberOfVertices / val)].array.items, graph[vertexCounter + (i - 1) * (numberOfVertices / val)].deg, MPI_INT, i, 0, MPI_COMM_WORLD);
                    }
                }

                for (int extra = 0; extra < numberOfVertices % val; extra++) // after sending the same amount of vertices to all the slaves, are sent the extra ones, starting from the rank 1,
                // here it must be sent also the name because at this stage the slave don't know which vertex he will receive
                {
                    MPI_Send(&graph[(numberOfVertices / val) * val + extra].nome, 1, MPI_INT, extra + 1, 0, MPI_COMM_WORLD);
                    MPI_Send(&graph[(numberOfVertices / val) * val + extra].deg, 1, MPI_INT, extra + 1, 0, MPI_COMM_WORLD);
                    MPI_Send(graph[(numberOfVertices / val) * val + extra].array.items, graph[(numberOfVertices / val) * val + extra].deg, MPI_INT, extra + 1, 0, MPI_COMM_WORLD);
                }

                // for each vertex it's freed the memory for the adiacency lists of each vertex
                for (int vertexCounter = 0; vertexCounter < numberOfVertices; vertexCounter = vertexCounter + 2)
                {
                    arrayDestroy(&graph[vertexCounter].array);

                    if (vertexCounter != numberOfVertices - 1)
                        arrayDestroy(&graph[vertexCounter + 1].array);
                }
                free(graph);                                                     // now it can be freed the graph itself
                int *indices = (int *)calloc(login2(size - 1) + 1, sizeof(int)); // array used to guarantee mutual exclusive at naming the nodes
                // now it starts the scheduler job of this rank
                char flag;                                                                // variable used to store the result of a recv, is just a convention, it is not used for the algorithm itself
                MPI_Request *requests = (MPI_Request *)malloc(val * sizeof(MPI_Request)); // array of requests, need to store each request made to the slaves
                int *currentLevel = (int *)calloc(sizeof(int), val);                      // an array used to store the current level of the algorithm of each slave
                int j;
                for (j = 1; j < size; j++) // the scheduler job starts adding to the request array all the requests made to all the slaves that
                // are at the beginning of their jobs and no one has already finished
                {
                    MPI_Irecv(&flag, 1, MPI_CHAR, j, 0, MPI_COMM_WORLD, &requests[j - 1]);
                }
                int contatore = val; // variable used to store the number of slaves ( so the number of requests to manage ), it will not change during the scheduler job, instead the val
                // value will decrement as each slave process die
                int check;       // variable used to check if a request is fulfilled, so a slave process has finished his task
                while (val != 1) // this loop is made until just one process is alive, that process will compute the last step of Tarjan algorithm and provide the result, so this scheduler will end his job
                {

                    for (int i = 0; i < contatore; i++) // for each vertex we check if he has completed his job with the MPI_Test, this is done only if the request array doesn't contain
                    // MPI_REQUEST_NULL, meaning that the process is already dead
                    {
                        if (requests[i] != MPI_REQUEST_NULL)
                        {
                            MPI_Test(&requests[i], &check, MPI_STATUS_IGNORE);

                            if (check) // if the slave has completed his job we update his level in the array, the level is increased by a factor of 3, this reason follow this reasoning:
                            // if the level 0 is considered as 0, level 1 as 1 and so on, when it's called the couplingProcesses function, it's not known if the level of a particular process, contained in the
                            // currentLevel array, it has just been updated or it was already paired. To solve this, it's used a convention that is: level 0 is considered 2, level 1 is considered 4, and so on,
                            // in this way when two processes are coupled togheter, it's setted as their value an odd number, so they can't be missinterpreted by the function
                            {

                                if (currentLevel[i] == 0)
                                    currentLevel[i] -= 1;
                                currentLevel[i] += 3;
                            }
                        }
                    }
                    for (int i = 0; i < login2(contatore) + 1; i++) // for each level it's checked if there are two processes that can be matched with the couplingProcesses function, if
                    // a pair is formed, one process is dead, so the val is decreased by the value of pair matched by the function
                    {
                        val -= couplingProcesses(i, requests, contatore, currentLevel, &flag, indices);
                    }
                }

                free(requests);     // at the end it can be freed the requests array
                free(currentLevel); // at the end it can be freed the currentLevel array
                free(indices);      // at the end it can be freed the indices array
            }
            else // here it's displayed the version with just one MPI process (so SCC founder job)
            {
                struct timeval all_end, start, end, total_time, graph_creation_time, communication_time, tarjan_time; // variables used for measure time of the steps of the algorithm

                gettimeofday(&end, NULL);
                timersub(&end, &all_start, &communication_time);

                gettimeofday(&start, NULL);
                int actualLength = numberOfVertices + (numberOfVertices / 2) + (numberOfVertices / 4);
                graph = (vertex *)malloc(actualLength * sizeof(vertex)); // the graph is allocated as the numberOfVertices * size of the vertex struct plus a bigger space to store new SCC founded (max 1/2 of the total vertices) and 
                // the space to store last SCC founded by the 2th tarjan 
                for (int i = 0; i < actualLength; i++) // convention used to ignore nodes in the algorithm
                {
                    graph[i].nome = -1;
                    graph[i].supernodo = -1;
                }

                int edges = makeGraphGeneral(type, graph, numberOfVertices, EdgesOrReplicas, seed); // edges is the total number of edges in the graph
                assert(graph != NULL);                                                              // check if the graph is created correctly
                gettimeofday(&end, NULL);
                timersub(&end, &all_start, &graph_creation_time);

                gettimeofday(&start, NULL);
                int numberOriginario = numberOfVertices;

                THT *hashtable; // hashtable used to map each vertex founded in the Tarjan Algorithm with the vertices that compose it
                if (numberOfVertices < 8)
                    hashtable = HTcreate(numberOfVertices);
                else
                    hashtable = HTcreate(numberOfVertices / 8);

                int numbers = omp_get_max_threads();                         // variable used to store the number of OMP threads of this run
                int *divisions = (int *)malloc(sizeof(int) * (numbers + 1)); // array to store the range of indices that each thread should consider in the algorithm
                divisions[0] = 0;
                divisions[numbers] = numberOriginario + 1;
                int diff = numberOriginario + 1;
                for (int i = 1; i < numbers; i++)
                {

                    divisions[i] = divisions[i - 1] + (diff) / numbers;

                    if (i <= diff % numbers)
                    {
                        divisions[i]++;
                    }
                }

                TStack temp = stackCreate(); // stack used for store the SCC founded in Tarjan, it's used a stack because the SCCs composed by a single vertex must be ignored, so if

#pragma omp parallel for schedule(static)                      // it can be used this "division" array because it's used a static schedule of the threads, and if it's not specified chunk-size variable,
                                                               // OpenMP will divide iterations into chunks that are approximately equal in size and it distributes chunks to threads in order
                for (int j = 0; j < numberOriginario + 1; j++) // this for loop is used to find the SCCs of the graph
                {
                    TStack stack = stackCreate(); // stack used for the Tarjan algorithm
                    int num = 0;                  // variable used as count_index in the Tarjan algorithm
                    scc(j, graph, &stack, &temp, &num, divisions);
                    stackDestroy(&stack); // after the computation it can be destroyed the stack used by Tarjan algorithm, freeing the memory
                }

                int max = 0;
                int dim = arrayLength(&temp.a);                                                                                      // it's checked how many SCC the algorithm have found, nay the numbers of vertex composing the SCCs plus a -1, used as convention, to divide
                numberOfVertices += updateGraph(graph, temp.a.items, dim, hashtable, actualLength, 1, 0, 1, numberOriginario, &max); // it's called the function that

                resetGraph(graph, numberOfVertices);

                stackDestroy(&temp);

                TStack stack = stackCreate(); // stack used for the Tarjan algorithm
                int num = 0;                  // variable used as count_index in the Tarjan algorithm
                temp = stackCreate();         // stack used for store the SCC founded in Tarjan, it's used a stack because the SCCs composed by a single vertex must be ignored, so if
                // after the algorithm it add a one-length SCC, it can be checked the length before and after the insertion and remove it from the stack with a simple pop

                for (int j = 0; j < actualLength; j++)                   // this for loop is used to find the SCCs of the graph, it calls the Tarjan algorithm from each vertex if that vertex is
                    if (graph[j].index == -1 && graph[j].supernodo == 0) // not already been founded by the algorithm itself or if it have to be ignored (if it compose a bigger vertex or if is not initialized yet (so the supernodo variable is set to 1))
                        sccFinale(j, graph, &stack, &temp, &num);

                stackDestroy(&stack);       // after the computation it can be destroyed the stack used by Tarjan algorithm, freeing the memory
                dim = arrayLength(&temp.a); // it's checked how many SCC the algorithm have found, nay the numbers of vertex composing the SCCs plus a -1, used as convention, to divide
                // different SCCs => if the algorithm have founded two SCCs composed by 1,2,3 and 4,5,6 this array will contain [1,2,3,-1,4,5,6,-1]

                numberOfVertices += updateGraph(graph, temp.a.items, dim, hashtable, actualLength, 1, 1, 1, numberOriginario, &max); // it's called the function that
                // update the graph adding the new vertex founded by the algorithm
                stackDestroy(&temp); // after the computation it can be destroyed the stack that contain the SCCs founded, freeing the memory

                FILE *fp;
                char path[200];
                sprintf(path, "ResultSCC/opt%d/type%d/Mpi+OpenMP_%d_%d_1_%d.txt", atoi(argv[5]), atoi(argv[3]), numberOriginario, edges, omp_get_max_threads()); // define the path where to store the SCCs

                char *filename = path;
                fp = fopen(filename, "w"); // open a file to write the SCCs founded by the algorithm on it
                if (fp == NULL)
                {
                    perror("Errore durante l'apertura del file");
                }
                for (int vertexCounter = 0; vertexCounter < actualLength; vertexCounter++) // in this for loop it's saved on file the SCCs of the total graph and contextually it's freed
                // the memory of the adiacency lists of the vertices
                {

                    if (graph[vertexCounter].supernodo == 0) // for print the SCC it's checked each node in the graph that have the flag supernodo = 0, if it's a simple vertex it is just
                    // saved on the file, else if it's in the hashtable it means that he is composed by other simple vertices, and they have to be saved on the file
                    {
                        fprintf(fp, "scc: ");
                        TArray *arrayHash = HTsearch(hashtable, graph[vertexCounter].nome);
                        if (arrayHash != NULL)
                        {
                            for (int j = 0; j < arrayLength(arrayHash); j++)
                            {

                                fprintf(fp, "%d ", arrayGet(arrayHash, j));
                            }
                        }
                        else
                        {
                            fprintf(fp, "%d ", graph[vertexCounter].nome);
                        }
                        fprintf(fp, "\n");
                    }
                    if (graph[vertexCounter].nome != -1)
                        arrayDestroy(&graph[vertexCounter].array);
                }
                fprintf(fp, "------------------------------------------------------\n");
                fclose(fp);

                free(graph);
                free(hashtable);
                gettimeofday(&end, NULL);
                timersub(&end, &start, &tarjan_time);
                gettimeofday(&end, NULL);
                timersub(&end, &all_start, &total_time);

                char path2[200];
                sprintf(path2, "Informations/opt%d/type%d/%d_%d.csv", atoi(argv[5]), atoi(argv[3]), numberOriginario, edges);
                char *filename2 = path2;
                fp = fopen(filename2, "a+");
                if (fp == NULL)
                {
                    perror("Errore durante l'apertura del file");
                }
                fprintf(fp, "MPI+OpenMP;%d;1; %ld.%06ld; %ld.%06ld; %ld.%06ld; %ld.%06ld;\n", omp_get_max_threads(), (long int)total_time.tv_sec, (long int)total_time.tv_usec, (long int)graph_creation_time.tv_sec, (long int)graph_creation_time.tv_usec, (long int)communication_time.tv_sec, (long int)communication_time.tv_usec, (long int)tarjan_time.tv_sec, (long int)tarjan_time.tv_usec);
                fclose(fp);
            }

            MPI_Finalize(); // terminates MPI execution environment
            exit(0);
        }

        else
        {

            struct timeval all_end, start, end, total_time, graph_creation_time, communication_time, tarjan_time; // variables used for measure time of the steps of the algorithm
            long int communication_sec = 0, communication_usec = 0, tarjan_sec = 0, tarjan_usec = 0;
            gettimeofday(&end, NULL);
            timersub(&end, &all_start, &communication_time);
            communication_sec = (long int)communication_time.tv_sec;
            communication_usec = (long int)communication_time.tv_usec;

            gettimeofday(&start, NULL); // starts to count the creation of the graph time

            int numberOfVertices;
            int edges;

            if (rank == 1)
                MPI_Recv(&edges, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // receive the edges value mentioned previously in the master branch

            MPI_Bcast(&numberOfVertices, 1, MPI_INT, 0, MPI_COMM_WORLD); // receive the number of vertices from the master
            int min = numberOfVertices;                                  // variables used for manage the range of vertex to give at each thread omp
            int max = 0;

            gettimeofday(&end, NULL); // at this time the graph is created, so it can be calculated the creational time of it
            timersub(&end, &start, &graph_creation_time);
            gettimeofday(&start, NULL); //  the timer starts again to measure the communicational time

            int nome = 0;   // variable used to store the nome variable of the current vertex sent by a rank to another
            int dim;        // variable used to store the degree variable of the current vertex sent by a rank to another
            THT *hashtable; // hashtable used to map each vertex founded in the Tarjan Algorithm with the vertices that compose it
            if (numberOfVertices < 8)
                hashtable = HTcreate(numberOfVertices);
            else
                hashtable = HTcreate(numberOfVertices / 8);

            int newNumberOfVertices = numberOfVertices; // this variable is used to approximate the number of vertices to the next value that is a multiple of the number of MPI processes
            // so that the numeration of the new vertices is exclusive for each rank at each iteration
            int resto = numberOfVertices % (size - 1);
            if (resto != 0)
            {
                newNumberOfVertices += (size - 1) - resto;
            }

            vertex *graph;
            int actualLength = newNumberOfVertices / 2 + newNumberOfVertices + newNumberOfVertices / 2 * (login2(size - 1) + 1); // the number of vertices of the graph is a bit more greater than the minimum one, because
            // the graph should be two time bigger than the original one, because at each step the algorithm should find max 1/2 SCCs of the number of vertex available, so the geometric
            // series converges to 2. It'is not chosen this size, because to guarantee the exclusive numeration of each rank at each iteration, it's needed a size which increases depending
            // on the number of MPI processes, and so with the number of iterations considering that at each iteration all the ranks can found max the number of his partition vertices / 2
            graph = (vertex *)malloc(sizeof(vertex) * (actualLength));

            char send = 'd';                         // convention used to send a value to the master scheduler, means that this rank has ended the current step of the algorithm, d stands for Done
            int privateNumberOfVertices = 0;         // variable used to store the number of vertices of the partition of the graph
            int val = numberOfVertices / (size - 1); // number of node that each slave should receive from the scheduler
            int endval = val * rank;                 // end index of the partition that this slave should receive from the scheduler
            int j = 0;
            int indice = rank; // variable used to guarantee exlusive identification number of the rank betweens ranks at each iteration
 
            for (j = 0; j < actualLength; j++)
            {

                if (j >= val * (rank - 1) && j < endval) // range of the partition of this slave
                {

                    privateNumberOfVertices++; // it's updated the number of vertices in this partition and are initialized all the "standard" variables such as index,low_link,in_stack,supernodo and also the nome depending on the iteration
                    graph[j].index = -1;
                    graph[j].low_link = -1;
                    graph[j].in_stack = 0;
                    graph[j].supernodo = 0;
                    graph[j].nome = j + 1;
                    MPI_Recv(&dim, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // here the slave receive the degree of the vertex from the master
                    if (min > j)                                                         // the min and max variables are updated
                    {
                        min = j;
                    }
                    if (max < j)
                    {
                        max = j;
                    }
                    graph[j].array = arrayCreate(dim);                                                     // it's allocated the array, the adiacency list
                    graph[j].deg = dim;                                                                    // it's initialized the degree
                    MPI_Recv(graph[j].array.items, dim, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // the slave reiceve the adiacency list from the master
                }
                else // are also setted the other node of the graph with this convention, so that the Tarjan algorithm ignore this values
                {
                    graph[j].supernodo = 1;
                    graph[j].nome = -1;
                }
            }

            if (rank <= numberOfVertices % (size - 1)) // it's checked if the slave should receive an extra vertex received as mentioned before
            {
                privateNumberOfVertices++;

                MPI_Recv(&nome, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                graph[nome - 1].index = -1;
                graph[nome - 1].low_link = -1;
                graph[nome - 1].in_stack = 0;
                graph[nome - 1].supernodo = 0;
                graph[nome - 1].nome = nome;
                MPI_Recv(&dim, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                graph[nome - 1].array = arrayCreate(dim);
                graph[nome - 1].deg = dim;
                if (min > nome - 1)
                {
                    min = nome - 1;
                }
                if (max < j)
                {
                    max = nome - 1;
                }

                MPI_Recv(graph[nome - 1].array.items, dim, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            int iterations = login2(size - 1) + 1; // this is the number of iteration the program will do: if there are 8 processes, there will be 4 iterations: first there will be 8 processes,
            // than 4, than 2 and then just one that will compute the last algorithm
            int slaves = size - 1; //  number of alive slaves
            int i = 0;
            int nextrank;       // variable used to store the match made by the scheduler
            int nextrankNumber; // variable used to store the number of vertices that the other rank will send to this slave, if the other rank is greater than the rank of this slave

            vertex v; // here it's created an MPI_Datatype made by 3 integer: nome,degree and supernodo. It's created so it can be sent just this information, and not 3 separate ones.
            // Variable such as index,low_link and in_stack  are not included because are default values as mentioned before, so the slave that receive a new vertex knows with which
            // value initialize  this variables
            int blocklengths[3] = {1, 1, 1};                     // array to store the size of each block
            MPI_Datatype types[3] = {MPI_INT, MPI_INT, MPI_INT}; // array with old data types (all integers)
            MPI_Aint offsets[3];                                 // array that specifies the offset of each block

            MPI_Aint startingStruct; // here it's calculated the offset of each block from the struct vertex
            MPI_Get_address(&(v.index), &startingStruct);
            MPI_Get_address(&(v.deg), &offsets[0]);
            MPI_Get_address(&(v.nome), &offsets[1]);
            MPI_Get_address(&(v.supernodo), &offsets[2]);
            offsets[2] -= startingStruct;
            offsets[1] -= startingStruct;
            offsets[0] -= startingStruct;
            MPI_Datatype vertex_type; // it is created the new MPI_Datatype called vertex_type
            MPI_Type_create_struct(3, blocklengths, offsets, types, &vertex_type);
            MPI_Type_commit(&vertex_type); // and it's committed in memory

            gettimeofday(&end, NULL); // at the end of this phase it's updated the communication time
            timersub(&end, &start, &communication_time);
            communication_sec += (long int)communication_time.tv_sec;
            communication_usec += (long int)communication_time.tv_usec;

            while (i < iterations) // here starts the algorithm, just one slave will reach the end of this while, other will die previously
            {
                if (i != 0) // this branch is used after a match from the scheduler to send, or receive, a part of the graph
                {
                    gettimeofday(&start, NULL); // timer to count the communication time starts
                    if (nextrank < rank)        // check if this rank have to send the graph
                    {
                        TArray hashArrays = arrayCreate(0);                                          // array used to store the vertices which are in the hashtable
                        MPI_Send(&privateNumberOfVertices, 1, MPI_INT, nextrank, 0, MPI_COMM_WORLD); // send the number of vertices of this partition to the other rank

                        for (int j = 0; j < actualLength; j++) // here are sent all the vertices of this partition that have meaning for the algorithm ( so have the variable nome not equal to -1)
                        {
                            if (graph[j].nome != -1)
                            {

                                MPI_Send(&graph[j], 1, vertex_type, nextrank, 0, MPI_COMM_WORLD);
                                MPI_Send(graph[j].array.items, graph[j].deg, MPI_INT, nextrank, 0, MPI_COMM_WORLD);
                            }
                        }

                        for (int j = numberOfVertices; j < actualLength; j++) // add to the hashArrays each new vertex with supernodo = 0, meaning that it is in the hashtable
                        {
                            if (graph[j].supernodo == 0)
                            {
                                arrayAdd(&hashArrays, j + 1);
                            }
                        }

                        int count = arrayLength(&hashArrays); // count how many vertices are in the hashtable
                        int informations[2];                  // store two informations, key of the vertex and length of his array in the hashtable (containing the simple vertex that compose it)

                        MPI_Send(&count, 1, MPI_INT, nextrank, 0, MPI_COMM_WORLD); // it's sent the number of vertices in the hashtable to the next rank

                        for (int j = 0; j < count; j++) // for every vertex in the hashtable it's sent his key and his length, so the next rank can allocate the memory, and after
                        // it's sent also the array itself
                        {

                            informations[0] = arrayGet(&hashArrays, j);
                            TArray *arrayHash = HTsearch(hashtable, informations[0]);
                            informations[1] = arrayLength(arrayHash);
                            MPI_Send(&informations, 2, MPI_INT, nextrank, 0, MPI_COMM_WORLD);

                            MPI_Send(arrayHash->items, informations[1], MPI_INT, nextrank, 0, MPI_COMM_WORLD);
                        }

                        arrayDestroy(&hashArrays); // it can be freed the memory for this array

                        break; // since this rank has terminate his computation,it can die coming out of the while
                    }
                    else // in this case the rank have to receive a part of the graph of another rank
                    {

                        MPI_Recv(&nextrankNumber, 1, MPI_INT, nextrank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // here this rank receive the number of vertex that he will receive
                        for (int j = 0; j < nextrankNumber; j++)
                        {
                            vertex temp; // temporary struct used to store temporary variables, it can't be used a vertex itself because it is not known which vertex will arrive, so
                            // it's not known his nome variable, and the rank can't initialize the right vertex

                            MPI_Recv(&temp, 1, vertex_type, nextrank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // the slave receive a vertex_type, containing nome,degree and supernodo flag

                            nome = temp.nome; // it's stored the name and it's initialized the right vertex

                            if (min > nome - 1) // min and max variable are updated
                            {
                                min = nome - 1;
                            }
                            if (max < nome - 1)
                            {
                                max = nome - 1;
                            }
                            graph[nome - 1].deg = temp.deg;
                            graph[nome - 1].supernodo = temp.supernodo;
                            graph[nome - 1].array = arrayCreate(graph[nome - 1].deg);                                                            // it's allocated the adiacency list
                            MPI_Recv(graph[nome - 1].array.items, graph[nome - 1].deg, MPI_INT, nextrank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // and then the rank receive the adiacency list from the next rank

                            graph[nome - 1].index = -1; // are also initialized the defaults variable and the nome too
                            graph[nome - 1].low_link = -1;
                            graph[nome - 1].in_stack = 0;
                            graph[nome - 1].nome = nome;
                        }
                        privateNumberOfVertices += nextrankNumber; // it's updated the number of vertices of this graph adding the new ones received from the next rank
                        int count;                                 // used, also in this case, to store the number of vertices stored in the hashtable of the nextrank
                        int informations[2];                       // same as before, used to store key and lenght of the array

                        MPI_Recv(&count, 1, MPI_INT, nextrank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // the rank receive the number of vertices in the hashtable of the next rank

                        for (int j = 0; j < count; j++) // for every vertex the rank receive his key, his length, so he can allocate the right space in memory, and the array itself
                        //  so it can be added to this rank hashtable and deallocate the temporary array
                        {
                            MPI_Recv(&informations, 2, MPI_INT, nextrank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                            TArray temporaneo = arrayCreate(informations[1]);
                            MPI_Recv(temporaneo.items, informations[1], MPI_INT, nextrank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                            HTinsert(hashtable, informations[0], temporaneo);
                            arrayDestroy(&temporaneo);
                        }
                    }

                    gettimeofday(&end, NULL); // at the end it's calculated the communication time
                    timersub(&end, &start, &communication_time);
                    communication_sec += (long int)communication_time.tv_sec;
                    communication_usec += (long int)communication_time.tv_usec;
                }
                gettimeofday(&start, NULL);  // after the send/receive of the graph, or in case it's the first iteration ( so no one have to send nothing to no one) the timer for measure the tarjan algorithm time starts
                MPI_Request request;         // communication request
                TStack temp = stackCreate(); // stack used for store the SCC founded in Tarjan, it's used a stack because the SCCs composed by a single vertex must be ignored, so if
                // after the algorithm it add a one-length SCC, it can be checked the length before and after the insertion and remove it from the stack with a simple pop

                int numbers = omp_get_max_threads();                         // variable used to store the number of OMP threads of this run
                int *divisions = (int *)malloc(sizeof(int) * (numbers + 1)); // array to store the range of indices that each thread should consider in the algorithm
                divisions[0] = min;
                divisions[numbers] = max + 1;
                int diff = (max - min) + 1;
                for (int i = 1; i < numbers; i++)
                {

                    divisions[i] = divisions[i - 1] + (diff) / numbers;

                    if (i <= diff % numbers)
                    {
                        divisions[i]++;
                    }
                }

#pragma omp parallel for schedule(static)           // it can be used this "division" array because it's used a static schedule of the threads, and if it's not specified chunk-size variable,
                                                    // OpenMP will divide iterations into chunks that are approximately equal in size and it distributes chunks to threads in order
                for (int j = min; j < max + 1; j++) // this for loop is used to find the SCCs of the graph, it calls the Tarjan algorithm from each vertex if that vertex is
                {
                    if (graph[j].index == -1 && graph[j].supernodo == 0) // not already been founded by the algorithm itself or if it have to be ignored (if it compose a bigger vertex or if is not initialized yet (so the supernodo variable is set to 1))
                    {
                        TStack stack = stackCreate(); // stack used for the Tarjan algorithm
                        int num = 0;                  // variable used as count_index in the Tarjan algorithm
                        scc(j, graph, &stack, &temp, &num, divisions);
                        stackDestroy(&stack); // after the computation it can be destroyed the stack used by Tarjan algorithm, freeing the memory
                    }
                }

                free(divisions);
            
                dim = arrayLength(&temp.a); // it's checked how many SCC the algorithm have found, nay the numbers of vertex composing the SCCs plus a -1, used as convention, to divide
                // different SCCs => if the algorithm have founded two SCCs composed by 1,2,3 and 4,5,6 this array will contain [1,2,3,-1,4,5,6,-1]

                privateNumberOfVertices += updateGraph(graph, temp.a.items, dim, hashtable, actualLength, indice, i, slaves, numberOfVertices, &max); // it's called the function that
                // update the graph adding the new vertex founded by the algorithm
                slaves = slaves / 2; // it's updated the number of slaves variable, because at the next iteration it's known that half of the process will be dead

                resetGraph(graph, actualLength); // after the computation the default values such as index and low_link have to be resetted because they were modified by the Tarjan algorithm
                // there is no need to reset the in_stack variable because it's resetted by the algorithm itself
            
                stackDestroy(&temp); // after the computation it can be destroyed the stack that contain the SCCs founded, freeing the memory

                if (i == iterations - 1) // on the last iteration, there is the need to compute another Tarjan but with no OMP threads, because there is the need to compute on the complete graph
                {
                    TStack stack = stackCreate();
                    TStack temp = stackCreate();
                    int num = 0;

                    for (int j = 0; j < actualLength; j++)
                        if (graph[j].index == -1 && graph[j].supernodo == 0)
                        {
                            sccFinale(j, graph, &stack, &temp, &num);
                        }
                    stackDestroy(&stack);
                    

                    dim = arrayLength(&temp.a);

                    privateNumberOfVertices += updateGraph(graph, temp.a.items, dim, hashtable, actualLength, 1, i + 1, 1, numberOfVertices, &max); 
                    stackDestroy(&temp);
                }
                

                i++;
                gettimeofday(&end, NULL);             // here ends the tarjan execution, included the allocation and deallocation of memory, of this step
                timersub(&end, &start, &tarjan_time); // it can be calculated the time of tarjan algorithm
                tarjan_sec += (long int)tarjan_time.tv_sec;
                tarjan_usec += (long int)tarjan_time.tv_usec;

                if ((i - 1) != iterations - 1) // after the completation of a step, the rank tell to the master scheduler that he has finished the job and it waits a master
                // answer to know with which rank he should communicate to send/receive the part of the graph and which is his index for the next step
                {
                    int informations[2];
                    gettimeofday(&start, NULL); // here we calculate the idle time of the rank to receive a match from the master considered as communication time

                    MPI_Isend(&send, 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &request);
                    MPI_Recv(informations, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    nextrank = informations[0];
                    indice = informations[1];
                    gettimeofday(&end, NULL);
                    timersub(&end, &start, &communication_time);
                    communication_sec += (long int)communication_time.tv_sec;
                    communication_usec += (long int)communication_time.tv_usec;
                }
            }
            if (i == iterations) // at the end of the loop, if this rank have reached the last iteration, it means that he his the rank 1, and he have to compute the final result
            {
                gettimeofday(&start, NULL); // this is considered as tarjan computation time

                FILE *fp;
                char path[200];
                sprintf(path, "ResultSCC/opt%d/type%d/Mpi+OpenMP_%d_%d_%d_%d.txt", atoi(argv[5]), atoi(argv[3]), numberOfVertices, edges, size, omp_get_max_threads()); // define the path where to store the SCCs

                char *filename = path;
                fp = fopen(filename, "w"); // open a file to write the SCCs founded by the algorithm on it
                if (fp == NULL)
                {
                    perror("Errore durante l'apertura del file");
                }
                for (int vertexCounter = 0; vertexCounter < actualLength; vertexCounter++) // in this for loop it's saved on file the SCCs of the total graph and contextually it's freed
                // the memory of the adiacency lists of the vertices
                {

                    if (graph[vertexCounter].supernodo == 0) // for print the SCC it's checked each node in the graph that have the flag supernodo = 0, if it's a simple vertex it is just
                    // saved on the file, else if it's in the hashtable it means that he is composed by other simple vertices, and they have to be saved on the file
                    {
                        fprintf(fp, "scc: ");
                        TArray *arrayHash = HTsearch(hashtable, graph[vertexCounter].nome);
                        if (arrayHash != NULL)
                        {
                            for (int j = 0; j < arrayLength(arrayHash); j++)
                            {

                                fprintf(fp, "%d ", arrayGet(arrayHash, j));
                            }
                        }
                        else
                        {
                            fprintf(fp, "%d ", graph[vertexCounter].nome);
                        }
                        fprintf(fp, "\n");
                    }
                    if (graph[vertexCounter].nome != -1)
                        arrayDestroy(&graph[vertexCounter].array);
                }
                fprintf(fp, "------------------------------------------------------\n");
                fclose(fp);
            }
            else // if the rank is not come to the end of the iterations, he still have to free the memory of the adiacency lists
            {

                for (int vertexCounter = 0; vertexCounter < actualLength; vertexCounter++)
                {
                    if (graph[vertexCounter].nome != -1)
                        arrayDestroy(&graph[vertexCounter].array);
                }
            }
            MPI_Type_free(&vertex_type); // the rank also free the memory allocated for the MPI_Datatype vertex_type

            HTdestroy(hashtable);     // the rank free the memory for the hashtable
            free(graph);              // the rank free the memory for the graph
            gettimeofday(&end, NULL); // tarjan computation time is ended, so we calculate this time
            timersub(&end, &start, &tarjan_time);
            tarjan_sec += (long int)tarjan_time.tv_sec;
            tarjan_usec += (long int)tarjan_time.tv_usec;

            gettimeofday(&all_end, NULL);
            timersub(&all_end, &all_start, &total_time); // the program is ended too, so we can stop the timer

            long double communication_time2 = communication_sec + ((long double)communication_usec) / 1000000; // this is made because adding sec to sec and usec to usec, doesn't
            // guarantee an adequate count, because if the sum of usec is, for example, 0.999999 and 0.999999, there will be 0 in the sec variable, and 1999998 in the usec variable, so there is the need
            // to convert to sec by dividing by a 1000000 factor
            long double tarjan_time2 = tarjan_sec + ((long double)tarjan_usec) / 1000000;

            if (i == iterations) // the rank who survives (rank 1) have to write on the file all the times he has calculated: total_time, graph_creation_time,communication_time and tarjan_time
            {
                FILE *fp;
                char path[200];
                sprintf(path, "Informations/opt%d/type%d/%d_%d.csv", atoi(argv[5]), atoi(argv[3]), numberOfVertices, edges);
                char *filename = path;
                fp = fopen(filename, "a+");
                if (fp == NULL)
                {
                    perror("Errore durante l'apertura del file");
                }
                fprintf(fp, "MPI+OpenMP;%d;%d; %ld.%06ld; %ld.%06ld; %Lf; %Lf;\n", omp_get_max_threads(), size, (long int)total_time.tv_sec, (long int)total_time.tv_usec, (long int)graph_creation_time.tv_sec, (long int)graph_creation_time.tv_usec, communication_time2, tarjan_time2);
                fclose(fp);
            }
            MPI_Finalize(); // terminates MPI execution environment
            exit(0);
        }
    }
}