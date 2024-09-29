/**
 * @file communications.h
 * @authors Madison Reilly & Sophie Waters
 * Communications file for the ENCE260 Assignment
 * Contains functions which are used to compare input from and send output to the IR LED
 * @brief IR LED Communications
*/

#ifndef COMMUNICATIONS_C
#define COMMUNICATIONS_C

#include <stdint.h>
#include "sprites_and_fighters.h"

/**
 * Tells the reciever how to interpret the packet based on the header (1st 2 bits) 
*/
typedef enum {
    ENDGAME = 0,
    HEALTH,
    ACKNOWLEDGE,
    NACKNOWLEDGE,
} PacketHeader_t;



/**
 * Send the enemy board an attack that has been done by this player
 * @param damage The damage inflicted to the enemy character. Positive to deduct health.
 * @param player_x The current position of our friend on the sending board. 1-7
*/
void sendAttack(uint8_t damage, uint8_t player_x);

/**
 * Send the enemy board this Characters new X-Position on that board and the sprite to display on the board
 * @param xPos New X-Position of this boards character on the enemy board
 * @param charToDisplay This boards current character stance which will be displayed on the enemies board
*/
void sendCharacterMoveSprite(int8_t xPos, sprites_t charToDisplay);

/**
 * Checks if there is an IR communication ready to be received, if so, processes the correct logic
 * This should be a regular polling function in the Task Scheduler/Pacer Loop
*/
void checkInputChar(void);

/**
 * Intended to exist as part of the interface between
 * the task scheduling, game, and communications modules.
 * Task function - executes the communications task.
 * 
*/
void communicationsProcessingTaskFunction(void);

/**
 * Send an Acknowledgment to the other UCFK to indicate correct receipt of the last packet sent to this funkit.
*/
void sendACK(void);

/**
 * Tell the other UCFK that it has won the game
*/
void sendLost(void);

#endif