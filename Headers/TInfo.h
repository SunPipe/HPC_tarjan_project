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
 * This file is used to manage the type (key,dynamic array), taken from the 2021/2022 course of "Algoritmi e strutture dati"
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

#include "TArray.h"
#ifndef TINFO_H
#define TINFO_H

typedef int TKey;
typedef TArray TValue;

/* TValue alternativo
typedef struct { 
    char name[20]; 
    float price; 
} TValue;
*/

typedef struct {
    TKey key;
    TValue value;
} TInfo;

int infoEqual (TInfo, TInfo);
int infoGreater (TInfo, TInfo);
int infoLess (TInfo, TInfo);
void infoPrint (TInfo);
unsigned int keyHash (TKey);


#endif

