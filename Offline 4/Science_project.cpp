#include<cstdio>
#include<pthread.h>
#include<semaphore.h>
#include<queue>
#include <unistd.h>
#include <random>
#include <bits/stdc++.h>
using namespace std;

int n,m,w,x,y;

default_random_engine generator;
poisson_distribution<int> distribution(9.9);
int rand_mod=5;
int rand_number;

bool printer_status[4];
pthread_mutex_t printer_locks[4];
sem_t binder_sem;
int submission_count;
int reader_count;
pthread_mutex_t reader_lock;
pthread_mutex_t read_write_lock;
int number_of_printers=4;

auto start = chrono::steady_clock::now();

int get_curr_time(){
    auto end = std::chrono::steady_clock::now();
    chrono::duration<double> elapsed_seconds = end-start;
    return elapsed_seconds.count();
}

class group;
class student
{
    public:
    int std_id;
    group *grp;
    bool is_leader=false;
    vector<group *> allgroups;

    bool print_done;
    bool waiting;
    pthread_mutex_t printed_lock;
    pthread_mutex_t waiting_lock;
    sem_t wait_sem;
    
    student(int std_id,group *g)
    {
        this->std_id=std_id;
        this->grp=g;
        sem_init(&wait_sem,0,0);
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
    void printing_arrival()
    {
        printf("Student %d has arrived at the print station at time %d\n",std_id,get_curr_time());
    }
    void printing_start()
    {
        printf("Student %d has started printing at time %d\n",std_id,get_curr_time());
    }
    void printing_end()
    {
        printf("Student %d has finished printing at time %d\n",std_id,get_curr_time());
    }
    void add_allgroups(group* grp[],int size)
    {
        for(int i=0;i<size;i++)
        {
            allgroups.push_back(grp[i]);
        }
    }
    bool release_waiting_print()
    {
        bool ret=false;

        pthread_mutex_lock(&waiting_lock);
        if(waiting)
        {
            ret=true;
            waiting=false;
            sem_post(&wait_sem);
        }
        pthread_mutex_unlock(&waiting_lock);

        return ret;

    }
   
};

class group
{
    public:
    int grp_id;
    vector<student*> students;
    student * grpleader;
    sem_t print_done;
    
    group(int grp_id)
    {
        this->grp_id=grp_id;
        sem_init(&print_done,0,0);
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
    void binding_arrival()
    {
        printf("Group %d has arrived at binding station at time %d\n",grp_id,get_curr_time());
    }
    void binding_start()
    {
        printf("Group %d has started binding at time %d\n",grp_id,get_curr_time());
    }
    void binding_end()
    {
        printf("Group %d has finished binding at time %d\n",grp_id,get_curr_time());
    }
    void submit_arrival()
    {
        printf("Group %d has arrived at entry-book station at time %d\n",grp_id,get_curr_time());
    }
    void submit_start()
    {
        printf("Group %d has started submitting at time %d\n",grp_id,get_curr_time());
    }
    void submit_end()
    {
        printf("Group %d has finished submitting at time %d\n",grp_id,get_curr_time());
    }
    sem_t * get_print_done_sem()
    {
        return &print_done;
    }
    void notify(student *std)
    {
        int printer_id = std->get_id() % number_of_printers;

        for(int i=0;i<students.size();i++)
        {
            int temp_printer_id = students[i]->get_id()%number_of_printers;
            if(printer_id==temp_printer_id && students[i]->release_waiting_print())
            {
                return;
            } 
        }

        for(int i=0;i<std->allgroups.size();i++)
        {
            if(std->allgroups[i]!=this)
            {
                for(int j=0;j<std->allgroups[i]->students.size();j++)
        {
            int temp_printer_id = std->allgroups[i]->students[j]->get_id()%number_of_printers;
            if(printer_id==temp_printer_id && std->allgroups[i]->students[j]->release_waiting_print())
            {
                return;
            } 
        }
            }
        }
    }

};

class staff
{
    public:
    int staff_id;

    staff(int id)
    {
        staff_id=id;
    }
    void read()
    {
        printf("Staff%d has started reading the entry book at time %d.No. of submission = %d\n",staff_id,get_curr_time(),submission_count);
    }
    static void accept_submission()
    {
        pthread_mutex_lock(&read_write_lock);
        submission_count++;
        pthread_mutex_unlock(&read_write_lock);
    }
};

 void * student_work(void *st)
    {
        student * std = (student *) st;
        //printf("I am inside std id %d\n",std->get_id());

        rand_number = distribution(generator)%rand_mod + 1;
        sleep(rand_number);
        //Printing station arrival
        std->printing_arrival();
        int printer_id = std->get_id()%number_of_printers;
        pthread_mutex_lock(&printer_locks[printer_id]);
        if(printer_status[printer_id])
        {
            //Printer Busy so block
            pthread_mutex_unlock(&printer_locks[printer_id]);
            pthread_mutex_lock(&std->waiting_lock);
            std->waiting=true;
            pthread_mutex_unlock(&std->waiting_lock);
            sem_wait(&std->wait_sem);
        }
        else
        {
            //Printer Free so mark it busy and proceed
            printer_status[printer_id]=true;
            pthread_mutex_unlock(&printer_locks[printer_id]);
        }
        //Printing
        std->printing_start();
        sleep(w);
        std->printing_end();
        pthread_mutex_lock(&printer_locks[printer_id]);
        printer_status[printer_id]=false;
        pthread_mutex_unlock(&printer_locks[printer_id]);
        //Printing Done.Notifying people waiting on the printer
        std->get_group()->notify(std);
        //Informing leader of work done
        sem_post(std->get_group()->get_print_done_sem()); 
        if(!std->is_leader)
        {
            pthread_exit(NULL);
        }
        //Group leader checks if all members have informed or not.If not blocks and waits.
        for(int i=0;i<std->get_group()->students.size();i++)
        {
          sem_wait(std->get_group()->get_print_done_sem());   
        }
        std->get_group()->printing_end();
        //Group Print Done.Go to binder. 
        std->get_group()->binding_arrival();
        sem_wait(&binder_sem);
        //Started Binding
        std->get_group()->binding_start();
        sleep(x);
        std->get_group()->binding_end();
        sem_post(&binder_sem);
        //Ended Binding.Start Submitting.
        std->get_group()->submit_arrival();
        staff::accept_submission();
        std->get_group()->submit_start();
        sleep(y);
        std->get_group()->submit_end();
        //All done exit
        pthread_exit(NULL);
    }
void * staff_work(void *st)
{
    staff * stf = (staff *)st;
    rand_number = distribution(generator)%rand_mod + 1;
    sleep(rand_number);
    //printf("I am inside staff %d\n",stf->staff_id);
    while(true)
    {
        //Constant Reader Update.Readers get priority.
        pthread_mutex_lock(&reader_lock);
        reader_count++;
        if(reader_count==1)
        {
            pthread_mutex_lock(&read_write_lock);
        }
        pthread_mutex_unlock(&reader_lock);

        sleep(y);
        stf->read();

        pthread_mutex_lock(&reader_lock);
        reader_count--;
        if(reader_count==0)
        {
            pthread_mutex_unlock(&read_write_lock);
        }
        pthread_mutex_unlock(&reader_lock);
        
        rand_number = distribution(generator)%rand_mod + 1;
        sleep(rand_number);

        if(submission_count==n/m)
        {
            sleep(y);
            stf->read();
            break;
        }
        }
    pthread_exit(NULL);
}

void read_file(){
    fstream myfile("input", std::ios_base::in);
    myfile >> n >> m;
    myfile >> w >> x >> y;
}
void init()
{
    for(int i=0;i<4;i++)
    {
        printer_status[i]=false;
        pthread_mutex_init(&printer_locks[i],0);
    }
    sem_init(&binder_sem,0,2);
    submission_count=0;
    reader_count=0;
    pthread_mutex_init(&reader_lock,0);
    pthread_mutex_init(&read_write_lock,0);
}
int main()
{
    read_file();
    //printf("n:%d\tm:%d\tw:%d\tx:%d\ty:%d\n",n,m,w,x,y);
    init();
    int num_of_groups=n/m;
    student * students[n];
    group * groups[num_of_groups];
    staff* staffs[2];
    for(int i=0;i<2;i++)
    {
        staffs[i]= new staff(i+1);
    }
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
    pthread_t student_threads[n];
    pthread_t staff_threads[2];
    int stat;
    for(int i=0;i<n;i++)
    {
        students[i]->add_allgroups(groups,num_of_groups);
        stat=pthread_create(&student_threads[i],NULL,student_work,(void *)students[i]);
        if(stat)
        {
            printf("Error while creating thread\n");
            exit(-1);
        }
        if( i == w/2)
        {
        stat=pthread_create(&staff_threads[0],NULL,staff_work,(void *)staffs[0]);
        if(stat)
        {
            printf("Error while creating thread\n");
            exit(-1);
        }
        }
        else if(i == w/7)
        {
            stat=pthread_create(&staff_threads[1],NULL,staff_work,(void *)staffs[1]);
        if(stat)
        {
            printf("Error while creating thread\n");
            exit(-1);
        }
        }
    }
    
    pthread_exit(NULL);
    return 0;
 }