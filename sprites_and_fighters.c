/**
 * @authors Madison Reilly & Sophie Waters
 * Asset storage for ENCE260 Assignment, wrapped by display.c and game.c to get like sprites & such on screen
 * Designed to run on the UCFK4
 * @brief Sprite Images for the LED Matrix
*/

#include "sprites_and_fighters.h"

/**Array of Bitmask Values for the LED Matrix Display to make the Fighter Neutral Position Sprite*/
const uint8_t FIGHTER_NEUTRAL_SPRITE[4] = {
    0b00001100,
    0b00011101,
    0b00101110,
    0b00101010
};

/**Array of Bitmask Values for the LED Matrix Display to make the Fighter Punch Position Sprite*/
const uint8_t FIGHTER_PUNCH_SPRITE[4] = {
    0b00110000,
    0b01110111,
    0b10111111,
    0b11110000
};

/**Array of Bitmask Values for the LED Matrix Display to make the Fighter Kick Position Sprite*/
const uint8_t FIGHTER_KICK_SPRITE[4] = {
    0b11000000,
    0b11010000,
    0b11110111,
    0b10110000
};

/**Array of Bitmask Values for the LED Matrix Display to make the Fighter Jump Position Sprite*/
const uint8_t FIGHTER_JUMP_SPRITE[4] = {
    0b01111000,
    0b11111111,
    0b10001000,
    0b00000000
};

/**Array of Bitmask Values for the LED Matrix Display to make the Fighter Neutral Attack Sprite*/
const uint8_t FIGHTER_NEUTRAL_ATTACK_SPRITE[4] = {
    0b00110011,
    0b01000111,
    0b10001000,
    0b10101000
};

/**Array of Bitmask Values for the LED Matrix Display to make the Fighter Hurt Sprite*/
const uint8_t FIGHTER_HURT_SPRITE[4] = {
    0b00011101,
    0b01011001,
    0b11001100,
    0b00110011
};

