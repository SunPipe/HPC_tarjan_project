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
 * This file is used to manage the list, used in the bucket of the hashtable, taken from the 2021/2022 course of "Algoritmi e strutture dati"
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
#include "../Headers/TList.h"




TNode *nodeCreate(TInfo info) {
    TNode *new = malloc (sizeof(TNode));
    if (new != NULL) {
        new->info = info;
        new->link = NULL;
    }
    return new;
}

void nodeDestroy(TNode *node) {
    free (node);
}

/* operazioni sulle liste */

TList listCreate() {
    return NULL;
}

TList listDestroy(TList list) {
    TNode *node;
    while (list != NULL) {
        node = list;
        list = list->link;
        nodeDestroy(node);
    }
    return list;
}

void listPrint(TList list) {
    if (list != NULL) {
        infoPrint (list->info);
        listPrint (list->link);
    }
    else 
        printf ("\n");
}

TNode *listSearch(TList list, TInfo info) {
    if (list == NULL || infoGreater(list->info, info)) 
        return NULL;
    if (infoEqual(list->info, info))
        return list;
    return listSearch(list->link, info);
}

TList listInsert(TList l, TInfo info) {
    if (l == NULL || infoGreater(l->info, info)) {
        TNode *node = nodeCreate(info);
        assert (node != NULL);
        node->link = l;
        return node;
    }
    l->link = listInsert(l->link, info);
    return l;
}

TList listDelete(TList l, TInfo info) {
    if (l == NULL || infoGreater(l->info, info)) 
        return l;
    if (infoEqual(l->info, info)) {
        TList m = l->link;
        nodeDestroy(l);
        return m;
    }
    l->link = listDelete(l->link, info);
    return l;
}