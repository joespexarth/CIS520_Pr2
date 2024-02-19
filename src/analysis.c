#include <stdio.h>
#include <stdlib.h>

#include "dyn_array.h"
#include "processing_scheduling.h"

#define FCFS "FCFS"
#define P "P"
#define RR "RR"
#define SJF "SJF"

// Add and comment your analysis code in this function.
// THIS IS NOT FINISHED.
int main(int argc, char **argv) 
{
    if (argc < 3) 
    {
        printf("%s <pcb file> <schedule algorithm> [quantum]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if(argc == 3){
        const char *input_filename = "../pcb.bin";
        dyn_array_t* blocks = load_process_control_blocks(input_filename);
        ScheduleResult_t* results = malloc(sizeof(ScheduleResult_t));

        first_come_first_serve(blocks, results);
        printf("Average Turnaround Time: %f\n", results->average_turnaround_time);
        printf("Total Run Time: %ld\n", results->total_run_time);
        printf("Average Waiting Time: %f\n", results->average_waiting_time);
    }

    return EXIT_SUCCESS;
}
