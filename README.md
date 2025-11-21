MultiProcessorSim.cpp is the source code for the Multiprocessor simulator
To compile just run "g++ MultiProcessorSim.cpp" and to run use "./a.out [Average Arrival Rate, Average Service Time, CPU Count, QueueSystem, SchedulerType]"

For QueueSystem: "1" is a system where each processor has its own ready queue and "2" is a system where all processors share a single queue
For SchedulerType: "1" is a FCFS scheduler and "2" is a SJF scheduler
