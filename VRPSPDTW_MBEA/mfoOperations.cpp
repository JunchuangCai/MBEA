//
// Created by tsun on 2021/9/15.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "functions.h"

void factorial_cost(int startPoint,int size){
    int seq[MAX_NODES];
    for(int i=0;i<TASK_NUMBER;i++){
        for(int j=startPoint;j<size;j++){
            memcpy(seq,chro[j].sequence,sizeof(seq));
            decoding(seq,vNumber[i]);
            split_T1(&indi[j],seq, i);
            chro[j].fitness[i]=indi[j].fitness[i];
            chro[j].distance[i]=indi[j].distance[i];
            chro[j].nums_of_vehicle[i]=indi[j].nums_of_vehicle[i];
        }
    }
}

void factorial_rank(int size){
    for(int i=0;i<TASK_NUMBER;i++){
        population_sorting(chro,size,i);
        for(int j=0;j<size;j++)
            chro[j].fRank[i]=j+1;
    }
}

void scalar_fitness(int size){
    int min_rank;
    for(int i=0;i<size;i++){
        min_rank=get_min_element(chro[i].fRank,TASK_NUMBER);
        chro[i].scalar_fitness=1.0/min_rank;
    }
}

void skill_factor(int startPoint,int size){
    int min_index;
    for(int i=startPoint;i<size;i++){
        min_index=get_min_index(chro[i].fRank,TASK_NUMBER);
        chro[i].skill_factor=min_index;

        chro[i].count=VERTEX_NUMBER-1;
        // Update route_list
        int seq[MAX_NODES];
        memcpy(seq,chro[i].sequence,sizeof(seq));

        decoding(seq,vNumber[min_index]);
        split_T1(&indi[i],seq, min_index);
        for (auto & it : chro[i].route_list) it.clear();
        int index = 0;
        for (int k=0; k<indi[i].count-1; k++){
            if (indi[i].sequence[k]==0 && indi[i].sequence[k+1]!=0){
                while(indi[i].sequence[k+1]!=0){
                    chro[i].route_list[index].push_back(indi[i].sequence[k+1]);
                    k++;
                }
                index ++;
            }
        }

    }
}

