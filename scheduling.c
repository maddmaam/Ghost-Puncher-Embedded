/**
 * @authors Madison Reilly & Sophie Waters
 * Scheduling Module for ENCE260 Assignment
 * Designed to run on the UCFK4
 * @brief Scheduling Module
 * 
*/

#include <avr/io.h>

#include "scheduling.h"

#define PRESCALER 256 /**TCNT1 iterates once every PRESCALER cpu ticks :)*/
#define PRESCALER_BITS 0b100 /**What we need to write to TCCR1B to achieve a prescaler of PRESCALER*/
static uint16_t loop_width; /**Number of ticks in one loop, before prescaler*/


// Because display must be called so much more frequently than every other task,
// we use a paced loop with the display task as the first task, and then occasionally
// we insert a maximum of one extra task after it.
// This system attempts to limit the chance that display will be starved, as it is sensitive to that.
// You may notice that this is a bad, bad, bad inefficient way to do tasks, with lots of extra looping
// But even though there are more instructions, they are better distributed and we actually get better performance.
// Thumbs down.


void pacerInit(uint16_t loop_rate)
{
    loop_width = loop_rate;

    TCCR1A = 0x00;
    TCCR1B = 0x00 | PRESCALER_BITS;
    TCCR1C = 0x00;
    TCNT1 = 0;
}

/**
 * Wait until the loop_width is concluded using TCNT1.
 * No sleeping no optimisation. Pure busy waiting.
*/
void pacerWait(void)
{
    while(PRESCALER * TCNT1 < loop_width) {
        continue;
    }

    TCNT1 = 0;
}


/**
 * File-local helper function for getting the submissive task for execution
 * @param submissive_tasks An array of size numtasks (so there are numtasks + 1 tasks total when you include dominant_task) where a max of task is selected to be run each loop
 * @param numtasks The number of submissive tasks to be run (should be == size(submissive_tasks))
*/
void callSingleFunction(Task_t** submissive_tasks, uint8_t numtasks)
{
    uint8_t task = 0;
    while(task < numtasks) {
        if (submissive_tasks[task] -> readiness == READY) {
            submissive_tasks[task] -> task_function();
            submissive_tasks[task] -> readiness = WAITING;
            task = numtasks; // end the loop
        }
        task++;
    }
}


/**
 * File-local helper function for getting the checking whether tasks are ready to execute
 * @param loop_count Our state trace, increments by one each time the paced loop iterates
 * @param submissive_tasks An array of size numtasks (so there are numtasks + 1 tasks total when you include dominant_task) where a max of task is selected to be run each loop
 * @param numtasks The number of submissive tasks to be run (should be == size(submissive_tasks))
*/
void updateReadinesses(uint16_t loop_count, Task_t** submissive_tasks, uint8_t numtasks)
{
    for (uint8_t task = 0; task < numtasks; task ++) { // aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa. so. many. cycles. why is this faster. 
        if ((1 + loop_count) % submissive_tasks[task] -> period == 0) { // 0 % x == 0 for all x element of N => + 1 to avoid
            submissive_tasks[task] -> readiness = READY;
        }
    }
}


void actualizeTaskOperation(Task_t* dominant_task, Task_t** submissive_tasks, uint8_t numtasks)
{
    uint16_t loop_count = 0;
    while (1) {
        pacerWait();
        dominant_task -> task_function();
        updateReadinesses(loop_count, submissive_tasks, numtasks);
        callSingleFunction(submissive_tasks, numtasks);
        loop_count ++; // ok if this overflows
    }
}