/**
 * @authors Madison Reilly & Sophie Waters
 * The game logic file for ENCE260 Assignment
 * For a description of the game, please see the README.md :)
*/


#include <stdbool.h> // another case of shut up vscode
#include <stdint.h>

#include "tinygl.h"

#include "display_matrix.h"
#include "user_input.h"
#include "communications.h"
#include "game.h"



#define MAX_HEALTH 14 /**The Maximum health of the Player/Enemy*/
#define COLUMN_LUM_MAX (0xFFFC) /**Maximum Luminosity of Each Column (Bits need to be Flipped when used on LED Matrix)*/
#define OVERLAP_DISTANCE 1 /**The maximum distance between two sprites such that they overlap, ie: the maximum disance such that attacks land*/
#define MAX_LOGIC_TICK 7 /**How many stall ticks the sprite retains for*/
#define MAX_XPOS 5 /**Max X-Position on the UCFK4 Display*/
#define MIN_XPOS 0 /**Min X-Position on the UCFK4 Display*/

User_Action_t* current_action; /**A pointer to the current action the user has taken (Value of ENUM User_Action_t)*/
static int8_t current_x = 3; /**The Current X-Coordinate of the Player. Player Starts at X=3*/
static sprites_t current_sprite = FIGHTER_NEUTRAL;/**Current Player Sprite. Initial Value=FIGHTER_NEUTRAL_SPRITE*/
static int16_t current_health = MAX_HEALTH; /**Current Player Health. Initial Value=MAX_HEALTH*/
uint8_t enemy_x = 0; /**The Current X-Coordinate of the enemy, Value polled from the other funkit*/
uint8_t enemy_sprite = 1; // The integer value of the ENUM which corresponds to each character
bool won_game = false;
static Task_t* dominant_ptr = 0;


// More Game Logic To Come! :)

void initializeActionReference(void)
{
    current_action = getActionPointer();
}

/**
 * Checks whether the player attack move has landed a hit on the enemy player
 * @returns true if the enemy is within range of the attack
*/
bool checkHitLanded(void) {
    uint8_t difference_in_positions = 0;
    if (enemy_x > current_x) {
        difference_in_positions = enemy_x - current_x;
    }
    else {
        difference_in_positions = current_x - enemy_x;
    }
    return (difference_in_positions <= OVERLAP_DISTANCE);
}

/**
 * Moves the character North (towards the USB Port on the UCFK4)
 *  * Also checks whether the player is going off this board, and if so sends them to the other board
*/
void moveNorth(void) 
{
    if (current_x <= MIN_XPOS) {
        return;
    }
    current_x--;

}

/**
 * Move the character South (away from the USB Port on the UCFK4)
 * Also checks whether the player is going off this board, and if so sends them to the other board
*/
void moveSouth(void)
{
    if (current_x > MAX_XPOS) {
        return;
    }
    current_x++;

}


/**
 * Set the Sprite to display on this board, will change enemy sprite if flag true, otherwise will change just this boards sprite
 * @param newSprite The Enum Value corresponding to the new Sprite to Display
 * @param enemy Boolean of whether we want to change the enemy sprite or not (if not will change this boards sprite)
*/
void setCharacter(sprites_t newSprite, bool enemy)
{
    if (enemy) {
        enemy_sprite = newSprite;
    } else {
        current_sprite = newSprite;
    }
}



/**
 * Checks the current_action being performed by the user and applies the corresponding game logic.
 * Moves/Changes Sprite and/or Adjusts Enemy health
*/
void gameLogicTaskFunction(void)
{
    static uint8_t logic_tick = 0; /**This is both stun frames and sprite keepup (so we don't get single frames per attack)*/
    static bool retransmission = false; /**The alternative is character writes*/
    if (logic_tick > 0) {
        logic_tick --;
        retransmission = true;
    } else {
        retransmission = false;
    }
    setDisplayLow(); // prevent sprite bleeding. temporary gauze fix until clk is locked in
    drawHealth(~(COLUMN_LUM_MAX >> current_health)); // this should eventually be changed to be your health, but that needs ir comms :)
    switch (*current_action) {
        case NEUTRAL:
            if (!retransmission) {
            setCharacter(FIGHTER_NEUTRAL, false);
            }
            break; // maybe this is shield? who knows :3
        case STRAFE_SOUTH:
            if (!retransmission) {
            moveSouth();
            logic_tick = MAX_LOGIC_TICK;
            setCharacter(FIGHTER_NEUTRAL, false);
            }
            // Need to add check for if the character goes off the board (I'm not sure of exact values)
            break;
        case STRAFE_NORTH:
            if (!retransmission) {
                moveNorth();
                logic_tick = MAX_LOGIC_TICK;
                setCharacter(FIGHTER_NEUTRAL, false);
            }
            // Need to add check for if the character goes off the board (values?)
            break;
        case JUMP:
            if (!retransmission) {
            logic_tick = MAX_LOGIC_TICK;
            setCharacter(FIGHTER_JUMP, false);
            }
            break;
        case NEUTRAL_ATTACK:
            if (!retransmission) {
                setCharacter(FIGHTER_NEUTRAL_ATTACK, false);
                setDisplayHigh(); // this will immediately be snapped low again by the next cycle, so will appear almost like a lazer :3
                logic_tick = MAX_LOGIC_TICK;
            } else {
                sendAttack(2, current_x);
            }
            break;
        case PUNCH:
            if (!retransmission) {
                setCharacter(FIGHTER_PUNCH, false);
                logic_tick = MAX_LOGIC_TICK;
            } else {
                sendAttack(2, current_x);
            }
            break;
        case KICK:
            if (!retransmission) {
                setCharacter(FIGHTER_KICK, false);
                logic_tick = MAX_LOGIC_TICK;
            } else {
                sendAttack(2, current_x);
            }
            
            
            break;
        case BLOCK:
            break;
        case FIREBALL:
            break;
        case WALL:
            break;
    }
    
    drawSprite(current_sprite, current_x);
}

void refreshEnemySpriteDisplay(void)
{
    setCharacter(enemy_sprite, true);
}

void setEnemySprite(uint8_t new_enemy_sprite)
{
    enemy_sprite = new_enemy_sprite;
}

void setEnemyX(uint8_t new_enemy_x)
{
    enemy_x = new_enemy_x;
}


uint8_t getEnemyX(void)
{
    return enemy_x;
}


uint8_t getCurrentX(void)
{
    return current_x;
}


void setPositionX(uint8_t new_position)
{
    current_x = new_position;
}

sprites_t getCurrentSprite(void)
{
    return current_sprite;
}

void setCurrentHealth(uint8_t new_health)
{
    current_health -= new_health;
    if (current_health <= 0) {
        won_game = 0;
    }
}

void checkEndGameTaskFunction(void)
{
    if (won_game) { // recieve an external trigger from the other board saying you've won
        displayWinText((current_health > 0));
        dominant_ptr -> task_function = tinygl_update; // this avoids a double function pointer
    } else if (current_health <= 0) { // internal trigger from you saying that you've lost.
        sendLost();
        displayWinText(!(current_health <= 0));
        dominant_ptr -> task_function = tinygl_update; // this avoids a double function pointer
    }

}

void setDisplayFunctionPointer(Task_t* dominant_task)
{
    dominant_ptr = dominant_task;
}