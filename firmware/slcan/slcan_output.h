#ifndef SLCAN_OUTPUT_H
#define SLCAN_OUTPUT_H

#include <can2040.h>

/*
 * @brief This function will log to stdout in the format
 * slcan expects. 
 * NOTE: requires stdio to be initialized and printf support 
 */
void log_can_message(struct can2040_msg * msg);


#endif