/**
 * @file communications.c
 * @authors Madison Reilly & Sophie Waters
 * Communications file for the ENCE260 Assignment
 * Contains functions which are used to compare input from and send output 
 * to the IR LED
 * @brief IR LED Communications
*/


#include <stdlib.h> // Need this for the calloc function to show up
#include "ir_uart.h"

#include "game.h"
#include "communications.h"


#define RESERVED_HEALTH 0b111 /**Want to send something that means "I am not sending a sprite right now"*/
#define RESERVED_DATA 0 /**Want to send something that means "I am not sending data right now" Has to be 0 because of bitmappping shenanigans*/
#define DATA_MASK 0x07 /**Get the last 3 bits of a sample by &*/

#define PACKET_HEADER_POS 6 
#define PACKET_HEALTH_POS 3
#define PACKET_DATA_POS   0
#define NUMBER_OF_UCFK 2
#define WONGAME 1
#define LOSTGAME 0

#define BITS_PER_UINT8 8

/**This is used to ensure everything gets freed*/
#define NULL ((void *)0)

/**Pointer to the last packet sent by this UCFK as stored in the Heap (Allows lifetime outside of the Program)*/
static int8_t* last_packet;

/**The number of elements in the last packet which was sent, only used for retransmission*/
static uint8_t n_last_packet = 0;

/**The most receently read packet from the ir tranciever*/
static char last_recieved_char;

static uint8_t current_packet = 0;

/**
 * Retransmit the last transmitted packet (stored in the heap) to another UCFK
 * Call this function when you receive a negative ACK
*/
void retransmit(void) 
{
    for (uint8_t i = 0; i < n_last_packet; i++) {
        char char_to_send = *(last_packet + i); // Get the character stored in memory at Last_Packet + i
        ir_uart_putc(char_to_send); // Send that character to the other funkit
    }
}

/**
 * Extracts the damage field out of a packet
 * @param packet The packet to extract from
 * @returns The damage field as a uint8_t
*/
uint8_t getDamageDone(uint8_t packet) 
{
    return ((packet >> PACKET_HEALTH_POS) & RESERVED_HEALTH);
}


void sendLost(void)
{
    PacketHeader_t header = ENDGAME;
    uint8_t packet = (header) << PACKET_HEADER_POS | (WONGAME);
    ir_uart_putc((char)packet);
    if (last_packet != NULL) {
        free(last_packet);
    }
}

void sendAttack(uint8_t damage, uint8_t player_x)
{
    PacketHeader_t header = HEALTH;
    uint8_t packet = (header) << PACKET_HEADER_POS | (damage << PACKET_HEALTH_POS) | player_x;
    ir_uart_putc((char)packet);
    if (last_packet != NULL) {
        free(last_packet);
    }

    last_packet = calloc(2, sizeof(int8_t));
    *(last_packet) = header;
    *(last_packet + 1) = damage;
    n_last_packet = 2;
}


/**
 * This deconstructs a packet. yielding the health field
 * @param packet The packet to deconstruct
 * @returns The packet'd health/damage field.
*/
uint8_t processEnemyPositionX(uint8_t packet)
{
    return (uint8_t)((packet >> PACKET_HEALTH_POS) & DATA_MASK);
}

void checkInputChar(void)
{
    if (ir_uart_read_ready_p()) { // This if makes this non blocking
        last_recieved_char = ir_uart_getc();
    } else {
        last_recieved_char = 0;
    }
}


/**
 * Registers an enemy win for a write to game.c's won_state variable from the other funkit's packets
 * @param packet Incoming packet
 * @returns The last 3 bits of the packet casted appropriately.
*/
uint8_t processEnemyWin(uint8_t packet)
{
    return (uint8_t)(packet & DATA_MASK);
}

/**
 * Counts the number of '1' bits in the packet and if it is even returns true (even parity)
 * @param packet The packet to check the parity validity of.
 * @returns True if the number of 1 bits in the Packet is even
*/
bool checkParityBits(uint8_t packet)
{
    uint8_t bit_count = 0; // Count the number of ones in the packet
    for (int8_t i = BITS_PER_UINT8 - 1; i > -1; i--) {
        if (((packet >> i) & 1) > 0) {
            bit_count++;
        }
    }
    return (bit_count % 2 == 0);
}


/**
 * Process the Game Logic from a received IR Transmission, alongside handling some QOS business re: mandantory retransmission
 * processing on reciever side and parity bit checks.
 * @param packet The packet read by the IR Receiver, contains the enemy sprite and data payload (either Enemy X or Damage Taken)
*/
void processLastCharacter(uint8_t packet)
{
    uint8_t x_position_trans;
    
    // this block helps with QOS, we send each packet multiple times and if they all get through the same we know the
    // packet is okay :)
    static uint8_t retransmission_count = 0;
    if (retransmission_count == 0) {
        current_packet = packet;
    } else if (packet == current_packet) {
        retransmission_count ++;
    } else {
        retransmission_count = 0;
    }

    uint8_t recieved_header = (packet >> PACKET_HEADER_POS);
    
    
    switch (recieved_header) {
    // Switch Statement so we can add more cases
        case ACKNOWLEDGE:
            if (checkParityBits(packet)) {
                free(last_packet);
                last_packet = NULL;
                n_last_packet = 0; // Set to 0 as packet is now totally empty
            } else {
                retransmit();
            }
            break;
        case ENDGAME:
            won_game = processEnemyWin(packet); // Set the won game flag to true
            break;
        case HEALTH:
            if (retransmission_count % 3) {
                x_position_trans = processEnemyPositionX(packet);
                setEnemyX(x_position_trans);
                if (checkHitLanded()) {
                    uint8_t damage = getDamageDone(packet);
                    setCurrentHealth(damage);
                }
                sendACK();
            } else {
                retransmission_count++;
            }
            break;
        default:
            //sendACK(true); // Send a Negative ACK to the other UCFK to tell it we didnt get their last transmission
            break; // Should only reach this case if we receive an 
            //        input that has no corresponding game Logic i.e an errored transmission
    }
}

void communicationsProcessingTaskFunction(void)
{
    char inputChar = last_recieved_char;
    if (inputChar != 0) {
        processLastCharacter(inputChar);
    }
}


void sendACK(void) 
{
    PacketHeader_t ack_packet = 0; // to ensure no 'there was still data here' shenanigans
    ack_packet = ACKNOWLEDGE;
    uint8_t parity_bit = 1; // know the parity of ACK ahead of time, can hard code this bit
    uint8_t packet = (ack_packet << PACKET_HEADER_POS) | parity_bit;
    ir_uart_putc(packet); // Send an A to acknowledge correct receipt of last transmission (ACK)
}

