/**
 * @authors Madison Reilly & Sophie Waters
 * Asset storage for ENCE260 Assignment, wrapped by display.c and game.c to get like sprites & such on screen
 * Header File
 * @brief Sprite Images for the LED Matrix
*/

#ifndef SPRITES_AND_FIGHTERS_H
#define SPRITES_AND_FIGHTERS_H

#include <stdint.h> // this is only here so that vs code stops yelling. it's not actually necessary at compile-time

const uint8_t FIGHTER_NEUTRAL_SPRITE[4];
const uint8_t FIGHTER_PUNCH_SPRITE[4];
const uint8_t FIGHTER_KICK_SPRITE[4];
const uint8_t FIGHTER_JUMP_SPRITE[4];
const uint8_t FIGHTER_NEUTRAL_ATTACK_SPRITE[4];
const uint8_t FIGHTER_HURT_SPRITE[4];

/**
 * Enum Representing all the Possible Sprites, each value corresponds to 
*/
typedef enum sprites_e {
    FIGHTER_NEUTRAL, /**Fighter Neutral Stance Sprite*/
    FIGHTER_PUNCH, /**Fighter Punch Stance Sprite*/
    FIGHTER_KICK, /**Fighter Kicking Stance Sprite*/
    FIGHTER_JUMP, /**Fighter Jumping Sprite*/
    FIGHTER_NEUTRAL_ATTACK, /**Fighter Neutral Attack Sprite*/
    FIGHTER_HURT /**Fighter Injured (Hurt) Sprite*/
} sprites_t;

#endif // SPRITES_AND_FIGHTERS_H