//
// Created by tsun on 2021/9/15.
//
#include <vector>
#include <algorithm>
#define TASK_NUMBER 2
#define VRP_T1 0
#define VRP_T2 1
#define RMP 0.5
#define EX_TIME 1
#define Cross_rate 0.95
#define Mutate_rate 0.05

#define DEPOT 0

#define T_SIZE 72
#define POP_SIZE 36
#define MAX_NODES 1001

#define MAX_INT 999999999.0

#define PLS 1
#define NP 5
#define NC 18

#define ENUMBER 3600
#define FNUMBER 1

typedef struct vertex{
    int number;
    float x;
    float y;
    float demand;
    float pick_up;
    float ready_time;
    float due_date;
    float service_time;
}vertex;


typedef struct undelimiter_individual
{
    int sequence[2*MAX_NODES];
    std::vector<int> route_list[MAX_NODES];
    int count;
    float fitness[TASK_NUMBER];
    float distance[TASK_NUMBER];
    int nums_of_vehicle[TASK_NUMBER];

    int fRank[TASK_NUMBER];
    float scalar_fitness;
    int skill_factor;
} undelimiter_individual;

typedef struct delimiter_individual
{
    int sequence[2*MAX_NODES];
    int count;

    float service_time[2*MAX_NODES];
    float fitness[TASK_NUMBER];
    int nums_of_vehicle[TASK_NUMBER];
    float distance[TASK_NUMBER];
} delimiter_individual;


extern int lsCount;
extern float lsTime;
extern int vNumber[TASK_NUMBER];
extern float max_capacity;
extern int vehicle_cost;
extern float uni_cost;
extern int VERTEX_NUMBER;
extern int PNM;

extern vertex customer_T1[MAX_NODES];
extern float travel_time_T1[MAX_NODES][MAX_NODES];
extern float distance_T1[MAX_NODES][MAX_NODES];


extern delimiter_individual indi[T_SIZE];
extern undelimiter_individual chro[T_SIZE];

//void MergeSort(int a[], int n);

void cal_cost(int startPoint,int size);
void factorial_cost(int startPoint,int size);
void factorial_rank(int size);
void scalar_fitness(int size);
void skill_factor(int startPoint,int size);

/* initialize.cpp */
void advanced_initial(delimiter_individual *indi);
void encoding(int *sequence);
void decoding(int *sequence,int vNumber);

/* searchOperators.cpp */
void population_sorting(undelimiter_individual *population, int popsize,int taskNumber);
void popSorting_by_scalarFitness(undelimiter_individual *population, int popsize);
void assortative_mating(undelimiter_individual *c1,undelimiter_individual *c2,int current_size);
void mutation(undelimiter_individual *p,undelimiter_individual *c);
void one_level_exchange(undelimiter_individual *p,undelimiter_individual *c);
void cultural_transmission(undelimiter_individual *c1,undelimiter_individual *c2,undelimiter_individual *p1,undelimiter_individual *p2, int type);
void get_decoding_split(undelimiter_individual *c,delimiter_individual *del_c);
void get_original_split(undelimiter_individual *c,delimiter_individual *del_c,delimiter_individual *deco_del_c1);
void simple_xover(undelimiter_individual *c1,undelimiter_individual *c2, undelimiter_individual *p1, undelimiter_individual *p2);
void ordered_xover(undelimiter_individual *c1,undelimiter_individual *c2,undelimiter_individual *p1, undelimiter_individual *p2);
void route_based_xover(undelimiter_individual *c1,undelimiter_individual *p1, undelimiter_individual *p2, int Task);
void transfer_ordered_xover(undelimiter_individual *c1,undelimiter_individual *c2,undelimiter_individual *p1, undelimiter_individual *p2);

void binary_tournament(undelimiter_individual *p1, undelimiter_individual *p2, int popsize);
void rebuilt_chrom(undelimiter_individual *c,delimiter_individual *del_c);
void local_search(delimiter_individual *del, int type);
void perturbation(undelimiter_individual *c, undelimiter_individual *pc);
std::vector<int> CreateRandomNums(int min,int max, int num);

/* arrayOperations.cpp */
void undel_indi_copy(undelimiter_individual *target, undelimiter_individual *source);
void del_indi_copy(delimiter_individual *target, delimiter_individual *source);
void indi_to_chro(delimiter_individual *indi, undelimiter_individual *chro);
int remove_task_seq_delimiters(int *task_seq,int count);
double split_get_fitness_T1(int *seq, int ind, int type);

void print_one_dim_array(int *a,int count);
void print_one_dim_array_float(float *a,int count);
int rand_choose(int num);

int delete_element(int *a, int pos,int count);
int find_element(int *a, int ele,int count);
int add_element(int *a, int e, int pos,int count);
int add_element_float(float *a, float e, int pos,int count);
void swap_element(int *sequence,int pos1,int pos2);
float abs_float(float a, float b);
int get_min_element(int *seq,int count);
int get_min_index(int *seq,int count);

int caculate_vehicle_number(int *sequence,int count);
float caculate_distance(int *seq, int count);

void split_T1(delimiter_individual *del_indi, int *seq, int type);
void split_T2(delimiter_individual *del_indi, int *seq);
void split_T3(delimiter_individual *del_indi, int *seq);
bool evaluate_load_T1(int *sequence,int count);
float caculate_fitness_T1(int *sequence,int count);
float choose_a_type_T1(int *temseq,int count);
void caculate_service_time_T1(int *sequence,float *stime,int count);
bool evaluate_service_time_T1(int *sequence,float *stime,int count);


