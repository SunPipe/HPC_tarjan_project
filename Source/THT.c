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
 * This file is used to manage hashtables, taken from the 2021/2022 course of "Algoritmi e strutture dati"
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
#include <unistd.h>
#include <time.h>
#include "../Headers/THT.h"
#include "../Headers/TInfo.h"

THT *HTcreate(int n)
{
    THT *ht = malloc(sizeof(THT));
    assert(ht != NULL);

    ht->bucket = malloc(n * sizeof(TList));
    assert(ht->bucket != NULL);

    for (int i = 0; i < n; i++)
        ht->bucket[i] = listCreate();
    ht->n_bucket = n;
    ht->elements=0;
    return ht;
}

void HTdestroy(THT *ht)
{
    for (int i = 0; i < ht->n_bucket; i++)
        ht->bucket[i] = listDestroy(ht->bucket[i]);
    free(ht->bucket);
    free(ht);
}

TValue *HTsearch(THT *ht, TKey key)
{

    unsigned h = keyHash(key) % ht->n_bucket;
    TInfo info = {key};

    TNode *node = listSearch(ht->bucket[h], info);

    if (node != NULL)
        return &node->info.value;

    return NULL;
}

void HTinsert(THT *ht, TKey key, TValue value)
{
    unsigned h = keyHash(key) % ht->n_bucket;

    TInfo info = {key, value};
    TNode *node = listSearch(ht->bucket[h], info);

    if (node != NULL)
        for (int i = 0; i < arrayLength(&value); i++)
        {
            arrayAdd(&(node->info.value), arrayGet(&value, i));
        }
    else
    {
        TInfo info2 = {key, arrayCreate(0)};
        for (int i = 0; i < arrayLength(&value); i++)
        {
            arrayAdd(&(info2.value), arrayGet(&value, i));
        }
        ht->elements++;
        ht->bucket[h] = listInsert(ht->bucket[h], info2);
    }
}

void HTdelete(THT *ht, TKey key)
{
    unsigned h = keyHash(key) % ht->n_bucket;
    TInfo info = {key};
    ht->elements--;
    ht->bucket[h] = listDelete(ht->bucket[h], info);
}

void HTprint(THT *ht)
{
    for (int i = 0; i < ht->n_bucket; i++)
        
        for (TNode *node = ht->bucket[i]; node != NULL; node = node->link)
        {
            printf("Chiave: %d\n",node->info.key);
            infoPrint(node->info);
        }
}

void HTprintStructure(THT *ht)
{
    for (int i = 0; i < ht->n_bucket; i++)
    {
        printf("Bucket %2d -> ", i);
        for (TNode *node = ht->bucket[i]; node != NULL; node = node->link)
            infoPrint(node->info);
        printf("\n");
    }
}


