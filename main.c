/**
 * @authors Madison Reilly & Sophie Waters
 * The Main File for the ENCE260 Assignment Game, contains the int main method
 * @brief File containing int main(void)
*/


#include "system.h"
#include "ir_uart.h"

#include "display_matrix.h"
#include "user_input.h"
#include "communications.h"
#include "scheduling.h"
#include "game.h"


#define LOOP_SPEED (6400) /**Number of cpu cycles per loop*/
#define TASK_COUNT 5      /**Number of submissive tasks*/

void startup(void) {
    system_init ();

    pacerInit(LOOP_SPEED);
    pioInitializeLedmat();

    initializeActionReference();
    ir_uart_init();
}




/**
 * Main Function of the Program, calls other Modules
 * This is the function that the compiler will begin compiling from
*/
int main (void)
{
    startup();

    

    // main loop time
    Task_t display_task = {.task_function = displayTaskFunction,        .period = 0,   .readiness = READY}; // period doesn't apply because this is dominant_task
    Task_t game_logic =   {.task_function = gameLogicTaskFunction,      .period = 15,  .readiness = READY};
    Task_t input_task =   {.task_function = inputTaskFunction,          .period = 50,  .readiness = READY};
    Task_t comms_task =   {.task_function = communicationsProcessingTaskFunction, .period = 30,  .readiness = READY};
    Task_t ir_task =      {.task_function = checkInputChar,             .period = 30,  .readiness = READY};
    Task_t end_task =     {.task_function = checkEndGameTaskFunction,   .period = 80, .readiness = READY};
    setDisplayFunctionPointer(&display_task);
    Task_t* submissive_tasks[TASK_COUNT] = {&game_logic, &input_task, &comms_task, &ir_task, &end_task};
    actualizeTaskOperation(&display_task, submissive_tasks, TASK_COUNT);
    
    return 0;
}