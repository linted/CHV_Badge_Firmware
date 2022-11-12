#include <pico/printf.h>
#include <slcan_output.h>

void log_can_message(struct can2040_msg * msg) {
    
    
    if (msg->id & CAN2040_ID_EFF)
    {
        // T - extended can msg
        // %08X - 8 hex character msg id
        printf("T%08X", msg->id);
    } 
    else
    {
        // t - regular can msg
        // %03X - 3 hex char msg id
        printf("t%03X", msg->id);
    }
    
    uint8_t len = msg->dlc;
    if(len > 8) { 
        len = 8;
    }

    // %01u - 1 character for the dlc
    printf("%01u", len);
    for(int i=0; i < len; i++)
    {
        // %02X - 2 hex chars for each data byte
        printf("%02X", msg->data[i]);
    }
    printf("\r");
    stdio_flush();
}