//
// Created by tsun on 2021/9/15.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "functions.h"
#include <iostream>
#include <fstream>
#include <assert.h>
#include <string>
#include <sstream>
#include <random>

using namespace std;

float lsTime;
int lsCount;

float bestfitness;
float bestdistance;
int bestvehiclenum;
int vNumber[TASK_NUMBER];
float max_capacity;
int vehicle_cost;
float uni_cost;
int VERTEX_NUMBER;
int PNM;

int task_eval_stage;
int total_task_eval;


/* for VRP_T1 */
vertex customer_T1[MAX_NODES];
float travel_time_T1[MAX_NODES][MAX_NODES];
float distance_T1[MAX_NODES][MAX_NODES];


delimiter_individual indi[T_SIZE];
undelimiter_individual chro[T_SIZE];

std::vector<std::string> split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

void Initialize_VRP_T1(int nodes_num, string input_files_T1){
    vNumber[VRP_T1] = nodes_num;
    ifstream infile;
    infile.open(input_files_T1);   //将文件流对象与文件连接起来
    assert(infile.is_open());   //若失败,则输出错误消息,并终止程序运行
    string buffer;
    while(getline(infile,buffer))
    {
        std::vector<std::string> results = split(buffer, ',');
        if(results.size() >= 6){
            int node = stoi(results[0]);
            float demand = stof(results[1]);
            float pick_up = stof(results[2]);
            float s_time = stof(results[3]);
            float e_time = stof(results[4]);
            float service = stof(results[5]);
            customer_T1[node].number = node;
            customer_T1[node].demand = demand;
            customer_T1[node].pick_up = pick_up;
            customer_T1[node].x = 0;
            customer_T1[node].y = 0;
            customer_T1[node].ready_time = s_time;
            customer_T1[node].due_date = e_time;
            customer_T1[node].service_time = service;
        }
        else{
            int pre_node = stoi(results[0]);
            int node = stoi(results[1]);
            float travel_dis = stof(results[2]);
            float travel_time = stof(results[3]);
            distance_T1[pre_node][pre_node] = 0;
            distance_T1[pre_node][node] = travel_dis;
            travel_time_T1[pre_node][pre_node] = 0;
            travel_time_T1[pre_node][node] = travel_time;
        }
    }
    infile.close();
}


void individual_clear(undelimiter_individual &c1){
    memset(c1.sequence,0,2*VERTEX_NUMBER*sizeof(int));
    for (auto & i : c1.route_list) i.clear();
    memset(c1.fitness, 0, TASK_NUMBER);
    c1.count = 0;
    memset(c1.distance, 0, TASK_NUMBER);
    memset(c1.nums_of_vehicle, 0, TASK_NUMBER);
    memset(c1.fRank, 0, TASK_NUMBER);
    c1.scalar_fitness = 0;
    c1.skill_factor = 0;
}

int main(int argc, char **argv)
{
    // input format
    /*
    program input_file nodes_num output_file capacity vehicle_cost unit_cost ls_num
    */

    argv[0]= const_cast<char*>("../cmake-build-debug/VRPHTO_clion");
    argv[1]= const_cast<char*>("../benchmark/200_1.vrpsdptw");
    argv[2]= const_cast<char*>("201");
    argv[3]= const_cast<char*>("../result/200_1.txt");
    argv[4]= const_cast<char*>("2.5");
    argv[5]= const_cast<char*>("300");
    argv[6]= const_cast<char*>("0.014");
    argv[7]= const_cast<char*>("10");

    FILE *fw_T1;
    string input_files_T1;
    string output_file_T1;

    input_files_T1 = argv[1];
    VERTEX_NUMBER = atoi(argv[2]);
    output_file_T1 = argv[3];
    max_capacity = atof(argv[4]);
    vehicle_cost = atoi(argv[5]);
    uni_cost = atof(argv[6]);
    PNM = atoi(argv[7]);
    double lower = 0.7;

    /* Initialize VRP_T1 */
    Initialize_VRP_T1(VERTEX_NUMBER, input_files_T1);
    for(int i=0;i<POP_SIZE;i++){
        encoding(chro[i].sequence);//初始化长度180的没分隔符的序列
    }

    int current_size;
    for(int ex_time=0;ex_time<EX_TIME;ex_time++){
        //srand(32);
        srand(unsigned(time(0)));
        factorial_cost(0,POP_SIZE);
        factorial_rank(POP_SIZE);
        scalar_fitness(POP_SIZE);
        skill_factor(0,POP_SIZE);

        current_size=POP_SIZE;
        population_sorting(chro,current_size, VRP_T1);
        bestfitness=chro[0].fitness[VRP_T1];
        bestdistance=chro[0].distance[VRP_T1];
        bestvehiclenum=chro[0].nums_of_vehicle[VRP_T1];

        popSorting_by_scalarFitness(chro,current_size);

        fw_T1=fopen(output_file_T1.c_str(),"w+");
        printf("%s\n", input_files_T1.c_str());
        printf("%s\n", output_file_T1.c_str());

        clock_t start_time=clock();
        clock_t used = 0;
        lsTime=0;lsCount=0;

        total_task_eval = 0;

        for(int phase=1;phase<=NP;){
            task_eval_stage = 0;
            default_random_engine el;
            uniform_real_distribution<double> uni_elem(lower,1);
            double ran = uni_elem(el);
            vNumber[VRP_T2] = int((VERTEX_NUMBER-1)*ran+1);

            //while(task_eval[VRP_T1]<ENUMBER||task_eval[VRP_T2]<ENUMBER){
            while(task_eval_stage<ENUMBER){
                //bool flag1 = false, flag2 = false;
                undelimiter_individual c1,c2;
                individual_clear(c1);
                individual_clear(c2);
                assortative_mating(&c1,&c2,current_size);

                if(task_eval_stage<ENUMBER){
                    /*
                    if (flag1)
                    undel_indi_copy(&chro[current_size],&pc1);
                    else
                    undel_indi_copy(&chro[current_size],&c1);
                    */
                    undel_indi_copy(&chro[current_size],&c1);
                    current_size++;
                    if (c1.skill_factor == 0){
                        if(c1.fitness[c1.skill_factor]<bestfitness){
                            used = (clock() - start_time) / CLOCKS_PER_SEC;
                            bestfitness=c1.fitness[c1.skill_factor];
                            bestdistance=c1.distance[c1.skill_factor];
                            bestvehiclenum=c1.nums_of_vehicle[c1.skill_factor];
                            printf("vehicle:%d, distance:%.2f, fitness:%.2f, time:%d\n",c1.nums_of_vehicle[c1.skill_factor], c1.distance[c1.skill_factor], c1.fitness[c1.skill_factor], used);
                        }
                    }
                }
                if(current_size>=T_SIZE){
                    factorial_rank(current_size);
                    scalar_fitness(current_size);
                    popSorting_by_scalarFitness(chro,current_size);
                    current_size=POP_SIZE;
                }

                if(task_eval_stage<ENUMBER){
                    undel_indi_copy(&chro[current_size],&c2);
                    current_size++;
                    if (c2.skill_factor == 0){
                        if(c2.fitness[c2.skill_factor]<bestfitness){
                            used = (clock() - start_time) / CLOCKS_PER_SEC;
                            bestfitness=c2.fitness[c2.skill_factor];
                            bestdistance=c2.distance[c2.skill_factor];
                            bestvehiclenum=c2.nums_of_vehicle[c2.skill_factor];
                            printf("vehicle:%d, distance:%.2f, fitness:%.2f, time:%d\n",c2.nums_of_vehicle[c2.skill_factor], c2.distance[c2.skill_factor], c2.fitness[c2.skill_factor], used);
                        }
                    }
                }
                if(current_size>=T_SIZE){
                    factorial_rank(current_size);
                    scalar_fitness(current_size);
                    popSorting_by_scalarFitness(chro,current_size);
                    current_size=POP_SIZE;
                }
                used = (clock() - start_time) / CLOCKS_PER_SEC;
                fprintf(fw_T1, "%d ", total_task_eval);
                fprintf(fw_T1, "%d ",bestvehiclenum);
                fprintf(fw_T1, "%0.2f ",bestdistance);
                fprintf(fw_T1, "%0.2f ",bestfitness);
                fprintf(fw_T1, "%d\n", used);
                task_eval_stage ++;
                total_task_eval ++;

                fprintf(fw_T1, "%d ", total_task_eval);
                fprintf(fw_T1, "%d ",bestvehiclenum);
                fprintf(fw_T1, "%0.2f ",bestdistance);
                fprintf(fw_T1, "%0.2f ",bestfitness);
                fprintf(fw_T1, "%d\n", used);
                task_eval_stage ++;
                total_task_eval ++;
            }
            popSorting_by_scalarFitness(chro,POP_SIZE);
            phase ++;
            if(phase<=NP){
                srand(time(NULL));
                for(int i=NC;i<POP_SIZE;i++){
                    encoding(chro[i].sequence);
                }
                factorial_cost(NC,POP_SIZE);
                factorial_rank(POP_SIZE);
                scalar_fitness(POP_SIZE);
                skill_factor(NC,POP_SIZE);
            }
        }
        used = (clock() - start_time) / CLOCKS_PER_SEC;
        printf("exeTime: %d\n", used);
        printf("lsTime: %f\n", lsTime);
        printf("lsCount: %d\n", lsCount);

        ////////////////////////////////////////////
        fclose(fw_T1);
    }

};

