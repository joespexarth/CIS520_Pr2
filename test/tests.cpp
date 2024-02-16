#include <fcntl.h>
#include <stdio.h>
#include "gtest/gtest.h"
#include <pthread.h>
#include "../include/processing_scheduling.h"

// Using a C library requires extern "C" to prevent function managling
extern "C" 
{
#include <dyn_array.h>
}


#define NUM_PCB 30
#define QUANTUM 5 // Used for Robin Round for process as the run time limit

unsigned int score;
unsigned int total;

class GradeEnvironment : public testing::Environment 
{
    public:
        virtual void SetUp() 
        {
            score = 0;
            total = 210;
        }

        virtual void TearDown()
        {
            ::testing::Test::RecordProperty("points_given", score);
            ::testing::Test::RecordProperty("points_total", total);
            std::cout << "SCORE: " << score << '/' << total << std::endl;
        }
};


int main(int argc, char **argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new GradeEnvironment);
    return RUN_ALL_TESTS();
}

/*
* First come first serve tests
*/

//Tests for when a parameter is wrong
TEST (first_come_first_serve, IncorrectParameters) 
{
    ScheduleResult_t results;
    results.average_waiting_time = 0;
    results.average_turnaround_time = 0;
    results.total_run_time = 0;

	bool success = first_come_first_serve(nullptr, &results);
	EXPECT_EQ(success,false);
}


//Tests for when first come first serve should work with no errors
TEST (first_come_first_serve, CorrectInput) 
{
	dyn_array_t* blocks = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);

    ProcessControlBlock_t block1;
    ProcessControlBlock_t block2;
    ProcessControlBlock_t block3;

    block1.arrival = 0;
    block2.arrival = 1;
    block3.arrival = 2;

    block1.priority = 3;
    block2.priority = 1;
    block3.priority = 2;

    block1.remaining_burst_time = 24;
    block2.remaining_burst_time = 3;
    block3.remaining_burst_time = 3;

    block1.started = false;
    block2.started = false;
    block3.started = false;
    
    dyn_array_push_front(blocks, &block1);
    dyn_array_push_front(blocks, &block2);
    dyn_array_push_front(blocks, &block3);

    ScheduleResult_t results;
    results.average_waiting_time = 0;
    results.average_turnaround_time = 0;
    results.total_run_time = 0;

	bool success = first_come_first_serve(blocks, &results);
	EXPECT_EQ(success,true);
    EXPECT_EQ(results.average_turnaround_time, (float)16); //Maybe calculated wrong
    EXPECT_EQ(results.average_waiting_time, (float)16);
    EXPECT_EQ(results.total_run_time, (float)30);
}


/*
* Load process control block tests
*/
TEST (load_process_control_blocks, NULLInputFile) 
{
	const char *input_filename = NULL;
	dyn_array_t* blocks = load_process_control_blocks(input_filename);
	EXPECT_EQ(blocks,nullptr);
}

TEST (load_process_control_blocks, GoodArray) 
{
	const char *input_filename = "../pcb.bin";
    size_t size = 32516;
	dyn_array_t* blocks = load_process_control_blocks(input_filename);
    EXPECT_NE(blocks, nullptr);
	EXPECT_EQ(dyn_array_size(blocks), size);
}