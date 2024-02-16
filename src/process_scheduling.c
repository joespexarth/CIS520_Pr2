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

bool first_come_first_serve(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
    if(ready_queue == NULL)                                 // Return false for invalid parameters
    {
        printf("Invalid Parmeter in FCFS");
        return false;
    }
    size_t n = dyn_array_size(ready_queue);
    if(n <= 0)
    {
        printf("Invalid Array Size in FCFS");
    }  

    if(!dyn_array_sort(ready_queue, compare_arrival_times)) // Uses qsort to organize by arrival time, O(n log n) timing complexity
    {
        printf("Failed to sort array in FCFS");
        return false;
    }

    unsigned long totalRunTime = 0;                         // Declare local variables for scheduling calcualtions
    unsigned long totalWaitTime = 0;
  
    for(size_t i = 0; i < n; i++)                              // loop over the ready_queue
    {
        ProcessControlBlock_t * PCB = dyn_array_at(ready_queue, i);

        totalWaitTime += totalRunTime;                      // Keep track of the total wait time -> calulating wait time first as we are not waiting on 
                                                            // the runtime of the current PCB in-order for this PCB to start

        totalRunTime += PCB->remaining_burst_time;          // Keep track of the total run time
    }

    if(totalWaitTime <= 0 || totalRunTime <= 0)             //  Ensure logical values
    {
        printf("Failed to calulate scheduling parameters in FCFS");
        return false;
    }
    
    result->average_waiting_time = (float) totalWaitTime / n;         // Set the schedule results
    result->average_turnaround_time = (float) totalRunTime / n;  
    result->total_run_time = totalRunTime;                  

    return true;                                            // Return true for successfull schedule
}

int compare_arrival_times(const void *blockOne, const void *blockTwo){ return ((ProcessControlBlock_t *)blockOne)->arrival - ((ProcessControlBlock_t *)blockTwo)->arrival; }

bool shortest_job_first(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
    UNUSED(ready_queue);
    UNUSED(result);
    return false;   
}

bool priority(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
    UNUSED(ready_queue);
    UNUSED(result);
    return false;   
}

bool round_robin(dyn_array_t *ready_queue, ScheduleResult_t *result, size_t quantum) 
{
    UNUSED(ready_queue);
    UNUSED(result);
    UNUSED(quantum);
    return false;
}

dyn_array_t *load_process_control_blocks(const char *input_file) 
{
    dyn_array_t* blocks;
    ProcessControlBlock_t* block;
    int* numBlocks;
    int file = open(input_file, O_RDONLY);
    if(file == -1){
        return NULL;
    }

    int error = read(file, numBlocks, 1);
    if (error == -1){
        return NULL;
    }
    blocks = dyn_array_create(*numBlocks, sizeof(ProcessControlBlock_t), NULL);
    for(int i = 0; i < *numBlocks; ++i){
        error = read(file, &(block->remaining_burst_time), 1);
        if(error == -1){
            return NULL;
        }
        error = read(file, &(block->priority), 1);
        if(error == -1){
            return NULL;
        }
        error = read(file, &(block->arrival), 1);
        if(error == -1){
            return NULL;
        }

        dyn_array_push_front(blocks, block);
    }

    close(file);
    return blocks;
}

bool shortest_remaining_time_first(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
    UNUSED(ready_queue);
    UNUSED(result);
    return false;
}
