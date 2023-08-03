#include<cstdio>
#include<pthread.h>
#include<semaphore.h>
#include<queue>
#include <unistd.h>
#include <random>
#include <bits/stdc++.h>
using namespace std;

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int n,m,w,x,y;

default_random_engine generator;
poisson_distribution<int> distribution(4.1);
int rand_number = distribution(generator);

auto start = chrono::steady_clock::now();

int get_curr_time(){
    auto end = std::chrono::steady_clock::now();
    chrono::duration<double> elapsed_seconds = end-start;
    return elapsed_seconds.count();
}
class group;
class student
{
    private:
    int std_id;
    group *grp;
    bool is_leader=false;
    public:
    student(int std_id,group *g)
    {
        this->std_id=std_id;
        this->grp=g;
    }
    int get_id()
    {
        return std_id;
    }
    group * get_group()
    {
        return grp;
    }
    void set_leader()
    {
        is_leader=true;
    }
    void print()
    {
        printf("The student id: %d\n",get_id());
    }
    void printing_start()
    {
        printf("Student %d has arrived at the print station at time %d\n",std_id,get_curr_time());
    }
    void printing_end()
    {
        printf("Student %d has finished printing at time %d\n",std_id,get_curr_time());
    }
   
};
 void * student_work(void *st)
    {
        student * std = (student *) st;
        printf("I am inside std id %d\n",std->get_id());
        pthread_exit(NULL);
    }
class group
{
    private:
    int grp_id;
    vector<student*> students;
    student * grpleader;
    public:
    group(int grp_id)
    {
        this->grp_id=grp_id;
    }
    void set_leader(student *std)
    {
        this->grpleader=std;
    }
    void add_student(student *std)
    {
        students.push_back(std);
    }
    int get_id()
    {
        return grp_id;
    }
    void print()
    {
        printf("Group Id: %d\n",grp_id);
        printf("Leader: ");
        grpleader->print();
        printf("Members:\n");
        for(int i=0;i<students.size();i++)
        {
            students[i]->print();
        }
    }
    void printing_end()
    {
        printf("Group %d has finished printing at time %d\n",grp_id,get_curr_time());
    }
    void binding_start()
    {
        printf("Group %d has started binding at time %d\n",grp_id,get_curr_time());
    }
    void binding_end()
    {
        printf("Group %d has finished binding at time %d\n",grp_id,get_curr_time());
    }

};

void read_file(){
    fstream myfile("input", std::ios_base::in);
    myfile >> n >> m;
    myfile >> w >> x >> y;
}

int main()
{
    read_file();
    //printf("n:%d\tm:%d\tw:%d\tx:%d\ty:%d\n",n,m,w,x,y);
    int num_of_groups=n/m;
    student * students[n];
    group * groups[num_of_groups];
    for(int i=0;i<num_of_groups;i++)
    {
        groups[i]=new group(i+1);
       // printf("Making group %d\n",i);
    }
    int grp_count=0;
    for(int i=0;i<n;i++)
    {
        if(i!=0 && i%m==0 )
        {
            students[i-1]->set_leader();
            groups[grp_count]->set_leader(students[i-1]);
           // printf("Incrementing grp_count %d\n",grp_count);
            grp_count++;
        }
       
        students[i]=new student((i+1),groups[grp_count]);
        groups[grp_count]->add_student(students[i]);
        //printf("Making student %d\n",i);
         if(i==n-1)
        {
            students[i]->set_leader();
            groups[grp_count]->set_leader(students[i]);
        }
    }
    //  for(int i=0;i<num_of_groups;i++)
    // {
    //     groups[i]->print();
    // }
    pthread_t threads[n];
    int stat;
    for(int i=0;i<n;i++)
    {
        stat=pthread_create(&threads[i],NULL,student_work,(void *)students[i]);
        if(stat)
        {
            printf("Error while creating thread\n");
            exit(-1);
        }
    }
    pthread_exit(NULL);
    return 0;
 }