//
// Created by tsun on 2021/9/15.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"
#include <time.h>
#include <float.h>

#define DOUBLE 0
#define SINGLE 1

void population_sorting(undelimiter_individual *population, int popsize,int taskNumber)
{
    undelimiter_individual tmp_chro;
    for (int i = 0; i < popsize-1; i++)
    {
        for (int j = i+1; j < popsize; j++)
        {
            if (population[j].fitness[taskNumber] < population[i].fitness[taskNumber])
            {
                undel_indi_copy(&tmp_chro, &population[i]);
                undel_indi_copy(&population[i], &population[j]);
                undel_indi_copy(&population[j], &tmp_chro);
            }
        }
    }
}

void popSorting_by_scalarFitness(undelimiter_individual *population, int popsize)
{
    undelimiter_individual tmp_chro;
    for (int i = 0; i < popsize-1; i++)
    {
        for (int j = i+1; j < popsize; j++)
        {
            if (population[j].scalar_fitness > population[i].scalar_fitness)
            {
                undel_indi_copy(&tmp_chro, &population[i]);
                undel_indi_copy(&population[i], &population[j]);
                undel_indi_copy(&population[j], &tmp_chro);
            }
        }
    }
}

void assortative_mating(undelimiter_individual *c1,undelimiter_individual *c2,int current_size){
    undelimiter_individual p1, p2;
    int type;
    binary_tournament(&p1,&p2,current_size);

    if(p1.skill_factor==p2.skill_factor){
        route_based_xover(c1,&p1,&p2,p1.skill_factor);
        route_based_xover(c2,&p2,&p1,p1.skill_factor);
        type=DOUBLE;
    }
    else{
        transfer_ordered_xover(c1,c2,&p1,&p2);
        type=DOUBLE;
    }

    // transfer_ordered_xover(c1,c2,&p1,&p2);
    /*
    else{
        //mutation(&p1,c1);
        //mutation(&p2,c2);
        one_level_exchange(&p1, c1);
        one_level_exchange(&p2, c2);
        type=SINGLE;
    }
    */
    // local search
    cultural_transmission(c1,c2,&p1,&p2,type);
}

std::vector<int> CreateRandomNums(int min,int max, int num)
{
    std::vector<int> res;
    res.clear();
    if (max - min + 1 < num)
    {
        return res;
    }
    //srand(time(0));
    for (auto i{0}; i < num; i++)
    {
        while (true)
        {
            auto temp{ rand() % (max + 1 - min) + min };
            auto iter{ find(res.begin(),res.end(),temp) };
            if (res.end() == iter)
            {
                res.push_back(temp);
                break;
            }
        }
    }
    return res;
}


void transfer_ordered_xover(undelimiter_individual *c1,undelimiter_individual *c2,undelimiter_individual *p1, undelimiter_individual *p2){
    int a, b;
    int i, j, k;
    int seq1[2*MAX_NODES], seq2[2*MAX_NODES];

    undelimiter_individual temp_c1;
    undelimiter_individual temp_c2;

    int length =VERTEX_NUMBER-1;
    int length1=length/2;

    a = rand_choose(length);
    b = (a+1+rand_choose(length1))%length;

    if (a > b)
    {
        int tmp = a;
        a = b;
        b = tmp;
    }
    if((b-a+1)==length)a++;
    int tem;

    memcpy(seq1, p1->sequence, sizeof(p1->sequence));
    memcpy(seq2, p2->sequence, sizeof(p2->sequence));
    memcpy(temp_c1.sequence, p1->sequence, sizeof(p1->sequence));
    temp_c1.count=length;
    int count1=length,count2=length;

    for(i=a;i<=b;i++){
        tem=seq1[i];
        count2=delete_element(seq2,find_element(seq2,seq1[i],count2),count2);
    }

    int offset=count2-(b+1)%count2+1;
    for(int i=0;i<count2;i++)
        temp_c1.sequence[(b+i+1)%count1]=seq2[(offset+i)%count2];

    memcpy(seq1, p1->sequence, sizeof(p1->sequence));
    memcpy(seq2, p2->sequence, sizeof(p2->sequence));
    memcpy(temp_c2.sequence, p2->sequence, sizeof(p2->sequence));
    temp_c2.count=length;
    count1=length,count2=length;
    for(i=a;i<=b;i++){
        tem=seq2[i];
        count1=delete_element(seq1,find_element(seq1,seq2[i],count1),count1);
    }

    offset=count1-(b+1)%count1+1;
    for(int i=0;i<count1;i++)
        temp_c2.sequence[(b+i+1)%count2]=seq1[(offset+i)%count1];

    undel_indi_copy(c1,&temp_c1);
    undel_indi_copy(c2,&temp_c2);
}

void route_based_xover(undelimiter_individual *c1,undelimiter_individual *p1, undelimiter_individual *p2, int Task){

    int selected_1 = rand()%(p1->nums_of_vehicle[Task]);
    int selected_2 = rand()%(p2->nums_of_vehicle[Task]);
    std::vector<int> child[MAX_NODES];
    // 复制p2给child
    child[selected_2] = p1->route_list[selected_1];
    for (int i=0; i<selected_2; i++) child[i] = p2->route_list[i];
    if (selected_2 < p2->nums_of_vehicle[Task]-1){
        for (int i=selected_2+1;i<p2->nums_of_vehicle[Task];i++) child[i] = p2->route_list[i];
    }
    int tttt = 2;
    // 删除重复的customer
    for (int i=0; i<selected_2;i++){
        int r_len = int(child[i].size());
        if (r_len<=0) continue;
        for(int j=r_len-1;j>=0;j--){
            for (auto sa=p1->route_list[selected_1].begin(); sa!=p1->route_list[selected_1].end(); sa++){
                int item = child[i][j];
                if (item == *sa){
                    child[i].erase(child[i].begin()+j);
                    break;
                }
            }
        }
    }
    if (selected_2 < p2->nums_of_vehicle[Task]-1){
        for (int i=selected_2+1; i<p2->nums_of_vehicle[Task];i++){
            int r_len = int(child[i].size());
            if (r_len<=0) continue;
            for(int j=r_len-1;j>=0;j--){
                for (auto sa=p1->route_list[selected_1].begin(); sa!=p1->route_list[selected_1].end(); sa++){
                    int item = child[i][j];
                    if (item == *sa){
                        child[i].erase(child[i].begin()+j);
                        break;
                    }
                }
            }
        }
    }
    int ssss = 3;
    int route_count = 0;
    std::vector<int> child_new[MAX_NODES];
    for(int i=0; i<p2->nums_of_vehicle[Task];i++){
        if(!child[i].empty()){
            child_new[route_count] = child[i];
            route_count ++;
        }
    }
    int tmp1 = 2;
    // 需要插入的customer
    std::vector<int> unrouted_nodes;
    for (auto it=p2->route_list[selected_2].begin();it!=p2->route_list[selected_2].end();it++){
        int j;
        for (j=0;j<int(p1->route_list[selected_1].size());j++){
            if (p1->route_list[selected_1][j] == *it){
                break;
            }
        }
        if (j >= int(p1->route_list[selected_1].size())){
            unrouted_nodes.push_back(*it);
        }
    }
    int tmp2 = 2;
    //逐个插入customer
    for (auto it=unrouted_nodes.begin();it!=unrouted_nodes.end();it++){
        double best_fitness = DBL_MAX;
        int min_i = -1;
        int min_j = -1;
        for (int i=0; i< route_count; i++){
            for (int j=0; j<=int(child_new[i].size()); j++){
                int seq[MAX_NODES]={0};
                std::vector<int> tmp_route;
                if (j==int(child_new[i].size())){
                    tmp_route = child_new[i];
                    tmp_route.push_back(*it);
                }
                else{
                    tmp_route = child_new[i];
                    tmp_route.insert(tmp_route.begin()+j,*it);
                }
                int ind = 0;
                for(int k=0; k<i; k++){
                    for(auto m=child_new[k].begin();m!=child_new[k].end();m++){
                        seq[ind] = *m;
                        ind ++;
                    }
                }
                for(auto m=tmp_route.begin();m!=tmp_route.end();m++){
                    seq[ind] = *m;
                    ind ++;
                }
                if(i<route_count-1){
                    for(int k=i+1;k<route_count;k++){
                        for(auto m=child_new[k].begin();m!=child_new[k].end();m++){
                            seq[ind] = *m;
                            ind ++;
                        }
                    }
                }
                double new_fintness;
                new_fintness = split_get_fitness_T1(seq, ind, Task);

                if (new_fintness<best_fitness){
                    best_fitness = new_fintness;
                    min_i = i;
                    min_j = j;
                }
            }
        }
        if (min_j == int(child_new[min_i].size())){
            child_new[min_i].push_back(*it);
        }
        else{
            child_new[min_i].insert(child_new[min_i].begin()+min_j,*it);
        }
    }
    // 生成的新序列给c1
    int n_seq[MAX_NODES]={0};
    int ind = 0;
    for (int i=0;i<route_count;i++){
        for (auto it=child_new[i].begin();it!=child_new[i].end();it++){
            n_seq[ind] = *it;
            ind ++;
        }
    }
    ind = 0;
    for (int i=0; i<p2->count; i++){
        if (p2->sequence[i] < vNumber[Task]){
            c1->sequence[i] = n_seq[ind];
            ind ++;
        }
        else{
            c1->sequence[i] = p2->sequence[i];
        }
    }

    int length =VERTEX_NUMBER-1;
    c1->count = length;
}

void binary_tournament(undelimiter_individual *p1, undelimiter_individual *p2, int popsize)
{
    int k1, k2, selk;
    int candi1[T_SIZE], candi2[T_SIZE];
    int count1=popsize,count2=popsize;

    for (int i =0; i < popsize; i++)
    {
        candi1[i] = i;
    }
    memcpy(candi2, candi1, sizeof(candi1));

    k1 = rand_choose(count1);
    k2 = rand_choose(count1);
    if (k1 < k2)
    {
        selk = k1;
    }
    else
    {
        selk = k2;
    }
    undel_indi_copy(p1, &chro[candi1[selk]]);
    count2=delete_element(candi2, selk,count2);

    k1 = rand_choose(count2);
    k2 = rand_choose(count2);
    if (k1 < k2)
    {
        selk = k1;
    }
    else
    {
        selk = k2;
    }
    undel_indi_copy(p2, &chro[candi2[selk]]);
}

void mutation(undelimiter_individual *p,undelimiter_individual *c){
    int pos1,pos2;
    int count;
    int seq[2*MAX_NODES];
    memcpy(seq,p->sequence,sizeof(seq));
    count=VERTEX_NUMBER-1;

    for(int i=0;i<1;i++){
        pos1=rand()%count;
        pos2=rand()%count;
        while(pos1==pos2)
            pos2=rand()%count;

        swap_element(seq,pos1,pos2);
    }

    memcpy(c->sequence,seq,sizeof(seq));
    c->count=count;
}

void one_level_exchange(undelimiter_individual *p,undelimiter_individual *c){

    double prob[MAX_NODES] = {0};
    double prob_tmp[MAX_NODES] = {0};
    double total_nodes = 0;
    for (int i=0; i<p->nums_of_vehicle[p->skill_factor];i++){
        total_nodes += int(p->route_list[i].size());
    }
    int min_route_size = MAX_INT;
    for (int i=0; i<p->nums_of_vehicle[p->skill_factor]; i++){
        if (int(p->route_list[i].size()) < min_route_size){
            min_route_size = int(p->route_list[i].size());
        }
    }
    for (int i=0; i<p->nums_of_vehicle[p->skill_factor]; i++) prob_tmp[i] = 1.0*(int(p->route_list[i].size()))/min_route_size;
    double tprob = 0;
    for (int i=0; i<p->nums_of_vehicle[p->skill_factor]; i++) tprob += 1/prob_tmp[i];
    double unit_prob = 1/tprob;

    for (int i=0; i<p->nums_of_vehicle[p->skill_factor]; i++) prob[i+1] = 1/prob_tmp[i]*unit_prob;

    double prob_gap[MAX_NODES] = {0};
    for (int i=1; i<=p->nums_of_vehicle[p->skill_factor]; i++){
        double pp = 0;
        for (int j=0; j<=i; j++){
            pp += prob[j];
        }
        prob_gap[i] = pp;
    }

    float ran=1.0*rand()/(RAND_MAX);
    int route_selected = -1;
    for (int i=1; i<=p->nums_of_vehicle[p->skill_factor]; i++){
        if(ran> prob_gap[i-1] && ran< prob_gap[i]){
            route_selected = i-1;
            break;
        }
    }

    int route_count = 0;
    std::vector<int> unrouted_nodes;
    unrouted_nodes = p->route_list[route_selected];

    std::vector<int> tmp_parent[MAX_NODES];
    for (int i=0; i<p->nums_of_vehicle[p->skill_factor]; i++){
        if (i != route_selected){
            tmp_parent[route_count] = p->route_list[i];
            route_count ++;
        }
    }

    //逐个插入customer
    for (auto it=unrouted_nodes.begin();it!=unrouted_nodes.end();it++){
        double best_fitness = DBL_MAX;
        int min_i = -1;
        int min_j = -1;
        for (int i=0; i< route_count; i++){
            for (int j=0; j<=int(tmp_parent[i].size()); j++){
                int seq[MAX_NODES]={0};
                std::vector<int> tmp_route;
                if (j==int(tmp_parent[i].size())){
                    tmp_route = tmp_parent[i];
                    tmp_route.push_back(*it);
                }
                else{
                    tmp_route = tmp_parent[i];
                    tmp_route.insert(tmp_route.begin()+j,*it);
                }
                int ind = 0;
                for(int k=0; k<i; k++){
                    for(auto m=tmp_parent[k].begin();m!=tmp_parent[k].end();m++){
                        seq[ind] = *m;
                        ind ++;
                    }
                }
                for(auto m=tmp_route.begin();m!=tmp_route.end();m++){
                    seq[ind] = *m;
                    ind ++;
                }
                if(i<route_count-1){
                    for(int k=i+1;k<route_count;k++){
                        for(auto m=tmp_parent[k].begin();m!=tmp_parent[k].end();m++){
                            seq[ind] = *m;
                            ind ++;
                        }
                    }
                }
                double new_fintness;
                new_fintness = split_get_fitness_T1(seq, ind, p->skill_factor);

                if (new_fintness<best_fitness){
                    best_fitness = new_fintness;
                    min_i = i;
                    min_j = j;
                }
            }
        }
        if (min_j == int(tmp_parent[min_i].size())){
            tmp_parent[min_i].push_back(*it);
        }
        else{
            tmp_parent[min_i].insert(tmp_parent[min_i].begin()+min_j,*it);
        }
    }
    // 生成的新序列给c1
    int n_seq[MAX_NODES]={0};
    int ind = 0;
    for (int i=0;i<route_count;i++){
        for (auto it=tmp_parent[i].begin();it!=tmp_parent[i].end();it++){
            n_seq[ind] = *it;
            ind ++;
        }
    }
    ind = 0;
    for (int i=0; i<p->count; i++){
        if (p->sequence[i] < vNumber[p->skill_factor]){
            c->sequence[i] = n_seq[ind];
            ind ++;
        }
        else{
            c->sequence[i] = p->sequence[i];
        }
    }
    int length =VERTEX_NUMBER-1;
    c->count = length;
}

void cultural_transmission(undelimiter_individual *c1,undelimiter_individual *c2,undelimiter_individual *p1,undelimiter_individual *p2, int type){
    delimiter_individual del_c1,del_c2;

    if(type==DOUBLE){
        float ran=1.0*rand()/RAND_MAX;
        if(ran<0.5){
            c1->skill_factor=p1->skill_factor;
            c2->skill_factor=p1->skill_factor;
        }
        else{
            c1->skill_factor=p2->skill_factor;
            c2->skill_factor=p2->skill_factor;
        }
    }
    if(type==SINGLE){
        c1->skill_factor=p1->skill_factor;
        c2->skill_factor=p2->skill_factor;
    }

    get_decoding_split(c1,&del_c1);
    get_decoding_split(c2,&del_c2);

    clock_t lsstart;
    double random = 1.0*rand()/RAND_MAX;
    if(random<PLS){
        lsCount++;
        lsstart=clock();
        local_search(&del_c1,c1->skill_factor);
        local_search(&del_c2,c2->skill_factor);
        lsTime+=(float)(clock()-lsstart)/CLOCKS_PER_SEC;
    }

    // Update route_list
    for (auto & i : c1->route_list) i.clear();
    int index = 0;
    for (int i=0; i<del_c1.count-1; i++){
        if (del_c1.sequence[i]==0 && del_c1.sequence[i+1]!=0){
            while(del_c1.sequence[i+1]!=0){
                c1->route_list[index].push_back(del_c1.sequence[i+1]);
                i++;
            }
            index ++;
        }
    }

    for (auto & i : c2->route_list) i.clear();
    index = 0;
    for (int i=0; i<del_c2.count-1; i++){
        if (del_c2.sequence[i]==0 && del_c2.sequence[i+1]!=0){
            while(del_c2.sequence[i+1]!=0){
                c2->route_list[index].push_back(del_c2.sequence[i+1]);
                i++;
            }
            index ++;
        }
    }

    del_c1.count=remove_task_seq_delimiters(del_c1.sequence,del_c1.count);
    del_c2.count=remove_task_seq_delimiters(del_c2.sequence,del_c2.count);

    rebuilt_chrom(c1,&del_c1);
    rebuilt_chrom(c2,&del_c2);

    c1->fitness[c1->skill_factor]=del_c1.fitness[c1->skill_factor];
    c1->count = VERTEX_NUMBER-1;
    c1->nums_of_vehicle[c1->skill_factor] = del_c1.nums_of_vehicle[c1->skill_factor];
    c1->distance[c1->skill_factor] = del_c1.distance[c1->skill_factor];

    c2->fitness[c2->skill_factor]=del_c2.fitness[c2->skill_factor];
    c2->count = VERTEX_NUMBER-1;
    c2->nums_of_vehicle[c2->skill_factor] = del_c2.nums_of_vehicle[c2->skill_factor];
    c2->distance[c2->skill_factor] = del_c2.distance[c2->skill_factor];
}

void get_decoding_split(undelimiter_individual *c,delimiter_individual *del_c){
    int seq[MAX_NODES];
    memcpy(seq,c->sequence,sizeof(seq));
    int type = c->skill_factor;

    decoding(seq,vNumber[type]);
    split_T1(del_c, seq, type);
    c->fitness[type]=del_c->fitness[type];
    for(int i=0; i<TASK_NUMBER; i++){
        if(i != type){
            c->fitness[i]=MAX_INT;
        }
    }

}

void rebuilt_chrom(undelimiter_individual *c,delimiter_individual *del_c){
    int j=0;
    int maxNumber=VERTEX_NUMBER;
    maxNumber-=1;
    for(int i=0;i<maxNumber;i++){
        if(c->sequence[i]<vNumber[c->skill_factor]){
            c->sequence[i]=del_c->sequence[j];
            j++;
        }
    }
}

void local_search(delimiter_individual *del,int type){
    int sequence[2*MAX_NODES];
    float service_time[2*MAX_NODES];
    int count=del->count;
    float current_fitness,best_fitness;

    int u,v,x,y;
    int mark=0,improved=0;
    int cnt = 0;

    while(1){
        memcpy(sequence,del->sequence,sizeof(sequence));
        best_fitness=del->fitness[type];
        for(int i=0;i<(count-1);i++){
            u=sequence[i];
            x=sequence[i+1];
            if(sequence[i]==DEPOT){
                if(i==(count-2))mark=1;
                continue;
            }

            for(int j=0;j<(count-1);j++){
                if(j==i)continue;
                v=sequence[j];y=sequence[j+1];
                /*If u is a client node, remove u then insert it after v*/
                if(j>i){
                    count=add_element(sequence,u,j+1,count);
                    count=delete_element(sequence,i,count);
                }
                if(j<i){
                    count=delete_element(sequence,i,count);
                    count=add_element(sequence,u,j+1,count);
                }
                caculate_service_time_T1(sequence,service_time,count);
                if(evaluate_service_time_T1(sequence,service_time,count)&&evaluate_load_T1(sequence,count)){
                    current_fitness=caculate_fitness_T1(sequence,count);
                    if(current_fitness<best_fitness){
                        improved=1;
                        break;
                    }
                }
                memcpy(sequence,del->sequence,sizeof(sequence));

                /*If u and v are clients, swap u and v*/
                if(v!=DEPOT){
                    swap_element(sequence,i,j);
                    caculate_service_time_T1(sequence,service_time,count);
                    if(evaluate_service_time_T1(sequence,service_time,count)&&evaluate_load_T1(sequence,count)){
                        current_fitness=caculate_fitness_T1(sequence,count);
                        if(current_fitness<best_fitness){
                            improved=1;
                            break;
                        }
                    }
                    memcpy(sequence,del->sequence,sizeof(sequence));
                }

                if((i+1)!=j && x!=DEPOT) {
                    /*If u and x are clients, remove them then insert (u, x) after v*/
                    if(i<j){
                        count=add_element(sequence,u,j+1,count);
                        count=add_element(sequence,x,j+2,count);
                        count=delete_element(sequence,i+1,count);
                        count=delete_element(sequence,i,count);
                    }
                    if(i>j){
                        count=delete_element(sequence,i+1,count);
                        count=delete_element(sequence,i,count);
                        count=add_element(sequence,u,j+1,count);
                        count=add_element(sequence,x,j+2,count);
                    }
                    caculate_service_time_T1(sequence,service_time,count);
                    if(evaluate_service_time_T1(sequence,service_time,count)&&evaluate_load_T1(sequence,count)){
                        current_fitness=caculate_fitness_T1(sequence,count);
                        if(current_fitness<best_fitness){
                            improved=1;
                            break;
                        }
                    }
                    memcpy(sequence,del->sequence,sizeof(sequence));
                    /*If u and x are clients, remove them then insert (x, u) after v*/
                    if(i<j){
                        count=add_element(sequence,x,j+1,count);
                        count=add_element(sequence,u,j+2,count);
                        count=delete_element(sequence,i+1,count);
                        count=delete_element(sequence,i,count);
                    }
                    if(i>j){
                        count=delete_element(sequence,i+1,count);
                        count=delete_element(sequence,i,count);
                        count=add_element(sequence,x,j+1,count);
                        count=add_element(sequence,u,j+2,count);
                    }
                    caculate_service_time_T1(sequence,service_time,count);
                    if(evaluate_service_time_T1(sequence,service_time,count)&&evaluate_load_T1(sequence,count)){
                        current_fitness=caculate_fitness_T1(sequence,count);
                        if(current_fitness<best_fitness){
                            improved=1;
                            break;
                        }
                    }
                    memcpy(sequence,del->sequence,sizeof(sequence));

                    /*If u, x and v are clients, swap (u,x) and v*/
                    if(v!=DEPOT){
                        swap_element(sequence,i,j);
                        if(i<j){
                            count=add_element(sequence,x,j+1,count);
                            count=delete_element(sequence,i+1,count);
                        }
                        if(i>j){
                            count=delete_element(sequence,i+1,count);
                            count=add_element(sequence,x,j+1,count);
                        }
                        caculate_service_time_T1(sequence,service_time,count);
                        if(evaluate_service_time_T1(sequence,service_time,count)&&evaluate_load_T1(sequence,count)){
                            current_fitness=caculate_fitness_T1(sequence,count);
                            if(current_fitness<best_fitness){
                                improved=1;
                                break;
                            }
                        }
                        memcpy(sequence,del->sequence,sizeof(sequence));
                    }
                    /*If (u, x) and (v, y) are clients, swap (u, x) and (v, y)*/
                    if(v!=DEPOT && y!=DEPOT){
                        swap_element(sequence,i,j);
                        swap_element(sequence,i+1,j+1);
                        caculate_service_time_T1(sequence,service_time,count);
                        if(evaluate_service_time_T1(sequence,service_time,count)&&evaluate_load_T1(sequence,count)){
                            current_fitness=caculate_fitness_T1(sequence,count);
                            if(current_fitness<best_fitness){
                                improved=1;
                                break;
                            }
                        }
                        memcpy(sequence,del->sequence,sizeof(sequence));
                    }

                    /*replace (u, x) and (v, y) by (u, v) and (x, y)*/
                    if(v!=DEPOT && y!=DEPOT){
                        swap_element(sequence,i+1,j);
                        caculate_service_time_T1(sequence,service_time,count);
                        if(evaluate_service_time_T1(sequence,service_time,count)&&evaluate_load_T1(sequence,count)){
                            current_fitness=caculate_fitness_T1(sequence,count);
                            if(current_fitness<best_fitness){
                                improved=1;
                                break;
                            }
                        }
                        memcpy(sequence,del->sequence,sizeof(sequence));
                    }

                    /*replace (u, x) and (v, y) by (u, y) and (x, v)*/
                    if(v!=DEPOT && y!=DEPOT){
                        swap_element(sequence,i+1,j+1);
                        swap_element(sequence,j,j+1);
                        caculate_service_time_T1(sequence,service_time,count);
                        if(evaluate_service_time_T1(sequence,service_time,count)&&evaluate_load_T1(sequence,count)){
                            current_fitness=caculate_fitness_T1(sequence,count);
                            if(current_fitness<best_fitness){
                                improved=1;
                                break;
                            }
                        }
                        memcpy(sequence,del->sequence,sizeof(sequence));
                    }
                }

                /*2-opt*: interchange the last parts of two trips */
                if(v!=DEPOT){
                    bool in_same_route=true;
                    if(j>i){
                        for(int k=i;k<j;k++)
                            if(sequence[k]==DEPOT){
                                in_same_route=false;
                                break;
                            }
                    }
                    if(j<i){
                        for(int k=j;k<i;k++)
                            if(sequence[k]==DEPOT){
                                in_same_route=false;
                                break;
                            }
                    }

                    if(!in_same_route){
                        /*case 1:before_u->u->x->after_x and before_v->v->y->after_y，to，before_u->u->y->after_y and before_v->v->x->after_x*/
                        int iu=i;int iv=j;
                        int sqiu,sqiv;
                        while(sequence[iu]!=DEPOT&&sequence[iv]!=DEPOT){
                            swap_element(sequence,iu,iv);
                            iu++;
                            iv++;
                        }
                        if(sequence[iu]!=DEPOT){
                            while(sequence[iu]!=DEPOT){
                                sqiu=sequence[iu];
                                if(j>i){
                                    count=add_element(sequence,sqiu,iv,count);
                                    count=delete_element(sequence,iu,count);}
                                if(j<i){
                                    count=delete_element(sequence,iu,count);
                                    count=add_element(sequence,sqiu,iv,count);
                                    iu++;
                                    iv++;
                                }
                            }
                        }
                        if(sequence[iv]!=DEPOT){
                            while(sequence[iv]!=DEPOT){
                                sqiv=sequence[iv];
                                if(j>i){
                                    count=delete_element(sequence,iv,count);
                                    count=add_element(sequence,sqiv,iu,count);
                                    iu++;
                                    iv++;
                                }
                                if(j<i){
                                    count=add_element(sequence,sqiv,iu,count);
                                    count=delete_element(sequence,iv,count);
                                }
                            }
                        }
                        caculate_service_time_T1(sequence,service_time,count);
                        if(evaluate_service_time_T1(sequence,service_time,count)&&evaluate_load_T1(sequence,count)){
                            current_fitness=caculate_fitness_T1(sequence,count);
                            if(current_fitness<best_fitness){
                                improved=1;
                                break;
                            }
                        }
                        memcpy(sequence,del->sequence,sizeof(sequence));
                        /*case 2:before_u->u->x->after_x and before_v->v->y->after_y，to，before_u->u->v->before_v and after_x->x->y->after_y*/
                        iu=i;iv=j;
                        while(sequence[iu]!=DEPOT&&sequence[iv]!=DEPOT){
                            swap_element(sequence,iu,iv);
                            iu++;
                            iv--;
                        }
                        if(sequence[iu]!=DEPOT){
                            while(sequence[iu]!=DEPOT){
                                sqiu=sequence[iu];
                                if(j>i){
                                    count=add_element(sequence,sqiu,iv+1,count);
                                    count=delete_element(sequence,iu,count);
                                    iv--;
                                }
                                if(j<i){
                                    count=delete_element(sequence,iu,count);
                                    count=add_element(sequence,sqiu,iv+1,count);
                                    iu++;
                                }
                            }
                        }
                        if(sequence[iv]!=DEPOT){
                            while(sequence[iv]!=DEPOT){
                                sqiv=sequence[iv];
                                if(j>i){
                                    count=delete_element(sequence,iv,count);
                                    count=add_element(sequence,sqiv,iu,count);
                                    iu++;
                                }
                                if(j<i){
                                    count=add_element(sequence,sqiv,iu,count);
                                    count=delete_element(sequence,iv,count);
                                    iv--;
                                }
                            }
                        }
                        caculate_service_time_T1(sequence,service_time,count);
                        if(evaluate_service_time_T1(sequence,service_time,count)&&evaluate_load_T1(sequence,count)){
                            current_fitness=caculate_fitness_T1(sequence,count);
                            if(current_fitness<best_fitness){
                                improved=1;
                                break;
                            }
                        }
                        memcpy(sequence,del->sequence,sizeof(sequence));
                    }
                }
            }
            if(improved){
                improved=0;
                memcpy(del->sequence,sequence,sizeof(sequence));
                del->fitness[type]=current_fitness;
                memcpy(del->service_time,service_time,sizeof(service_time));
                del->count=count;
                del->nums_of_vehicle[type]=caculate_vehicle_number(del->sequence,del->count);
                del->distance[type] = caculate_distance(del->sequence, del->count);
                cnt ++;
                if (cnt >= PNM){
                    mark = 1;
                }
                //mark = 1;
                break;
            }
            if(i==(count-2)){
                mark=1;
            }
        }
        if(mark)
            break;
    }

}



