/**
 * @authors Madison Reilly & Sophie Waters
 * Scheduling Module for ENCE260 Assignment 
 * Header File
 * @brief Scheduling Module Header File
*/

#ifndef SCHEDULING_H
#define SCHEDULING_H


/**
 * Enum to Show the Current State of the Task (Is it Ready to be run, Waiting to Run or is it Blocked)
*/
typedef enum Task_Readiness_e {
    READY,
    WAITING
} Task_Readiness_t;


/**Generic task type, really being only an association of a call frequency with a function.
 * task_function is the function callback associated with the instance
 * period is the number of paced loop cycles to wait between calls to this task
 * readiness is used in scheduling.c to allow for task buffering (thus preventing multiple interstital tasks between dominant_task calls)
*/
typedef struct {
    void (* task_function)(void);
    uint16_t period;
    Task_Readiness_t readiness;
} Task_t;

/**Directly write to TCCR control registers to initialise the paced loop.
 * @param loop_rate The number of cpu cycles that should occur (approx) during one loop cycle.
*/
void pacerInit(uint16_t loop_width);


/**
 * Execute the Specified Tasks on the ATMega32U2 Microcontroller
 * @param dominant_tasks The task (display) to run every loop. Ideally needs to be significantly more frequent than any of the submissive tasks
 * @param submissive_tasks An array of size numtasks (so there are numtasks + 1 tasks total) where a max of task is selected to be run each loop
 * @param numtasks The number of submissive tasks to be run (should be ==size(submissive_tasks))
*/
void actualizeTaskOperation(Task_t* dominant_task, Task_t** submissive_tasks, uint8_t numtasks);

#endif // SCHEDULING_H