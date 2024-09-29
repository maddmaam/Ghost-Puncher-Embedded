/**
 * @authors Madison Reilly & Sophie Waters
 * Input Module for ENCE260 Assignment
 * Designed to run on the UCFK4
 * Both gets input and interprets it. Combos etc happen here.
 * @brief Gets and Interprets Inputs
*/


#include "navswitch.h"

#include "user_input.h"

#define INPUT_FORGET_LENGTH 10 /**The Number of past Inputs to remember*/
#define INPUT_NEUTRAL 19 /**Need a way to deal with NAVSWITCH_NORTH being mapped to 0. This gets sent to recent_inputs when no action is read*/
#define NO_COMBO 255 /**Signifies that only STRAFE_LEFT, STRAFE_RIGHT, and JUMP actions have occured.*/

static User_Action_t current_action = INPUT_NEUTRAL; 
static uint8_t recent_inputs[INPUT_FORGET_LENGTH]; /**Array containing the recent inputs of size INPUT_FORGET_LENGTH*/
static uint8_t current_array_index = 0; /**The current index of the recent inputs array*/

/**
 * Polls the NavSwitch to checks for a recent action Input event
*/
void pollNavswitch(void)
{
    navswitch_update();

    // get, in the lamest way possible, the recently pushed inputs. sigh.
    if (navswitch_push_event_p(NAVSWITCH_NORTH)) {
        recent_inputs[current_array_index] = NAVSWITCH_NORTH;
    } else if (navswitch_push_event_p(NAVSWITCH_EAST)) {
        recent_inputs[current_array_index] = NAVSWITCH_EAST;
    } else if (navswitch_push_event_p(NAVSWITCH_SOUTH)) {
        recent_inputs[current_array_index] = NAVSWITCH_SOUTH;
    } else if (navswitch_push_event_p(NAVSWITCH_WEST)) {
        recent_inputs[current_array_index] = NAVSWITCH_WEST;
    } else if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
        recent_inputs[current_array_index] = NAVSWITCH_PUSH;
    } else {
        recent_inputs[current_array_index] = INPUT_NEUTRAL;
    }

    if (current_array_index + 1 == INPUT_FORGET_LENGTH) {
        current_array_index = 0;
    } else {
        current_array_index ++;
    }
}





User_Action_t getAction(void)
{
    static uint8_t combo_start = NO_COMBO;/**This is the value of current_array_index when a combo event (NAVSWITCH_PUSH) is registered. Ie: When current_array_index gets BACK to combo_start the input window closes. NULL if combo not active*/

    User_Action_t output = NEUTRAL;
    uint8_t recalled_array_index; /**Used to iterate over the array without changing current_array index for combos*/

    pollNavswitch();

    // check if a combo has occured. this is before the combo_start value is assigned but after the current_array_index increments, so only hits the falling edge :)
    if (current_array_index == combo_start && combo_start != NO_COMBO) {
        recalled_array_index = current_array_index; // +1 from "we already know what this first input is, it's NAVSWITCH_PUSH", and -1 from the ++ being after the write, they cancel.
        //led_task_function();
        uint8_t target; /**Flow control, this tells you when to stop looking through the array.*/
        if (current_array_index == 0) {
            target = INPUT_FORGET_LENGTH - 1;
        } else {
            target = current_array_index - 1;
        }

        while (recalled_array_index != target) {
            switch (recent_inputs[recalled_array_index]) {
                    case NAVSWITCH_NORTH:
                        recalled_array_index = target;
                        output = PUNCH;
                        break;
                    case NAVSWITCH_WEST:
                        recalled_array_index = target;
                        output = KICK;
                        break;
                    case NAVSWITCH_SOUTH:
                        recalled_array_index = target;
                        output = FIREBALL;
                        break;
                    default:
                        if (recalled_array_index == INPUT_FORGET_LENGTH - 1) {
                            recalled_array_index = 0;
                        } else {
                            recalled_array_index ++;
                        }
                }
        }
        // they inputted PUSH (=ATTACK) but then didn't give a dir
        if (output == NEUTRAL) {
            output = KICK;
        }
        combo_start = NO_COMBO; // reset for next loop
    }

    // if no combo has been set, then check for movement
    if (output == NEUTRAL) {
        switch (recent_inputs[current_array_index == 0 ? INPUT_FORGET_LENGTH - 1 : current_array_index - 1]) {
            case NAVSWITCH_NORTH:
                output = STRAFE_NORTH;
                break;
            case NAVSWITCH_EAST:
                output = JUMP;
                break;
            case NAVSWITCH_SOUTH:
                output = STRAFE_SOUTH;
                break;
            case NAVSWITCH_PUSH:
                combo_start = current_array_index == 0 ? INPUT_FORGET_LENGTH - 1 : current_array_index - 1;
                output = NEUTRAL;
        }
    }
    return output;
}

User_Action_t* getActionPointer(void)
{
    return &current_action;
}

void inputTaskFunction(void)
{
    current_action = getAction();
}