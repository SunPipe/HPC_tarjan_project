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
 * This file is used to manage the dynamic stacks, taken from the 2021/2022 course of "Algoritmi e strutture dati"
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
#include "../Headers/TStack.h"

TStack stackCreate(void)
{
    TStack s;
    s.a = arrayCreate(0);
    return s;
}

void stackDestroy(TStack *s)
{
    arrayDestroy(&s->a);
}

void stackPush(TStack *s, int x)
{
    arrayResize(&s->a, s->a.length + 1);
    s->a.items[s->a.length - 1] = x;
}

int stackPop(TStack *s)
{
    int x = s->a.items[s->a.length - 1];
    arrayResize(&s->a, s->a.length - 1);
    return x;
}

int stackTop(TStack *s)
{
    return s->a.items[s->a.length - 1];
}

int stackIsEmpty(TStack *s)
{
    return s->a.length == 0;
}

int stackLen(TStack *s)
{
    return s->a.length;
}

