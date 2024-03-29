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
        //printf("Entered the loop");
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
    //Check for nulls
    if (ready_queue == NULL || result == NULL)  return false;

    //Get ready queue size
    size_t size = dyn_array_size(ready_queue);
    if (size == 0){
        printf("Invalid Array Size in RR");
        return false;
    }

    unsigned long totalRunTime = 0; // Total process run time
    unsigned long totalWaitTime = 0; // Total process wait time
    unsigned long totalTurnAroundTime = 0; // Total process turnaround

        //loops until processes in ready queue are executed   
        while(dyn_array_size(ready_queue) != 0)
        {
            //get first process control block
            ProcessControlBlock_t *PCB = malloc(sizeof(PCB));
            dyn_array_extract_front(ready_queue, PCB);
            //Calculate total wait time for the process if hasnt started
            if(!(PCB->started)){
                    totalWaitTime += totalRunTime;        // Keep track of the total wait time -> calulating wait time first as we are not waiting on first iteration
                    //printf("Total Waiting time = %lu\n", totalWaitTime);
                    PCB->started = true;
            }
            //quantum execution 
            for(size_t i = 0; i < quantum; i++)
            {
                virtual_cpu(PCB);
                totalRunTime++;
                if(PCB->remaining_burst_time == 0)
                    i = quantum;
            }
            //push back process to ready queue if burst time remains
            if(PCB->remaining_burst_time > 0){
                dyn_array_push_back(ready_queue, PCB);
            }
            else{
                totalTurnAroundTime += totalRunTime;      // the runtime of the current PCB in-order for this PCB to start
                //printf("Total turnaround = %lu\n", totalTurnAroundTime);
            } 
            
        }

    
    result->average_waiting_time = (float)totalWaitTime / size; // Set the schedule results
    result->average_turnaround_time = (float)totalTurnAroundTime / size;
    result->total_run_time = totalRunTime;

    return true; 
}

dyn_array_t *load_process_control_blocks(const char *input_file) 
{
    dyn_array_t* blocks; //The array of blocks.
    ProcessControlBlock_t block; //The current block being read from the file
    int numBlocks; //The number of blocks in the file
    int file = open(input_file, O_RDONLY);
    if(file == -1){ //Checks to see if the file is opened
        return NULL;
    }

    int error = read(file, &numBlocks, 4); //reads the number of blocks
    if (error == -1){ //Checks to see if the file is read properly
        return NULL;
    }
    blocks = dyn_array_create(numBlocks, sizeof(ProcessControlBlock_t), NULL); //Creates an array of blocks
    for(int i = 0; i < numBlocks; ++i){ //Loops until all blocks have been pushed into the array
        error = read(file, &(block.remaining_burst_time), 4); //reads the file into the burst time of the block
        if(error == -1){ //Checks to see if the file is read properly
            return NULL;
        }
        error = read(file, &(block.priority), 4); //reads the file into the priority of the block
        if(error == -1){ //Checks to see if the file is read properly
            return NULL;
        }
        error = read(file, &(block.arrival), 4); //reads the file into the arrival time for the block
        if(error == -1){ //Checks to see if the file is read properly
            return NULL;
        }

        dyn_array_push_front(blocks, &block); //Pushes the block into the array.
    }

    close(file); //Closes the file
    return blocks; //Returns the array of blocks
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
