
#define ARR 0
#define DEP 1
#include <algorithm>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <cmath>
float avgArrivalRate=0.f;
float avgServiceTime=0.f;
float simClock=0.f;
float totalTurnAroundTime=0.f;
class process{
  public:
    static int idCount;
    int id;
    float serviceTime;
    float arrivalTime;
    process* next;
    int cpu;
  process(){
    id = idCount++;
    serviceTime = 0;
    arrivalTime = 0;
    next=nullptr;
    cpu=0;
    }
  ~process() {
    delete next;
  }
  void setCpu(int c) {
    cpu=c;
  }
  void setArrivalTime(float simTime){
    float randomNumber;
    do{
      randomNumber=(float)rand()/(RAND_MAX);
      }while(randomNumber==1.f || randomNumber==0.f);
    arrivalTime=simTime+(-1.0/avgArrivalRate)*std::log(1-(randomNumber));;
  }
  void setServiceTime(float simTime){
    float randomNumber;
    do{
      randomNumber=(float)rand()/(RAND_MAX);
    }while(randomNumber==1.f || randomNumber==0.f);
    serviceTime=(-1.f/(1.f/avgServiceTime))*std::log(1.f-(randomNumber));
  }
};
int process::idCount=0;
class event{
  public:
    int type;
    process* p;
    event* next;
    float time;
    event() {
      type=0;
      p=nullptr;
      next=nullptr;
      time=0.f;
    }
    event(int eventType, process* proc){
      type=eventType;
      p=proc;
      if(type==ARR){
        time=p->arrivalTime;
      }
      else if(type==DEP){
        time=(p->serviceTime)+simClock;
      }
      next=nullptr;
      }
};
class eventQueue{
  public:
    eventQueue() {
      head=nullptr;
    }
    void enqueue(event* e){
      if(head==nullptr){
        head=e;
      }
      else{
        if (e->time<head->time) {
          e->next=head;
          head=e;
        }
        else {
          bool inserted=false;
          event *curr=head->next;
          event *prev=head;
          while(curr!=nullptr){
            if(e->time<curr->time){
              prev->next=e;
              e->next=curr;
              inserted=true;
              break;
            }
            prev=curr;
            curr=curr->next;
          }
          if (!inserted) {
            prev->next=e;
          }
        }
      }
    }
  event* dequeue() {
      if(head==nullptr) {
        return nullptr;
      }
      event* e=head;
      head=head->next;
      return e;
    }
private:
  event* head;

};
class readyQueue{
  public:
    readyQueue() {
      head=nullptr;
      size=0;
    }
    ~readyQueue() {
      delete head;
    }
    void enqueue(process* proc){
      size++;
      if (head==nullptr)
        head=proc;
      else {
        process *i=head;
        while (i->next!=nullptr) {
          i=i->next;
        }
        i->next=proc;
      }
    }
  process* dequeue() {
      if (head==nullptr) {
        return nullptr;
      }
      size--;
      process* i=head;
      head=head->next;
      if (size=0) {
        head=nullptr;
      }
      return i;
    }
  bool isEmpty() {
      return(head==nullptr);
    }
  int getSize() {
      return size;
    }
  private:
    process* head;
    int size;
    };

void scheduleEvent(int type, float &time, process* proc, eventQueue &queue) {
  if (proc!=nullptr) {
    event* e;
    if (type==ARR) {
      proc->setArrivalTime(time);
      proc->setServiceTime(time);
    }
    e=new event(type, proc);
    queue.enqueue(e);
  }
}
void arrivalHandler(int cpu, bool *cpuIdle, event* arrival, eventQueue &eQueue, readyQueue &rQueue) {
  scheduleEvent(ARR,simClock,new process,eQueue);
  if (*cpuIdle) {
    *cpuIdle=false;
    arrival->p->setCpu(cpu);

    scheduleEvent(DEP, simClock, arrival->p, eQueue);
  }
  else {
    rQueue.enqueue(arrival->p);
  }
}
void departureHandler(int cpu,bool *cpuIdle, event* departure, eventQueue &eQueue, readyQueue &rQueue) {
  totalTurnAroundTime=totalTurnAroundTime+(departure->time-departure->p->arrivalTime);
  if (rQueue.isEmpty()) {
    *cpuIdle=true;
  }
  else {
    process* p=rQueue.dequeue();
    p->setCpu(cpu);
    scheduleEvent(DEP, simClock, p, eQueue);
  }
}
void init(eventQueue &eq) {
  simClock=0.f;
  srand(time(0));
  scheduleEvent(ARR,simClock,new process,eq);
}
int getIdleCPU(bool cpuArr[], int size) {
  for (int i=0; i<size; i++) {
    if (cpuArr[i]==true) {
      return i;
    }
  }
  return(0);
}
int findRandReadyQueue(int size) {
  float chance = 1.f/size;
  float randomNumber;
  do{
    randomNumber=(float)rand()/(RAND_MAX);
  }while(randomNumber==1.f || randomNumber==0.f);
  return((int)((randomNumber)/chance));
}
int main(int argc, char** argv) {
  int queueSystem=2;
  int compProcesses=0;
  float prevTime=0.f;
  eventQueue eq= eventQueue();
  avgArrivalRate=atof(argv[1]);
  avgServiceTime=atof(argv[2]);
  queueSystem=atoi(argv[3]);
  simClock=0.f;
  int cpuCount=atoi(argv[4]);
  bool cpuStatus[cpuCount];
  float cpuBusyTime[cpuCount];

  init(eq);
  for (int i=0; i<cpuCount;i++) {
    cpuStatus[i]=true;
    cpuBusyTime[i]=0.f;
  }
  if (queueSystem==1) {
    readyQueue rQueues[cpuCount];
    float rQueueSizes[cpuCount];
    for (int i=0; i<cpuCount; i++) {
      rQueues[i]=readyQueue();
      rQueueSizes[i]=0.f;
    }
    event* currEvent;
    int cpu;
    while (compProcesses<10000) {
      cpu=findRandReadyQueue(cpuCount);
      currEvent=eq.dequeue();
      prevTime=simClock;
      simClock=currEvent->time;
      for (int i=0; i<cpuCount; i++) {
        if (cpuStatus[i]==false) {
          cpuBusyTime[i]=cpuBusyTime[i]+(simClock-prevTime);
          rQueueSizes[i]=rQueueSizes[i]+(static_cast<float>(rQueues[i].getSize())*(simClock-prevTime));
        }
      }
      if (currEvent!=nullptr) {
        if (currEvent->type==ARR) {
          arrivalHandler(cpu, &cpuStatus[cpu],currEvent,eq,rQueues[cpu]);
        }
        else if (currEvent->type==DEP) {
          departureHandler(currEvent->p->cpu,&cpuStatus[currEvent->p->cpu],currEvent,eq,rQueues[currEvent->p->cpu]);
          compProcesses=compProcesses+1;
        }
        delete currEvent;
      }

    }
    printf("Comp Processes: %d Total Sim Time: %f AverageTurnaroundTime: %f Throughput: %f \n",compProcesses,simClock, totalTurnAroundTime/compProcesses, compProcesses/simClock);
    for (int i=0; i<cpuCount; i++) {
      printf("Cpu %d Busy Time:%f Utilization: %f Average Processes in Ready Queue: %f \n",i,cpuBusyTime[i],(cpuBusyTime[i]/simClock),(rQueueSizes[i]/simClock));
    }
    printf("Average Arrival Rate, QueueSystem, Average TurnAroundTime, Throughput, ");
    for (int i=0; i<cpuCount; i++) {
      printf("CPU #%d Utilization, ",i);
    }
    for (int i=0; i<cpuCount; i++) {
      printf("RQ #%d Average Processes in Ready Queue, ",i);
    }
    printf("\n");
    printf("%f, %d, %f, %f",avgArrivalRate, queueSystem,totalTurnAroundTime/static_cast<float>(compProcesses), static_cast<float>(compProcesses)/simClock);
    for (int i=0; i<cpuCount; i++) {
      printf(", %f",(cpuBusyTime[i]/simClock));
    }
    for (int i=0; i<cpuCount; i++) {
      printf(", %f",rQueueSizes[i]/simClock);
    }
  }
  else if (queueSystem==2) {
    readyQueue rQueue;
    event* currEvent;
    int cpu;
    float rQueueSize=0.f;

    while (compProcesses<10000) {
      currEvent=eq.dequeue();
      prevTime=simClock;
      simClock=currEvent->time;
      for (int i=0; i<cpuCount; i++) {
        if (cpuStatus[i]==false) {
          cpuBusyTime[i]=cpuBusyTime[i]+(simClock-prevTime);
        }
      }
      rQueueSize=rQueueSize+(static_cast<float>(rQueue.getSize())*(simClock-prevTime));
      if (currEvent!=nullptr) {
        if (currEvent->type==ARR) {
          cpu=getIdleCPU(cpuStatus, cpuCount);
          arrivalHandler(cpu,&cpuStatus[cpu],currEvent,eq,rQueue);
        }
        else if (currEvent->type==DEP) {
          departureHandler(currEvent->p->cpu,&cpuStatus[currEvent->p->cpu],currEvent,eq,rQueue);
          compProcesses=compProcesses+1;
        }
      }
    }
    printf("Comp Processes: %d Total Sim Time: %f AverageTurnaroundTime: %f Throughput: %f \n",compProcesses,simClock, totalTurnAroundTime/static_cast<float>(compProcesses), static_cast<float>(compProcesses)/simClock);
    for (int i=0; i<cpuCount; i++) {
      printf("Cpu %d Busy Time:%f Utilization: %f Average People in Ready Queue: %f \n",i,cpuBusyTime[i],(cpuBusyTime[i]/simClock),rQueueSize/simClock);
    }
    printf("Average Arrival Rate, QueueSystem, Average TurnAroundTime, Throughput, ");
    for (int i=0; i<cpuCount; i++) {
      printf("CPU #%d Utilization, ",i);
    }
    printf("Average Processes in Ready Queue\n");
    printf("%f, %d, %f, %f", avgArrivalRate,queueSystem,totalTurnAroundTime/static_cast<float>(compProcesses), static_cast<float>(compProcesses)/simClock);
    for (int i=0; i<cpuCount; i++) {
      printf(", %f",(cpuBusyTime[i]/simClock));
    }
    printf(", %f \n",rQueueSize/simClock);

  }
}