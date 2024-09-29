/**
 * @authors Madison Reilly & Sophie Waters
 * Display Module for ENCE260 Assignment
 * Header file
*/

#ifndef DISPLAY_C 
#define DISPLAY_C 

#include "sprites_and_fighters.h"

/**
 * Initialises the LED Matrix Display - Sets all rows and columns to High. 
 * Wrapper for pio_config_set function calls.
*/
void pioInitializeLedmat(void);


/**
 * Set the luminosity for a single LED in the LED Matrix
 * @param row The row the LED is located in - value between 1 and 7
 * @param col The column the LED is located in - value between 1 and 5
 * @param luminosity The desired luminosity for the LED - value of 0, 1, 2, or 3, which correspond to off, 1/4 max luminosity, 1/2 max luminosity, and max respectively
*/
void setSingleLuminosity(uint8_t row, uint8_t col, uint8_t luminosity);


/** Set the desired luminosity value of a Rectangle of LEDs
 * @param row_start The start Row for the Rectangle - value between 1 and 7 (inclusive)
 * @param row_end The end Row of the Rectangle - value between 1 and 7 (inclusive). Must be greater than row_start
 * @param col_start The start Column for the Rectangle - value between 1 and 5 (inclusive)
 * @param col_end The end Column for the Rectangle - value between 1 and 5 (inclusive). Must be greater than col_start
 * @param luminosity The desired luminosity for the LED - value of 0, 1, 2, or 3, which correspond to off, 1/4 max luminosity, 1/2 max luminosity, and max respectively
*/
void drawRect(uint8_t row_start, uint8_t row_end, uint8_t col_start, uint8_t col_end, uint8_t luminosity);


/** Set the entire LED Matrix Display to have Luminosity Low (0).*/
void setDisplayLow(void);


/** Set the entire LED Matrix Display to have Luminosity High (3).*/
void setDisplayHigh(void);


/** 
 * Physically update the luminosity of the selected column to be the value in the write storage.
 * @param col The Column to write the intended luminosities to.
*/
void updateCol(uint8_t col);


/**
 * Draw the Selected Sprite onto the LED Matrix Display
 * @param sprite The Sprite to draw onto the Display. Enum Value corresponding to the sprite to display
 * @param x_pos X Position of the Sprite in the LED Matrix Display.
*/
void drawSprite(sprites_t sprite, uint8_t x_pos);


/**
 * Write healthbar to the display matrix so that it is visible on module task function export call
 * @param health Your current healthbar state in luminosity form, with each of the 7 rows getting two bits
*/
void drawHealth(uint16_t health);


/**
 * Wrapper for update col. This should be called at 250 Hz or higher.
 * Intended to be included into main & used at the interface between
 * the display module and the scheduling module. Kinda sloppy, improvement potential present.
*/
void displayTaskFunction(void);

/**
 * Displays a tinyGL Text saying You Win! or You Lose!
 * @param player_won boolean of whether this board (player) won or not
*/
void displayWinText(bool player_won);

#endif // DISPLAY_C