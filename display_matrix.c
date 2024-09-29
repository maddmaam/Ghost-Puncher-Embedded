/**
 * @authors Madison Reilly & Sophie Waters
 * Display Module for ENCE260 Assignment
 * Designed to run on the UCFK4
 * @brief Display Module for UCFK4
*/

#include "pio.h"
#include "tinygl.h"
#include "../fonts/font5x7_1.h"

#include "display_matrix.h"
#include "sprites_and_fighters.h"


#define LOW  PIO_OUTPUT_LOW /**Output Low*/
#define HIGH PIO_OUTPUT_HIGH /** Output High*/

#define ONE_ORIGIN_CORRECTOR (-1)       /**Rows are indexed 1 origin, but stored in a zero origin indexed array. So row1 is stored at rows[0]. Very annoying*/
#define HEALTH_BAR_COLUMN_RESERVER (-1) /**The playable area is actually only 4 columns because the healthbar takes up the fifth*/
#define BITS_PER_LED 2                  /**Number of bits per LED*/
#define PWM_CYCLE_LENGTH 4              /**Describes the number of calls of update_col to one col needed to specify an entire PWM cycle, ie: all leds meant to turn on are active at least once*/
#define VALID_LUM_MASK 0b11             /**Luminosities are two bit values, but they are stored at uint16_ts, so we often need a mask to bit manip :3 */
#define SPRITE_MASK 0xFF                /**Similar story here but it's for an entire sprite*/

#define CHAR_SPEED 1 /**Number of characters per 10s, tinygl */
#define CALL_SPEED 1250 /**How fast we intend to call tinygl_update so that tinygl can do math to work out how fast to move the letters*/

/**PIO Value Array of the LED Matrix Display Rows*/
static const pio_t rows[] =
{
    LEDMAT_ROW1_PIO, LEDMAT_ROW2_PIO, LEDMAT_ROW3_PIO, LEDMAT_ROW4_PIO, LEDMAT_ROW5_PIO, LEDMAT_ROW6_PIO, LEDMAT_ROW7_PIO
};

/**PIO Value Array of the LED Matrix Display Columns*/
static const pio_t cols[] =
{
    LEDMAT_COL1_PIO, LEDMAT_COL2_PIO, LEDMAT_COL3_PIO, LEDMAT_COL4_PIO, LEDMAT_COL5_PIO
};


/**
 * Stores the current state of each Row in the LED Matrix Display.
 * Each LED gets 2 bits, so each value in the Array represents up to 8 LEDs
*/
static uint16_t current_display_state[LEDMAT_COLS_NUM] = {0xFF, 0xFF, 0xFF, 0xFF, 0b00011011011011}; // this should be zeroed in the final product.


void pioInitializeLedmat(void)
{
    for (uint8_t row = 1; row <= LEDMAT_ROWS_NUM; row ++) {
        pio_config_set(rows[row + ONE_ORIGIN_CORRECTOR], HIGH);
    }

    for (uint8_t col = 1; col <= LEDMAT_COLS_NUM; col ++) {
        pio_config_set(cols[col + ONE_ORIGIN_CORRECTOR], HIGH);
    }
}


void setSingleLuminosity(uint8_t row, uint8_t col, uint8_t luminosity)
{
    uint16_t mask = VALID_LUM_MASK << (BITS_PER_LED * row); // the - 1 to deal with 1 origin rows and the dead bits at the end of the uint16 cancel :)
    // if high because we're setting it high (lum & mask) or high because it was already high and not at target pos (rhs + de morgan to deal with leading zeros)
    current_display_state[col + ONE_ORIGIN_CORRECTOR] = ((luminosity << BITS_PER_LED * (row)) & mask) | ~(~current_display_state[col + ONE_ORIGIN_CORRECTOR] | mask);
}


void setDisplayLow(void)
{// Just... just set it low... it hurts my eyes :( The whole thing. off, now, please :( :( 
    for (uint8_t col = 1; col <= LEDMAT_COLS_NUM; col ++) {
        current_display_state[col + ONE_ORIGIN_CORRECTOR] = 0;
    }
}


void setDisplayHigh(void)
{// SET THE WHOLE DISPLAY HIGH!!! BWAHAHAHAHAHAHAHAHAHAAAH!!!! BEHOLD THE POWER OF THE SUN!!!! LUMINOSITY 3!!!
    for (uint8_t col = 1; col <= LEDMAT_COLS_NUM; col ++) {
        current_display_state[col + ONE_ORIGIN_CORRECTOR] = 0xFFFC;
    }
}


void drawRect(uint8_t row_start, uint8_t row_end, uint8_t col_start, uint8_t col_end, uint8_t luminosity)
{// helper function to draw a rectangle between (row/col)_start and (row/col)_end inclusive, at value luminosity. assumes start <= end
    if (row_start < 1) {
        row_start = 1;
    }
    if (row_end > LEDMAT_ROWS_NUM) {
        row_end = LEDMAT_ROWS_NUM;
    }
    if (col_start < 1) {
        col_start = 1;
    }
    if (col_end > LEDMAT_COLS_NUM) {
        col_end = LEDMAT_COLS_NUM;
    }
    
    for (uint8_t row = row_start; row <= row_end; row ++) { // this can be done faster. if I have time if I have time if I have time
        for (uint8_t col = col_start; col <= col_end; col++) {
            setSingleLuminosity(row, col, luminosity);
        }
    }
}


void drawSprite(sprites_t sprite, uint8_t x_pos)
{// writes a sprite (ideally from "sprites_and_fighters.h") to current_display_state
    uint16_t mask = SPRITE_MASK << (BITS_PER_LED * x_pos);
    const uint8_t* current_sprite = FIGHTER_NEUTRAL_SPRITE;
    switch (sprite) {
        case FIGHTER_NEUTRAL:
            current_sprite = FIGHTER_NEUTRAL_SPRITE;
            break;
        case FIGHTER_PUNCH:
            current_sprite = FIGHTER_PUNCH_SPRITE;
            break;
        case FIGHTER_KICK:
            current_sprite = FIGHTER_KICK_SPRITE;
            break;
        case FIGHTER_JUMP:
            current_sprite = FIGHTER_JUMP_SPRITE;
            break;
        case FIGHTER_NEUTRAL_ATTACK:
            current_sprite = FIGHTER_NEUTRAL_ATTACK_SPRITE;
            break;
        case FIGHTER_HURT:
            current_sprite = FIGHTER_HURT_SPRITE;
            break;
        default:
            // THIS STATE SHOULD NEVER BE REACHED UNLESS SOMETHING HAS GONE WRONG
            break;
    }
    for (uint8_t col = LEDMAT_COLS_NUM + HEALTH_BAR_COLUMN_RESERVER; col > 0; col--){
        // very similar to the way set_single_luminosity works, relies on sprites being formatted nicely
        current_display_state[col + ONE_ORIGIN_CORRECTOR] = ((current_sprite[LEDMAT_COLS_NUM - col + ONE_ORIGIN_CORRECTOR] << BITS_PER_LED * x_pos) & mask) | ~(~current_display_state[col + ONE_ORIGIN_CORRECTOR] | mask);
    }
}


void drawHealth(uint16_t health)
{// health should be displayed in a bar along the top column
    current_display_state[LEDMAT_COLS_NUM + ONE_ORIGIN_CORRECTOR] = health;
}


void updateCol(uint8_t col)
{
    static uint8_t pwm_selector = 0;
    static const uint8_t pwm_selections[PWM_CYCLE_LENGTH] = {3, 3, 2, 1}; // the minimum luminosity required to be active
    // note to self: might be better if pwm_selections wasn't static?
    if (col == 1) {
        pio_output_high(cols[LEDMAT_COLS_NUM + ONE_ORIGIN_CORRECTOR]);
        
        if (pwm_selector < PWM_CYCLE_LENGTH - 1) {
            pwm_selector ++;
        } else {
            pwm_selector = 0;
        }
    } else {
        pio_output_high(cols[(col + ONE_ORIGIN_CORRECTOR) - 1]);
    }

    for (uint8_t row = 1; row <= LEDMAT_ROWS_NUM; row ++) {
        // if (current mimimum lum to be active) <= the luminosity of this specific led: activate this led
        if (pwm_selections[pwm_selector] <= (((current_display_state[col + ONE_ORIGIN_CORRECTOR] >> (BITS_PER_LED * row))) & VALID_LUM_MASK)) {
            pio_output_low(rows[row + ONE_ORIGIN_CORRECTOR]);
        } else {
            pio_output_high(rows[row + ONE_ORIGIN_CORRECTOR]);
        }
    }

    pio_output_low(cols[col + ONE_ORIGIN_CORRECTOR]);
    
}


void displayTaskFunction(void)
{
    static uint8_t col = 1;
    updateCol(col);
    if (col < LEDMAT_COLS_NUM) {
        col ++;
    } else {
        col = 1;
    }

}


void displayWinText(bool player_won)
{   

    //setDisplayLow();
    tinygl_init(CALL_SPEED);
    tinygl_font_set(&font5x7_1);
    tinygl_text_speed_set (CHAR_SPEED);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    
    if (player_won) {
        tinygl_text("You Win!");
    } else {
        tinygl_text("You Lose!");
    }
    while(1) {
        tinygl_update();
    }
}