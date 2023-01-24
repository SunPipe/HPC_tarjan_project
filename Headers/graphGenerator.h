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


#ifndef GRAPHGENERATOR_H
#define GRAPHGENERATOR_H
#include "../Headers/TArray.h"

/**
 *@brief  Structure to represent vertices of the graph
 * @param index represent the index of the vertex, used in the Tarjan algorithm
 * @param low_link represent the low_link of the vertex, used in the Tarjan algorithm
 * @param in_stack represent if the vertex is in the Tarjan stack during the algorithm
 * @param deg represent the degree of the vertex, the outer degree, the number of outer links with other vertices
 * @param nome represent the name of the vertex
 * @param supernodo is setted to 0 if the vertex is not part of a bigger node, instead is setted to 1 if the vertex is part of a bigger node founded during the Tarjan algorithm
 * @param array is a TArray, a dinamic array, used to store the links between a vertex and the other vertices, just the outer links, is its adjacency list*/
typedef struct
{
    int index;
    int low_link;
    int in_stack;
    int deg;
    int nome;
    int supernodo;
    TArray array;
} vertex;

/**
 *@brief  Function that creates randoms links, each vertex has a number of links between 0 and maxNumberOfEdges
 *@param graph is the array of vertex rapresenting the graph
 *@param numberOfVertices is the number of vertices of the graph
 *@param maxNumberOfEdges is the max number of edges of each vertex
 *@param seed is an integer added as parameter for testing reason, to make all the same test also with random numbers
 *@return number of total edges of the graph */
int makeGraphRandom(vertex *graph, int numberOfVertices, int maxNumberOfEdges,int seed);

/**
 *@brief  Function that creates all possible links between al the vertices of the graph
 *@param graph is the array of vertex rapresenting the graph
 *@param numberOfVertices is the number of vertices of the graph
 *@return number of total edges of the graph */

int makeGraphComplete(vertex *graph, int numberOfVertices);

/**
 *@brief Function that creates no links for each vertex
 *@param graph is the array of vertex rapresenting the graph
 *@param numberOfVertices is the number of vertices of the graph
 *@return number of total edges of the graph */
int makeGraphDisconnected(vertex *graph, int numberOfVertices);

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
int makeGraphReplica(vertex *graph,int numberOfVertices,int numberOfReplicas, int coupling, bool randomness, bool totalConnected,int seed);

/**
 *@brief  Function that manage the creation of all the combination of graphs
 *@param type is the specific type of graph
 *@param graph is the array of vertex rapresenting the graph
 *@param numberOfVertices is the number of vertices of the graph
 *@param EdgesOrReplica is the max number of edges of each vertex or the number of replicas, depending on the type of the graph
 *@param seed is an integer added as parameter for testing reason, to make all the same test also with random numbers 
 *@return number of total edges of the graph */
int makeGraphGeneral(int type,vertex *graph,int numberOfVertices,int EdgesOrReplicas, int seed);


#endif