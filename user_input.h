/*
Madison Reilly & Sophie Waters
Input Module for ENCE260 Assignment
Header file
*/

#ifndef USER_INPUT_H
#define USER_INPUT_H

/**Integer corresponding to the Action which the User has Inputted into the Navigation Switch*/
typedef enum User_Action_e {
    NEUTRAL,
    STRAFE_SOUTH,
    STRAFE_NORTH,
    JUMP,
    NEUTRAL_ATTACK,
    PUNCH,
    KICK,
    BLOCK,
    FIREBALL,
    WALL
} User_Action_t;


/**Get the Input action from the UCFK4 Navigational Switch
 * @returns User_Action_t - the enum value corresponding to the Action
*/
User_Action_t getAction(void);


/**Creates a pointer to the current_action variable which is static to user_input.c
 * @returns A pointer to the User_Action_t current_action
*/
User_Action_t* getActionPointer(void);


/** Updates current_action. Intended to be used at the interface between user_input and the scheduling system
 *  Please bear in mind that game.c also has a pointer to current_action.
*/
void inputTaskFunction(void);

#endif // USER_INPUT_H