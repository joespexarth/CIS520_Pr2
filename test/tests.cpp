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
	dyn_array_t* blocks;
    ScheduleResult_t* results;
	bool success = first_come_first_serve(NULL, results);
	EXPECT_EQ(success,false);
}


//Tests for when first come first serve should work with no errors
TEST (first_come_first_serve, CorrectInput) 
{
	dyn_array_t* blocks;
    ScheduleResult_t* results;
	bool success = first_come_first_serve(blocks, results);
	EXPECT_EQ(success,true);
    EXPECT_EQ(results->average_turnaround_time,16); //Maybe calculated wrong
    EXPECT_EQ(results->average_waiting_time,16);
    EXPECT_EQ(results->total_run_time,30);
}



/*
* Load process control block tests
*/

//Tests for a null input file
TEST (load_process_control_blocks, NULLInputFile) 
{
	const char *input_filename = NULL;
	dyn_array_t* blocks = load_process_control_blocks(input_filename);
	EXPECT_EQ(blocks,NULL);
}

//Tests for when the input file is good.
TEST (load_process_control_blocks, GoodInput) 
{
	const char *input_filename = "pcb.bin";
    size_t size = 15;
	dyn_array_t* blocks = load_process_control_blocks(input_filename);
	EXPECT_EQ(dyn_array_size(blocks), size);
}