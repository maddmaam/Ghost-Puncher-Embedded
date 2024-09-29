/**
 * @authors Madison Reilly & Sophie Waters
 * Game logic for ENCE260 assignment
 * Header File
 * @brief High level game functions for easy conversion to tasks
*/

#ifndef GAME_H
#define GAME_H

#include "sprites_and_fighters.h"
#include "scheduling.h"

/**Boolean of whether the player has won the game*/
bool won_game;

/**
 * Perform game logic on *current_action, reading
 * from user_input and writing to display in the process.
 * Intended to exist as part of the interface between
 * the task scheduling and game modules.
*/
void gameLogicTaskFunction(void);


/**
 * Wrapper for setCharacter, allows communications module to perform a refresh of the
 * enemy's sprite without externalising the enemy_sprite variable
*/
void refreshEnemySpriteDisplay(void);

/**
 * Wrapper to allow communications module to write enemy sprite without externalising
 * @param new_enemy_sprite The new value of the enemy_sprite, as gotten from an ir_getc call
*/
void setEnemySprite(uint8_t new_enemy_sprite);

/**
 * Wrapper to allow communications module to write enemy x without externalising
 * @param new_enemy_x The new value of the enemy_x, as gotten from an ir_getc call
*/
void setEnemyX(uint8_t new_enemy_x);


/**
 * This function tells the *current_action pointer inside game.c to set
 * itself to refer to the static current_action value inside user_input.c
 * A wrapper of a wrapper.
*/
void initializeActionReference(void);

/**
 * Get the current X Position of the enemy sprite relative to this board
 * @returns Relative X-Position of the enemy sprite on this board
*/
uint8_t getEnemyX(void);

/**
 * Get the Current X Position of the sprite on this board
 * @returns Current X position of the sprite
*/
uint8_t getCurrentX(void);

/**
 * Set the current X Position of the sprite on this board
 * @param new_position New X Position for the sprite on this board
*/
void setPositionX(uint8_t new_position);

/**
 * Get the current Sprite of this boards player
 * @returns Enum value corresponding to the sprite being displayed
*/
sprites_t getCurrentSprite(void);

/**
 * Set the Current health of the player to current_health - new_health
 * @param new_health The amount to deduct off this players health
*/
void setCurrentHealth(uint8_t new_health);

bool checkHitLanded(void);

/**
 * Queries the game.c file to see if the game is still going periodically.
 * If not, displays either a win or a loss on screen via tinygl.
*/
void checkEndGameTaskFunction(void);


/**
 * Allows the endgame to overwrite the task function
 * of the dominant task by transferring a pointer to it
 * (after initialisation) from main.c to game.c.
 * @param task_function Takes a pointer the dominant task
*/
void setDisplayFunctionPointer(Task_t* dominant_task);

#endif // GAME_H