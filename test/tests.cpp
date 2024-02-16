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
* Load process control block tests
*/
TEST (load_process_control_blocks, NULLInputFile) 
{
	const char *input_filename = NULL;
	dyn_array_t* blocks = load_process_control_blocks(input_filename);
	EXPECT_EQ(blocks,NULL);
}

TEST (load_process_control_blocks, GoodArray) 
{
	const char *input_filename = "pcb.bin";
    size_t size = 15;
	dyn_array_t* blocks = load_process_control_blocks(input_filename);
	EXPECT_EQ(dyn_array_size(blocks), size);
}