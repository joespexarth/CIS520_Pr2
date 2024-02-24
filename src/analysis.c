#include <stdio.h>
#include <stdlib.h>

#include "dyn_array.h"
#include "processing_scheduling.h"

#define FCFS "FCFS"
#define P "P"
#define RR "RR"
#define SJF "SJF"
#define SRTF "SRTF"

// Add and comment your analysis code in this function.
// THIS IS NOT FINISHED.
int main(int argc, char **argv) 
{
    int quantum = 0;
    if (argc < 3) 
    {
        printf("%s <pcb file> <schedule algorithm> [quantum]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if(argc == 3 || argc == 4){
        const char *input_filename = argv[1];
        dyn_array_t* blocks = load_process_control_blocks(input_filename);
        ScheduleResult_t* results = malloc(sizeof(ScheduleResult_t));
        if(argv[2] == FCFS){
            first_come_first_serve(blocks, results);
        }
        else if (argv[2] == SRTF){
            shortest_remaining_time_first(blocks, results);
        }
        else if (argv[2] == RR){
            round_robin(blocks, results, sscanf(argv[3], "%d", quantum));
        }
        else if (argv[2] == SJF){
            shortest_job_first(blocks, results);
        }
        printf("Average Turnaround Time: %f\n", results->average_turnaround_time);
        printf("Total Run Time: %ld\n", results->total_run_time);
        printf("Average Waiting Time: %f\n", results->average_waiting_time);
    }

    return EXIT_SUCCESS;
}
