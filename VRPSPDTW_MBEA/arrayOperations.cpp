//
// Created by tsun on 2021/9/15.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "functions.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void undel_indi_copy(undelimiter_individual *target, undelimiter_individual *source)
{
    memcpy(target->sequence, source->sequence, sizeof(source->sequence));
    memcpy(target->fitness, source->fitness, sizeof(source->fitness));
    memcpy(target->fRank, source->fRank, sizeof(source->fRank));
    memcpy(target->distance, source->distance, sizeof (source->distance));
    memcpy(target->nums_of_vehicle, source->nums_of_vehicle, sizeof(source->nums_of_vehicle));

    target->count=source->count;
    target->scalar_fitness=source->scalar_fitness;
    target->skill_factor=source->skill_factor;
    for (int i =0; i< MAX_NODES; i++) target->route_list[i] = source->route_list[i];
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int remove_task_seq_delimiters(int *task_seq,int count)
{
    for (int i =count-1; i >=0; i--)
    {
        if (task_seq[i] == DEPOT){
            delete_element(task_seq, i,count);
            count--;
        }
    }
    return count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int rand_choose(int num)
{

    int k = rand()%num;
    //if(k==24){
    //
    //	printf("24！num=%d",num);
    //}
    return k;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int find_element(int *a, int ele,int count){
    for(int i=0;i<count;i++){
        if(a[i]==ele) return i;
    }
    return -1;
}

int delete_element(int *a, int pos,int count)
{

    count--;
    for (int i = pos; i < count; i++)
    {
        a[i] = a[i+1];
    }
    return count;
}

int add_element(int *a, int e, int pos,int count)
{
    if(count==0){
        a[0]=e;
        return ++count;
    }


    for (int i = count; i > pos; i--)
    {
        a[i] = a[i-1];
    }

    count++;
    a[pos] = e;

    return count;
}

void swap_element(int *sequence,int pos1,int pos2){
    int temp;

    temp=sequence[pos1];

    sequence[pos1]=sequence[pos2];
    sequence[pos2]=temp;
}

int get_min_element(int *seq,int count){
    int min=seq[0];
    for(int i=0;i<count;i++){
        if(seq[i]<min)
            min=seq[i];
    }
    return min;
}
int get_min_index(int *seq,int count){
    int min=seq[0];
    int index=0;
    for(int i=0;i<count;i++){
        if(seq[i]<min){
            min=seq[i];
            index=i;
        }
    }
    return index;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int caculate_vehicle_number(int *sequence,int count){
    int vehicle_number=0;
    for(int i=0;i<count;i++){
        if(sequence[i]==DEPOT){
            vehicle_number++;
            if(i>0){
                if(sequence[i-1] == DEPOT){
                    vehicle_number--;
                }
            }
        }
    }
    return vehicle_number-1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float caculate_distance(int *seq, int count){
    int sequence[2*MAX_NODES];
    memset(sequence, MAX_INT, sizeof(sequence));
    for(int i=0;i<count;i++)
        sequence[i]=seq[i];

    float dis = 0;
    for(int i=1;i<count;i++){
        dis+=distance_T1[sequence[i-1]][sequence[i]];
    }
    return dis*uni_cost;
}

double split_get_fitness_T1(int *seq, int ind, int type){
    int count=ind+1;
    int sequence[2*MAX_NODES];
    sequence[0]=0;
    for(int i=0;i<count-1;i++)
        sequence[i+1]=seq[i];

    int tmp_seq[2*MAX_NODES];
    int tmp_count;

    const int n=VERTEX_NUMBER;
    double v[n];
    int p[n];
    v[0]=0;
    p[0]=0;
    for(int i=1;i<n;i++)
        v[i]=MAX_INT;

    float load,cost;
    int j;

    int selected;

    float arrived_time;
    float service_time;
    float wait_time;
    float leave_time;
    float route_time;

    for(int i=1;i<vNumber[type];i++){
        load=0;
        cost=0;

        service_time=0;
        leave_time=0;
        route_time=0;
        selected=DEPOT;
        j=i;

        tmp_seq[0]=0;tmp_seq[1]=0;
        tmp_count=2;

        while(j<ind+1&&load<=max_capacity&&route_time<=customer_T1[DEPOT].due_date&&service_time<=customer_T1[selected].due_date){
            selected=sequence[j];
            load = 0;
            tmp_count=add_element(tmp_seq,selected,tmp_count-1,tmp_count);
            for(int k=0; k<tmp_count; k++){
                load += customer_T1[tmp_seq[k]].demand;
            }
            if(load > max_capacity)break;
            //load+=customer_T1[sequence[j]].demand;
            for(int k=0; k<tmp_count; k++){
                load = load - customer_T1[tmp_seq[k]].demand + customer_T1[tmp_seq[k]].pick_up;
                if(load>max_capacity) break;
            }
            if(load>max_capacity) break;
            if(i==j){
                cost=choose_a_type_T1(tmp_seq,tmp_count);
                arrived_time=leave_time+travel_time_T1[DEPOT][sequence[j]];
            }
            else{
                cost=choose_a_type_T1(tmp_seq,tmp_count);
                arrived_time=leave_time+travel_time_T1[sequence[j-1]][sequence[j]];
            }

            wait_time=customer_T1[sequence[j]].ready_time>arrived_time?(customer_T1[sequence[j]].ready_time-arrived_time):0;
            service_time=arrived_time+wait_time;
            leave_time=service_time+customer_T1[sequence[j]].service_time;
            route_time=leave_time+travel_time_T1[DEPOT][sequence[j]];

            if(load<=max_capacity&&route_time<=customer_T1[DEPOT].due_date&&service_time<=customer_T1[sequence[j]].due_date){
                if(v[i-1]+cost<v[j]){
                    v[j]=v[i-1]+cost;
                    p[j]=i-1;
                }
            }
            j++;
        }
    }
    return v[ind];
}


void split_T1(delimiter_individual *del_indi, int *seq, int type)
{
    int count=vNumber[type];
    int sequence[2*MAX_NODES];
    sequence[0]=0;
    for(int i=0;i<count-1;i++)
        sequence[i+1]=seq[i];

    int tmp_seq[2*MAX_NODES];
    int tmp_count;

    const int n=VERTEX_NUMBER;
    float v[n];
    int p[n];
    v[0]=0;
    p[0]=0;
    for(int i=1;i<n;i++)
        v[i]=MAX_INT;

    float load,cost;
    int j;

    int selected;

    float arrived_time;
    float service_time;
    float wait_time;
    float leave_time;
    float route_time;

    for(int i=1;i<vNumber[type];i++){
        load=0;
        cost=0;

        service_time=0;
        leave_time=0;
        route_time=0;
        selected=DEPOT;
        j=i;

        tmp_seq[0]=0;tmp_seq[1]=0;
        tmp_count=2;

        while(j<vNumber[type]&&load<=max_capacity&&route_time<=customer_T1[DEPOT].due_date&&service_time<=customer_T1[selected].due_date){
            selected=sequence[j];
            /*
            load = 0;
            for(int k=0; k<tmp_count; k++){
                load += customer_T1[tmp_seq[k]].demand;
            }
            tmp_count=add_element(tmp_seq,selected,tmp_count-1,tmp_count);
            //load += customer_T1[selected].demand;
            load = load - customer_T1[sequence[j]].demand + customer_T1[sequence[j]].pick_up;
             */
            load = 0;
            tmp_count=add_element(tmp_seq,selected,tmp_count-1,tmp_count);
            for(int k=0; k<tmp_count; k++){
                load += customer_T1[tmp_seq[k]].demand;
            }
            if(load > max_capacity)break;
            //load+=customer_T1[sequence[j]].demand;
            for(int k=0; k<tmp_count; k++){
                load = load - customer_T1[tmp_seq[k]].demand + customer_T1[tmp_seq[k]].pick_up;
                if(load>max_capacity) break;
            }
            if(load>max_capacity) break;

            if(i==j){
                cost=choose_a_type_T1(tmp_seq,tmp_count);
                arrived_time=leave_time+travel_time_T1[DEPOT][sequence[j]];
            }
            else{
                cost=choose_a_type_T1(tmp_seq,tmp_count);
                arrived_time=leave_time+travel_time_T1[sequence[j-1]][sequence[j]];
            }

            wait_time=customer_T1[sequence[j]].ready_time>arrived_time?(customer_T1[sequence[j]].ready_time-arrived_time):0;
            service_time=arrived_time+wait_time;
            leave_time=service_time+customer_T1[sequence[j]].service_time;
            route_time=leave_time+travel_time_T1[DEPOT][sequence[j]];

            if(load<=max_capacity&&route_time<=customer_T1[DEPOT].due_date&&service_time<=customer_T1[sequence[j]].due_date){
                if(v[i-1]+cost<v[j]){
                    v[j]=v[i-1]+cost;
                    p[j]=i-1;
                }
            }
            j++;
        }
    }

    int current_position=p[count-1];
    while(current_position){
        count=add_element(sequence,0,current_position+1,count);
        current_position=p[current_position];
    }
    count=add_element(sequence,0,count,count);
    memcpy(del_indi->sequence,sequence,sizeof(sequence));
    del_indi->count=count;
    del_indi->fitness[type] = v[vNumber[type]-1];
    del_indi->nums_of_vehicle[type] = caculate_vehicle_number(del_indi->sequence,del_indi->count);
    del_indi->distance[type] = caculate_distance(del_indi->sequence, del_indi->count);
    caculate_service_time_T1(del_indi->sequence,del_indi->service_time,del_indi->count);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////new
void caculate_service_time_T1(int *sequence,float *stime,int count){
    float arrived_time;
    float wait_time;
    float service_time;
    float leave_time;

    for(int i=0;i<count;i++){
        if(sequence[i]==DEPOT){
            leave_time=0;
            stime[i]=0;
            continue;
        }
        arrived_time=travel_time_T1[sequence[i-1]][sequence[i]]+leave_time;
        wait_time=customer_T1[sequence[i]].ready_time>arrived_time?(customer_T1[sequence[i]].ready_time-arrived_time):0;
        service_time=arrived_time+wait_time;
        leave_time=service_time+customer_T1[sequence[i]].service_time;

        stime[i]=service_time;
    }
}


bool evaluate_service_time_T1(int *sequence,float *stime,int count){
    for(int i=0;i<count;i++)
        if(stime[i]>customer_T1[sequence[i]].due_date)
            return false;
        return true;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////new

bool evaluate_load_T1(int *sequence,int count){
    int seq[MAX_NODES*2]={0};
    for(int i=0; i<count; i++) seq[i] = sequence[i];
    std::vector<int> one_route;
    for(int i=0; i<count; i++){
        if(seq[i] == 0){
            float load = 0;
            for(int j : one_route){
                load += customer_T1[j].demand;
            }
            if(load > max_capacity) return false;
            for(int j : one_route){
                load = load - customer_T1[j].demand + customer_T1[j].pick_up;
                if(load > max_capacity) return false;
            }
            one_route.clear();
        }
        else{
            one_route.push_back(seq[i]);
        }
    }
    return true;
}


/*
bool evaluate_load_T1(int *sequence,int count){
    int seq[MAX_NODES*2]={0};
    for(int i=0; i<count; i++) seq[i] = sequence[i];
    std::vector<int> one_route;
    for(int i=0; i<count; i++){
        if(seq[i] == 0){
            float load = 0;
            for(int j : one_route){
                load += customer_T1[j].demand;
            }
            if(load > max_capacity) return false;
            one_route.clear();
        }
        else{
            one_route.push_back(seq[i]);
        }
    }
    return true;
}

bool evaluate_load_T2(int *sequence,int count){
    int seq[MAX_NODES*2]={0};
    for(int i=0; i<count; i++) seq[i] = sequence[i];
    std::vector<int> one_route;
    for(int i=0; i<count; i++){
        if(seq[i] == 0){
            float load = 0;
            for(int j : one_route){
                load += customer_T1[j].demand;
            }
            if(load > max_capacity) return false;
            one_route.clear();
        }
        else{
            one_route.push_back(seq[i]);
        }
    }
    return true;
}
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////new

float caculate_fitness_T1(int *sequence,int count){

    float fitness=0;
    float dist;
    float demand;
    int current_type;

    int tmp_seq[2*MAX_NODES];
    int tmp_count;

    float total=0;
    float totalfix=0;

    int i=1;
    while(i<count){

        tmp_seq[0]=0;tmp_seq[1]=0;
        tmp_count=2;

        demand=0;
        dist=0;

        int j;
        for(j=i;j<count;j++){
            demand+=customer_T1[sequence[j]].demand;
            if(sequence[j]==DEPOT)break;
            tmp_count=add_element(tmp_seq,sequence[j],tmp_count-1,tmp_count);
        }

        i=j+1;
        if(demand!=0){
            fitness+=choose_a_type_T1(tmp_seq,tmp_count);
        }
    }
    return fitness;
}


float choose_a_type_T1(int *temseq,int count){
    float off_dis=0;
    float official_cost;
    int seq[2*MAX_NODES];
    for(int i=0;i<count;i++)
        seq[i]=temseq[i];

    for(int i=1;i<count;i++){
        off_dis+=distance_T1[seq[i-1]][seq[i]];
    }
    official_cost = float(vehicle_cost) + off_dis*uni_cost;
    return official_cost;
}


