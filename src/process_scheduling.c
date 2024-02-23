#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "dyn_array.h"
#include "processing_scheduling.h"


// You might find this handy.  I put it around unused parameters, but you should
// remove it before you submit. Just allows things to compile initially.
#define UNUSED(x) (void)(x)

// private function
void virtual_cpu(ProcessControlBlock_t *process_control_block) 
{
    // decrement the burst time of the pcb
    --process_control_block->remaining_burst_time;
}

int compare_arrival_times(const void *blockOne, const void *blockTwo){ return ((ProcessControlBlock_t *)blockOne)->arrival - ((ProcessControlBlock_t *)blockTwo)->arrival; }

bool first_come_first_serve(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
    if(ready_queue == NULL)                                 // Return false for invalid parameters
    {
        printf("Invalid Parameter in FCFS");
        return false;
    }
    size_t n = dyn_array_size(ready_queue);
    if(n <= 0)
    {
        printf("Invalid Array Size in FCFS");
        return false;
    }  

    if(!dyn_array_sort(ready_queue, compare_arrival_times)) // Uses qsort to organize by arrival time, O(n log n) timing complexity
    {
        printf("Failed to sort array in FCFS");
        return false;
    }

    unsigned long totalRunTime = 0;                         // Declare local variables for scheduling calcualtions
    unsigned long totalWaitTime = 0;
    unsigned long totalTurnAroundTime = 0;
  
    for(size_t i = 0; i < n; i++)                              // loop over the ready_queue
    {
        ProcessControlBlock_t * PCB = dyn_array_at(ready_queue, i);
        totalWaitTime += totalRunTime;                                          // Keep track of the total wait time -> calulating wait time first as we are not waiting on 
        totalTurnAroundTime += (totalRunTime + PCB->remaining_burst_time);      // the runtime of the current PCB in-order for this PCB to start
        totalRunTime += PCB->remaining_burst_time;                              // Keep track of the total run time
        while(PCB->remaining_burst_time > 0)
        {
            virtual_cpu(PCB);
        }
        printf("Entered the loop");
    }

    if(totalWaitTime <= 0 || totalRunTime <= 0 || totalTurnAroundTime <= 0)             //  Ensure logical values
    {
        printf("Failed to calulate scheduling parameters in FCFS");
        return false;
    }
    
    result->average_waiting_time = (float) totalWaitTime / n;         // Set the schedule results
    result->average_turnaround_time = (float) totalTurnAroundTime / n;  
    result->total_run_time = totalRunTime;                  

    free(ready_queue);
    return true;                                            // Return true for successfull schedule
}

int compare_burst_times(const void *blockOne, const void *blockTwo){ return ((ProcessControlBlock_t *)blockOne)->remaining_burst_time - ((ProcessControlBlock_t *)blockTwo)->remaining_burst_time; }

bool shortest_job_first(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
    if(ready_queue == NULL)                                    // Return false for invalid parameters
    {
        printf("Invalid Parameter in SJF");
        return false;
    }
    size_t n = dyn_array_size(ready_queue);
    if(n <= 0)
    {
        printf("Invalid Array Size in SJF");
        return false;
    }  

    if(!dyn_array_sort(ready_queue, compare_burst_times))      // Uses qsort to organize by burst time, O(n log n) timing complexity
    {
        printf("Failed to sort array in SJF");
        return false;
    }

    unsigned long totalRunTime = 0;                            // Declare local variables for scheduling calcualtions
    unsigned long totalWaitTime = 0;
    unsigned long totalTurnAroundTime = 0;
  
    for(size_t i = 0; i < n; i++)                              // loop over the ready_queue
    {
        ProcessControlBlock_t * PCB = dyn_array_at(ready_queue, i);
        totalWaitTime += totalRunTime;                                          // Keep track of the total wait time -> calulating wait time first as we are not waiting on 
        totalTurnAroundTime += (totalRunTime + PCB->remaining_burst_time);      // the runtime of the current PCB in-order for this PCB to start
        totalRunTime += PCB->remaining_burst_time;                              // Keep track of the total run time
        while(PCB->remaining_burst_time > 0)
        {
            virtual_cpu(PCB);
        }
    }

    if(totalWaitTime <= 0 || totalRunTime <= 0 || totalTurnAroundTime <= 0)             //  Ensure logical values
    {
        printf("Failed to calulate scheduling parameters in SJF");
        return false;
    }
    
    result->average_waiting_time = (float) totalWaitTime / n;         // Set the schedule results
    result->average_turnaround_time = (float) totalTurnAroundTime / n;  
    result->total_run_time = totalRunTime;                  

    free(ready_queue);
    return true;                                            // Return true for successfull schedule  
}
/*
int compare_priority(const void *blockOne, const void *blockTwo){ return ((ProcessControlBlock_t *)blockOne)->priority - ((ProcessControlBlock_t *)blockTwo)->priority; }

bool priority(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
    if(ready_queue == NULL)                                    // Return false for invalid parameters
    {
        printf("Invalid Parameter in Priority");
        return false;
    }
    size_t n = dyn_array_size(ready_queue);
    if(n <= 0)
    {
        printf("Invalid Array Size in Priority");
    }  

    if(!dyn_array_sort(ready_queue, compare_priority))      // Uses qsort to organize by priority, O(n log n) timing complexity
    {
        printf("Failed to sort array in Priority");
        return false;
    }

    unsigned long totalRunTime = 0;                            // Declare local variables for scheduling calcualtions
    unsigned long totalWaitTime = 0;
    unsigned long totalTurnAroundTime = 0;
  
    for(size_t i = 0; i < n; i++)                              // loop over the ready_queue
    {
        ProcessControlBlock_t * PCB = dyn_array_at(ready_queue, i);
        totalWaitTime += totalRunTime;                                          // Keep track of the total wait time -> calulating wait time first as we are not waiting on 
        totalTurnAroundTime += (totalRunTime + PCB->remaining_burst_time);      // the runtime of the current PCB in-order for this PCB to start
        totalRunTime += PCB->remaining_burst_time;                              // Keep track of the total run time
        while(PCB->remaining_burst_time > 0)
        {
            virtual_cpu(PCB);
        }
    }

    if(totalWaitTime <= 0 || totalRunTime <= 0 || totalTurnAroundTime <= 0)             //  Ensure logical values
    {
        printf("Failed to calulate scheduling parameters in Priority");
        return false;
    }
    
    result->average_waiting_time = (float) totalWaitTime / n;         // Set the schedule results
    result->average_turnaround_time = (float) totalTurnAroundTime / n;  
    result->total_run_time = totalRunTime;                  

    free(ready_queue);
    return true;                                            // Return true for successfull schedule  
}
*/
bool round_robin(dyn_array_t *ready_queue, ScheduleResult_t *result, size_t quantum) 
{
      if (ready_queue == NULL || result == NULL)  return false;

    size_t size = dyn_array_size(ready_queue);
    if (size == 0){
        printf("Invalid Array Size in RR");
        return false;
    }

    unsigned long totalRunTime = 0; // Total process run time
    unsigned long totalWaitTime = 0; // Total process wait time
    unsigned long totalTurnAroundTime = 0; // Total process turnaround

    size_t remaining_processes = size; // processes left, should be initialized to size and decremement while executing
    /*
    while (remaining_processes > 0) {
        for (size_t i = 0; i < size; ++i) {
            ProcessControlBlock_t *PCB = dyn_array_at(ready_queue, i);
            
            if (PCB->started == false) {
                PCB->started = true; // Mark process as started
                totalWaitTime += totalRunTime - PCB->arrival; // Update wait time
            }
            
           size_t exec; //execution time
           //check process and execute until finishing or for quantum
            if (PCB->remaining_burst_time < quantum) {
                exec = PCB->remaining_burst_time;
            } else {
                exec = quantum;
            }

            totalRunTime += exec; // Update total run time
            PCB->remaining_burst_time -= exec; // Update remaining burst time

            //check if process is finished
            if (PCB->remaining_burst_time == 0) { 
                totalTurnAroundTime += totalRunTime - PCB->arrival; // Uupdate turn around 
                --remaining_processes; // decrement processes left
                dyn_array_remove(ready_queue, i); //get rid of process
                --size; //  dEcrement size 
                break;
            }
        }
    */
        while(dyn_array_size(ready_queue) != 0)
        {
            ProcessControlBlock_t *PCB = dyn_array_pop_front(ready_queue);
            if(!(PCB->started)){
                    totalWaitTime += totalRunTime;                                          // Keep track of the total wait time -> calulating wait time first as we are not waiting on first iteration
                    PCB->started = true;
            }
            for(size_t i = 0; i < quantum; i++)
            {
                virtual_cpu(PCB);
                totalRunTime++;
                if(PCB->remaining_burst_time == 0)
                    i = quantum;
            }
            if(PCB->remaining_burst_time > 0){
                dyn_array_push_back(ready_queue, PCB);
            }
            else totalTurnAroundTime += totalRunTime;      // the runtime of the current PCB in-order for this PCB to start
            
        }
    }

    
    result->average_waiting_time = (float)totalWaitTime / size; // Set the schedule results
    result->average_turnaround_time = (float)totalTurnAroundTime / size;
    result->total_run_time = totalRunTime;

    return true; 
}

dyn_array_t *load_process_control_blocks(const char *input_file) 
{
    dyn_array_t* blocks;
    ProcessControlBlock_t block;
    int numBlocks;
    int file = open(input_file, O_RDONLY);
    if(file == -1){
        return NULL;
    }

    int error = read(file, &numBlocks, 4);
    if (error == -1){
        return NULL;
    }
    blocks = dyn_array_create(numBlocks, sizeof(ProcessControlBlock_t), NULL);
    for(int i = 0; i < numBlocks; ++i){
        error = read(file, &(block.remaining_burst_time), 4);
        if(error == -1){
            return NULL;
        }
        error = read(file, &(block.priority), 4);
        if(error == -1){
            return NULL;
        }
        error = read(file, &(block.arrival), 4);
        if(error == -1){
            return NULL;
        }

        dyn_array_push_front(blocks, &block);
    }

    close(file);
    return blocks;
}

bool shortest_remaining_time_first(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
    if(ready_queue == NULL)                                    // Return false for invalid parameters
    {
        printf("Invalid Parameter in SRTF");
        return false;
    }
    size_t n = dyn_array_size(ready_queue);
    if(n <= 0)
    {
        printf("Invalid Array Size in Priority");
        return false;
    }  
    
    unsigned long totalRunTime = 0;                            // Declare local variables for scheduling calcualtions
    unsigned long totalWaitTime = 0;
    unsigned long totalTurnAroundTime = 0;
  
    while(dyn_array_size(ready_queue) != 0)                   // loop until we reach the end of the array
    {
        if(!dyn_array_sort(ready_queue, compare_burst_times))                   // Uses qsort to organize by burst time, O(n log n) timing complexity
        {
            printf("Failed to sort array in SRTF");
            return false;
        }
        ProcessControlBlock_t * PCB = dyn_array_at(ready_queue, 0);

        totalWaitTime += totalRunTime;                                          // Keep track of the total wait time -> calulating wait time first as we are not waiting on first iteration
        totalTurnAroundTime += (totalRunTime + PCB->remaining_burst_time);      // the runtime of the current PCB in-order for this PCB to start
        totalRunTime += PCB->remaining_burst_time;                              // Keep track of the total run time

        while(PCB->remaining_burst_time > 0)
        {
            virtual_cpu(PCB);
        }
        if(!dyn_array_pop_front(ready_queue))                                  // Remove that item from the queue
        {
            printf("Failed to pop from queue in SRTF");
            return false;
        }
    }


    if(totalWaitTime <= 0 || totalRunTime <= 0 || totalTurnAroundTime <= 0)             //  Ensure logical values
    {
        printf("Failed to calulate scheduling parameters in SRTF");
        return false;
    }
    
    result->average_waiting_time = (float) totalWaitTime / n;         // Set the schedule results
    result->average_turnaround_time = (float) totalTurnAroundTime / n;  
    result->total_run_time = totalRunTime;                  

    free(ready_queue);
    return true;                                            // Return true for successfull schedule  
}
