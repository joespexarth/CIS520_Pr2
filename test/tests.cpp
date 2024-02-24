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
    EXPECT_EQ(results.average_turnaround_time, (float)27);
    EXPECT_EQ(results.average_waiting_time, (float)17);
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
    size_t size = 4;
	dyn_array_t* blocks = load_process_control_blocks(input_filename);
    EXPECT_NE(blocks, nullptr);
	EXPECT_EQ(dyn_array_size(blocks), size);
}

//Tests for comparing arrival times
TEST(compare_arrival_times, FirstArrivalEarlier) {
    ProcessControlBlock_t block1;
    block1.arrival = 5;
    
    ProcessControlBlock_t block2;
    block2.arrival = 10;

    int result = compare_arrival_times(&block1, &block2);
    EXPECT_LT(result, 0);
}

TEST(compare_arrival_times, SecondArrivalEarlier) {
    ProcessControlBlock_t block1;
    block1.arrival = 15;
    
    ProcessControlBlock_t block2;
    block2.arrival = 10;

    int result = compare_arrival_times(&block1, &block2);
    EXPECT_GT(result, 0);
}

//TESTS for comparing burst times
TEST(compare_burst_times, FirstBurstShorter) {
    ProcessControlBlock_t block1;
    block1.remaining_burst_time = 10;
    
    ProcessControlBlock_t block2;
    block2.remaining_burst_time = 15;

    int result = compare_burst_times(&block1, &block2);
    EXPECT_LT(result, 0);
}

TEST(compare_burst_times, SecondBurstShorter) {
    ProcessControlBlock_t block1;
    block1.remaining_burst_time = 20;
    
    ProcessControlBlock_t block2;
    block2.remaining_burst_time = 10;

    int result = compare_burst_times(&block1, &block2);
    EXPECT_GT(result, 0);
}

//1 Process in ready queue
TEST(shortest_job_first, SingleProcess) {
    dyn_array_t* blocks = dyn_array_create(1, sizeof(ProcessControlBlock_t), NULL);
    ProcessControlBlock_t block;
    block.arrival = 0;
    block.priority = 3;
    block.remaining_burst_time = 24;
    block.started = false;

   dyn_array_push_front(blocks, &block);

    ScheduleResult_t results;
    results.average_waiting_time = 0;
    results.average_turnaround_time = 0;
    results.total_run_time = 0;

    bool success = shortest_job_first(blocks, &results);
    EXPECT_TRUE(success);
    EXPECT_EQ(results.average_turnaround_time, (float)24);
    EXPECT_EQ(results.average_waiting_time, (float)0);
    EXPECT_EQ(results.total_run_time, (float)24);

}

//Shortest job test with multiple processes
TEST(shortest_job_first, MultipleProcess) {
    dyn_array_t* blocks = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
    ProcessControlBlock_t block1, block2, block3, block4;
    block1.remaining_burst_time = 6;
    block2.remaining_burst_time = 8;
    block3.remaining_burst_time = 7;
    block4.remaining_burst_time = 3;
    dyn_array_push_front(blocks, &block1);
    dyn_array_push_front(blocks, &block2);
    dyn_array_push_front(blocks, &block3);
    dyn_array_push_front(blocks, &block4);

    ScheduleResult_t results;
    results.average_waiting_time = 0;
    results.average_turnaround_time = 0;
    results.total_run_time = 0;

    bool success = shortest_job_first(blocks, &results);
    EXPECT_TRUE(success);
    EXPECT_EQ(results.average_turnaround_time, (float)13);
    EXPECT_EQ(results.average_waiting_time, (float)7);
    EXPECT_EQ(results.total_run_time, (float)24);

}


//Tests for when round robin should work with multiple processes
TEST (round_robin, CorrectInput) 
{
	dyn_array_t* blocks = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);

    ProcessControlBlock_t block1;
    ProcessControlBlock_t block2;
    ProcessControlBlock_t block3;

    block1.arrival = 24;
    block2.arrival = 3;
    block3.arrival = 3;

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
    EXPECT_EQ(results.average_turnaround_time, (float)47.0/3.0);
    EXPECT_EQ(results.average_waiting_time, (float)11.0/3.0);
    EXPECT_EQ(results.total_run_time, (float)30);
}

//Tests for when a parameter is wrong
TEST (round_robin, IncorrectParameters) 
{
    ScheduleResult_t results;
    results.average_waiting_time = 0;
    results.average_turnaround_time = 0;
    results.total_run_time = 0;

	bool success = round_robin(nullptr, &results);
	EXPECT_EQ(success,false);
}
