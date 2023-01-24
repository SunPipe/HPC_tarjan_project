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
 * This file has the purpose to create various type of graphs (e.g. random,complete,replicas...)
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
#include <assert.h>
#include <stdbool.h>
#include <time.h>
#include "../Headers/graphGenerator.h"

/**
 *@brief  Function that creates randoms links, each vertex has a number of links between 0 and maxNumberOfEdges
 *@param graph is the array of vertex rapresenting the graph
 *@param numberOfVertices is the number of vertices of the graph
 *@param maxNumberOfEdges is the max number of edges of each vertex
 *@param seed is an integer added as parameter for testing reason, to make all the same test also with random numbers
 *@return number of total edges of the graph */
int makeGraphRandom(vertex *graph, int numberOfVertices, int maxNumberOfEdges, int seed)
{
    srand(seed); // we set the seed
   
    int *available = (int *)calloc(sizeof(int), numberOfVertices); // available is an array of integer, each mapping the name of a vertex, used for create no replicate links between the same vertices
    int edges=0; // variable used for store the number of edges created


    int vertexCounter = 1;
    int edgeCounter = 0;
   
    /* 
        For each vertex we initialize its name, its index, its low_link, its in_stack, its deg, its supernodo variable, its adjacency list, then for each vertex we add
        a random number of links, using the variable available to avoid duplicates, and we also update the degree value
    */
    for (vertexCounter = 1; vertexCounter <= numberOfVertices; vertexCounter++)
    {

        graph[vertexCounter - 1].nome = vertexCounter;
        graph[vertexCounter - 1].index = -1;
        graph[vertexCounter - 1].low_link = -1;
        graph[vertexCounter - 1].in_stack = 0;
        graph[vertexCounter - 1].deg = 0;
        graph[vertexCounter - 1].supernodo = 0;
        graph[vertexCounter - 1].array = arrayCreate(0);

        int randomEdges = rand() % maxNumberOfEdges;
        for (edgeCounter = 0; edgeCounter < randomEdges; edgeCounter++)
        {
            int valore = rand() % numberOfVertices;
            if (available[valore] != vertexCounter)
            {
                arrayAdd(&graph[vertexCounter - 1].array, valore + 1);
                available[valore] = vertexCounter;
                graph[vertexCounter - 1].deg += 1;
                edges++;

            }
        }
    }
    free(available);
    return edges;
}



/**
 *@brief  Function that creates all possible links between al the vertices of the graph
 *@param graph is the array of vertex rapresenting the graph
 *@param numberOfVertices is the number of vertices of the graph 
 *@return number of total edges of the graph */
int makeGraphComplete(vertex *graph, int numberOfVertices)
{
    

    int vertexCounter = 1;
    int edgeCounter = 0;
    /*
        For each vertex we link it with all the other vertices and also with itself
    */
    for (vertexCounter = 1; vertexCounter <= numberOfVertices; vertexCounter++)
    {

        graph[vertexCounter - 1].nome = vertexCounter;
        graph[vertexCounter - 1].index = -1;
        graph[vertexCounter - 1].low_link = -1;
        graph[vertexCounter - 1].in_stack = 0;
        graph[vertexCounter - 1].deg = 0;
        graph[vertexCounter - 1].supernodo = 0;
        graph[vertexCounter - 1].array = arrayCreate(0);

        for (edgeCounter = 1; edgeCounter <= numberOfVertices; edgeCounter++)
        {
           
            arrayAdd(&graph[vertexCounter - 1].array, edgeCounter);
            graph[vertexCounter - 1].deg += 1;
        
    
        }
    }
    return numberOfVertices*numberOfVertices;
}

/**
 *@brief Function that creates no links for each vertex
 *@param graph is the array of vertex rapresenting the graph
 *@param numberOfVertices is the number of vertices of the graph
 *@return number of total edges of the graph  */
int makeGraphDisconnected(vertex *graph, int numberOfVertices){
    
    int vertexCounter = 1;
    int edgeCounter = 0;
    /*
        We just initialize each vertex, there is no link between any vertex
    */
    for (vertexCounter = 1; vertexCounter <= numberOfVertices; vertexCounter++)
    {

        graph[vertexCounter - 1].nome = vertexCounter;
        graph[vertexCounter - 1].index = -1;
        graph[vertexCounter - 1].low_link = -1;
        graph[vertexCounter - 1].in_stack = 0;
        graph[vertexCounter - 1].deg = 0;
        graph[vertexCounter - 1].supernodo = 0;
        graph[vertexCounter - 1].array = arrayCreate(0);

    }
    return 0;
}

/**
 *@brief  Function that creates a mini graph and replicate it including its links
 *@param graph is the array of vertex rapresenting the graph
 *@param numberOfVertices is the number of vertices of the graph
 *@param numberOfReplicas is the number of replicas of the mini graph
 *@param coupling represent the coupling between replica: 0 -> no coupling, 1 -> low coupling (just a link), 2 -> coupling (random link between 0 and the number of vertices of the mini graph)
 *@param randomness is an integer, if it's 0 the links between the replicas are sequential, so the first replica links with the second, the second with the third and so on...,
 if it's 1 the links between the replicas are random
 *@param totalConnected is an integer, if it's 0 the mini graph is created as a random graph, if it's 1 the mini graph is created as a complete graph
 *@param seed is an integer added as parameter for testing reason, to make all the same test also with random numbers 
 *@return number of total edges of the graph */
int makeGraphReplica(vertex *graph,int numberOfVertices,int numberOfReplicas, int coupling, bool randomness, bool totalConnected,int seed){
    int vertexCounter = 1;
    int repetition = 0;
    int replica=0;
    int arrayvalue=0;
    assert(numberOfVertices%numberOfReplicas==0 && numberOfReplicas!=1);
    int spike = numberOfVertices/numberOfReplicas;
    int numbersOfLinks;
    srand(seed);
    int edges=0;
    if(!totalConnected)
        edges=makeGraphRandom(graph,spike,spike,seed);
    else
        edges=makeGraphComplete(graph,spike);
    

    for(vertexCounter=spike+1;vertexCounter<=numberOfVertices;vertexCounter++){
        graph[vertexCounter - 1].nome = vertexCounter;
        graph[vertexCounter - 1].index = -1;
        graph[vertexCounter - 1].low_link = -1;
        graph[vertexCounter - 1].in_stack = 0;
        graph[vertexCounter - 1].deg = 0;
        graph[vertexCounter - 1].supernodo = 0;
        graph[vertexCounter - 1].array = arrayCreate(0);
    }
    /*
        After creating the mini graph we replicate it for create the big graph 
    */
     for(repetition = 1; repetition<=spike;repetition++){
         for (arrayvalue = 0; arrayvalue < arrayLength(&graph[repetition-1].array); arrayvalue++)
         {  
             for(replica=1;replica<numberOfReplicas;replica++){
                 arrayAdd(&graph[(replica*spike+repetition)-1].array, arrayGet(&graph[repetition-1].array,arrayvalue)+(replica*spike));
                 graph[(replica*spike+repetition)-1].deg += 1;
                 edges++;
                 }

         }
     }
    
    /* 
        Based on the coupling and randomness value we add links between replicas using two array that map the starting and ending vertex of a link to avoid duplicates
    */
    if(coupling!=0){
        if(coupling==1){
            numbersOfLinks = 1;
        }
        else if(coupling==2){
            numbersOfLinks = rand()%spike+1;

        }
        int *availableFrom = (int *)calloc(sizeof(int), spike);
        int *availableTo = (int *)calloc(sizeof(int), spike);


        if(randomness){
            for(int i=0;i<numbersOfLinks;i++){
                int from = rand()%spike;
                int to = rand()%spike;
                if(!(availableFrom[from]==1 && availableTo[to]==1)){
                    availableFrom[from]=1;
                    availableTo[to]=1;
                    for (int i=0;i<numberOfReplicas;i++){
                        int next=rand()%numberOfReplicas;
                        while(next == i){
                            next=rand()%numberOfReplicas;
                        }
                        int value = to+1+(next)*spike;
                        
                        arrayAdd(&graph[from+i*spike].array,value);
                        edges++;
                        graph[from+i*spike].deg+=1;
                    }
                }
                
            }
        }else{
            for(int i=0;i<numbersOfLinks;i++){
                int from = rand()%spike;
                int to = rand()%spike;
                if(!(availableFrom[from]==1 && availableTo[to]==1)){

                    availableFrom[from]=1;
                    availableTo[to]=1;
                    for (int i=0;i<numberOfReplicas;i++){
                        if(i==numberOfReplicas-1)
                            arrayAdd(&graph[from+i*spike].array,to+1);
                        else
                            arrayAdd(&graph[from+i*spike].array,to+1+(i+1)*spike);
                        graph[from+i*spike].deg+=1;
                        edges++;
                    }
                }
                
            }
            

        }

        free(availableFrom);
        free(availableTo);
    }
    return edges;

    
}

/**
 *@brief  Function that manage the creation of all the combination of graphs
 *@param type is the specific type of graph, it can be: 
 * 0 -> disconnected
 * 1 -> random
 * 2 -> complete
 * 3 -> replicas random disconnected 
 * 4 -> replicas random low linked in a sequential way
 * 5 -> replicas random low linked in a random way
 * 6 -> replicas random linked in a sequential way
 * 7 -> replicas random linked in a random way
 *@param graph is the array of vertex rapresenting the graph
 *@param numberOfVertices is the number of vertices of the graph
 *@param EdgesOrReplica is the max number of edges of each vertex or the number of replicas, depending on the type of the graph
 *@param seed is an integer added as parameter for testing reason, to make all the same test also with random numbers 
 *@return number of total edges of the graph */
int makeGraphGeneral(int type,vertex *graph,int numberOfVertices,int EdgesOrReplicas,int seed){
    switch (type)
            {
            case 0:
                return makeGraphDisconnected(graph,numberOfVertices);
                break;
            case 1:
                return makeGraphRandom(graph,numberOfVertices,EdgesOrReplicas,seed);
                break;
            case 2:
                return makeGraphComplete(graph,numberOfVertices);
                break;
            case 3:
                return makeGraphReplica(graph,numberOfVertices,EdgesOrReplicas,0,false,false,seed);
                break;
            case 4:
                return makeGraphReplica(graph,numberOfVertices,EdgesOrReplicas,1,false,false,seed);
                break;
            case 5:
                return makeGraphReplica(graph,numberOfVertices,EdgesOrReplicas,1,true,false,seed);
                break;
            case 6:
                return makeGraphReplica(graph,numberOfVertices,EdgesOrReplicas,2,false,false,seed);
                break;
            case 7:
                return makeGraphReplica(graph,numberOfVertices,EdgesOrReplicas,2,true,false,seed);
                break;
            default:
                break;
            }
}
