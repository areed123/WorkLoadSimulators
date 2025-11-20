#define ARR 0
#define DEP 1
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <cmath>
#include <stdio.h>
#include <queue>
//
// Created by aiden on 10/22/2025.
//

struct process;
struct event;
struct readyQueue;
struct procNode;
bool cpuIdle=true;
int Schedule;
int idCount=0;
float simClock=0;
float oldClock=0;
float timeBusy=0;
float randomNumber;
float avgArrivalRate=0;
float avgServiceTime=0;
float totalTurnAroundTime=0;
float peopleInQueue=0;
int readyQueueCount=0;
int total;
int scheduler;
event* head;
void init();
void schedule(int type, process* proc);
// process structure: A process is represented by an id, an arrival time and service time
struct process{
  int id;
  float arrivalTime;
  float serviceTime;
  process(){
    randomNumber=(float)rand()/RAND_MAX;
    while (randomNumber==1||randomNumber==0) {
      randomNumber=(float)rand()/RAND_MAX;
    }
    id=idCount++;
    arrivalTime=simClock+(-1.0/avgArrivalRate)*std::log(1-(randomNumber));
    randomNumber=(float)rand()/RAND_MAX;
    while (randomNumber==1||randomNumber==0) {
      randomNumber=(float)rand()/RAND_MAX;
    }

    serviceTime=(-1.0/(1/avgServiceTime))*std::log(1-(randomNumber));
    serviceTime=0;
  }
  void genServiceTime() {
    randomNumber=(float)rand()/RAND_MAX;
    while (randomNumber==1||randomNumber==0) {
      randomNumber=(float)rand()/RAND_MAX;
    }
    serviceTime=(-1.0/(1/avgServiceTime))*std::log(1-(randomNumber));
  }

};
//procNode: Node in the process linked list used to implement a readyqueue
struct procNode {
  process* proc;
  procNode* next;
  procNode() {
    proc=nullptr;
    next=nullptr;
  }
  procNode(process* p) {
    proc=p;
    next=nullptr;
  }
  ~procNode() {
    delete proc;
  }
};

//readyQueue: contains the head of a process linked list and all necessary functions to implement a readyQueue
struct readyQueue {
  procNode* rqhead;
  int size;
  readyQueue() {
    rqhead=nullptr;
    size=0;
  }
  //push(process* p) adds a process to the readyQueue and checks the global Schedule to determine how to add to the queue
  void push(process* p) {
    procNode* newProc=new procNode(p);
    if (rqhead==nullptr) {
      rqhead=newProc;
    }

    else {
      procNode* i=rqhead;
      if (Schedule==0) {

        while (i->next!=nullptr) {
          i=i->next;
        }
        i->next=newProc;
      }
      else if (Schedule==1) {
        //procNode* temp=rqhead;
        if (rqhead->proc->serviceTime > p->serviceTime) {
          newProc->next=rqhead;
          rqhead=newProc;
          size++;
          return;
        }
        procNode* temp;
        while (i->next!=nullptr) {
          temp=i;
          i=i->next;
          if (i->proc->serviceTime > p->serviceTime) {
            newProc->next=i;
            temp->next=newProc;
            size++;
            return;
          }
        }
        i->next=newProc;
        size++;
        return;



      }
    }
    size++;
  }
  process* front() {
    if (size==0) {
      return nullptr;
    }
    return(rqhead->proc);
  }
  void pop() {
    if (rqhead==nullptr) {return;}
    //delete rqhead->proc;
    rqhead=rqhead->next;
    //delete temp;
    size--;
  }
  ~readyQueue() {
    delete rqhead;
  }
};

//event struct: represents either an arrival or departure and contains a pointer to an associated process
//events of type Departure have their time set to the simClock + p->serviceTime if its an arrival its time is set to p->arrivalTime
//points to the next event in the queue

struct event{
  int type;
  float time;
  event* next;
  process* p;
  event() {
    type=-1;
    time=-1;
    next=nullptr;
    p=nullptr;
  }
  event(int eventType, process* proc){
    type = eventType;
    p=proc;
    next=nullptr;
    if(type==ARR){
      time=p->arrivalTime;
    }
    else if(type==DEP){
      time=p->serviceTime+simClock;
    }

  }

};
//init(): ensures all essential global variables are instantiated and the eventQueue
void init(){
  simClock=0;
  totalTurnAroundTime=0;
  srand(time(0));
  head=nullptr;
  schedule(ARR, (new process()));
  cpuIdle=true;
  }

/* schedule(int type, process* proc) adds another event to the event queue given a type ARR or DEP and a process proc
 */
void schedule(int type, process* proc){
        event * e = new event(type,proc);
        event* temp=head;
        if(head==nullptr){
          head=e;
          return;}
        if (head->time>e->time) {
          e->next=head;
          head=e;
        }
        event* i=head;
        while(i->next!=nullptr) {
          temp=i;
          i=i->next;
          if (i->time>e->time) {
            temp->next=e;
            e->next=i;
            return;
          }
        }
        i->next=e;
        return;
        if(head->next==nullptr){
          if(e->time>head->time)
            head->next=e;
          else{
            e->next=head;
            head=e;
           }
          return;
        }
        else{
          event* i=head;
          if (head->time>e->time) {
            e->next=head;
            head=e;
            return;
          }
          while(i->time<e->time){
            temp=i;
            i=i->next;
            if(i==nullptr){
              e->next=i;
              temp->next=e;
              return;
            }
          }
          temp->next=e;
          e->next=i;
        }
      return;
     }

void arrivalHandler(event* arrival, readyQueue& processes){

      if(cpuIdle==true){
        cpuIdle=false;
        arrival->p->genServiceTime();
        schedule(DEP, (arrival->p));
        }
      else{
          timeBusy=timeBusy+(simClock-oldClock);
          processes.push((arrival->p));
      }
  schedule(ARR,new process());
}
void departureHandler(event* departure, readyQueue& processes){
  if ((departure->time-departure->p->arrivalTime)<=0) {
  }
  timeBusy=timeBusy+(simClock-oldClock);
  if(processes.size>0){
    processes.front()->genServiceTime();
    schedule(DEP,processes.front());
    processes.pop();
  }
  else
  {
    cpuIdle=true;
  }
}
event* getNextEvent(){
  event* gotEvent=head;
  head=head->next;
  return gotEvent;

}
int run(int compCount, readyQueue& rq){
  init();
  rq.rqhead=nullptr;
  float theTime;
  event* gotEvent;
  int completed=0;
  int periodCount=0;
  while(completed<compCount){
    gotEvent=getNextEvent();
    oldClock=simClock;
    simClock=gotEvent->time;
    theTime=simClock;
    peopleInQueue=peopleInQueue+(static_cast<float>(rq.size))*(simClock-oldClock);


        switch(gotEvent->type){
          case ARR:
            arrivalHandler(gotEvent,rq);
            break;
          case DEP:
              totalTurnAroundTime=totalTurnAroundTime+(gotEvent->time-gotEvent->p->arrivalTime);
            departureHandler(gotEvent,rq);
            completed++;
            break;
        }
    }


  std::cout<<"Completed Events: "<<completed<<std::endl;
  std::cout<<"End Clock Time: "<<simClock<<std::endl;
  std::cout<<"Total Time Busy: "<<timeBusy<<std::endl;
  std::cout<<"Utilization: "<<timeBusy/simClock<<std::endl;
  std::cout<<"Average Turnaround Time: "<<totalTurnAroundTime/completed<<std::endl;
  std::cout<<"Average Processes in Ready Queue: "<<peopleInQueue/simClock<<std::endl;
  std::cout<<"Total Throughput: "<<completed/simClock<<" processes per second"<<std::endl;
  return 0;

}

int main(int argc, char** argv) {
    std::cout<<"Chosen arrival rate: "<<atof(argv[1])<<" Chosen service time: "<<atof(argv[2])<<std::endl;
    avgArrivalRate=atof(argv[1]);
    avgServiceTime=atof(argv[2]);
    Schedule=atoi(argv[3]);
    readyQueue processes;
    run(10000,processes);
    std::cout<<"Finished"<<std::endl;

    return 0;
}