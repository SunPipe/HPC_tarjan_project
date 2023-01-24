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
 * This file is used to manage the dynamic arrays, taken from the 2021/2022 course of "Algoritmi e strutture dati"
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
#include "../Headers/TArray.h"

#define C_EXP 5  // costante di espansione
#define C_RED 10 // costante di riduzione



TArray arrayCreate (int length) {
    TArray a;
    int size = length + C_EXP;
    a.items = malloc(size * sizeof(int));
    assert (a.items != NULL);
    a.length = length;
    a.size = size; 
    return a;
}

void arrayDestroy (TArray *a) {
    free(a->items);
    a->items = NULL;
    a->length = a->size = 0;
}

void arrayResize (TArray *a, int length) {
    if (length > a->size || length < a->size - C_RED) {
        int size = length + C_EXP;
       
        a->items = realloc(a->items, size * sizeof(int));
        assert (a->items != NULL);
        a->size = size;        
    }
    a->length = length;
 }

void arrayPrint(TArray *a) {
    printf("Lista di adiacenza:  ");
    for (int i = 0; i < a->length; ++i)
        printf("%d ->\t",a->items[i]);
}

/* FUNZIONI DI UTILITA' */

// Aggiunta di un elemento in coda all'array
void arrayAdd (TArray *a, int info) {
    arrayResize(a, a->length + 1);
    a->items[a->length - 1] = info;
}

// Lettura di un elemento in posizione pos
int arrayGet (TArray *a, int pos) {
    return a->items[pos];
}

// Lettura della lunghezza dell'array
int arrayLength (TArray *a) {
    return a->length;
}

// Inserimento di un elemento in posizione pos con ridimensionamento automatico
void arraySet (TArray *a, int pos, int info) {
    if (pos >= a->length)
        arrayResize(a, pos + 1);
    a->items[pos] = info;
}