#include <RP2040.h>
#include <pico/stdlib.h>
#include <pico/util/queue.h>
#include <pico/multicore.h>
#include <can2040.h>

#define CAN_RX 11;
#define CAN_TX 12;

static struct can2040 cbus = {};
// const unsigned int LEDs[] = {5,6,7,10,11};
// bool Led_Status[5] = {0};
queue_t recv_queue = {};

static void
can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
    if (notify == CAN2040_NOTIFY_RX)
    {
        // add the msg to the queue
        queue_add_blocking(&recv_queue, msg); 
    } else if (notify == CAN2040_NOTIFY_TX) {
        msg->dlc &= CAN2040_NOTIFY_TX;
        queue_add_blocking(&recv_queue, msg);
    } else if (notify & CAN2040_NOTIFY_ERROR) {
        msg->data32[0] = notify;
        queue_try_add(&recv_queue, msg);
    }
}

static void
PIOx_IRQHandler(void)
{
    can2040_pio_irq_handler(&cbus);
}

// static void
// can_msg_handler(void) 
// {
//     struct can2040_msg msg;

//     while (1)
//     {
//         queue_remove_blocking(&recv_queue, &msg);

//         log_can_message(&msg);
        
//     }
// }

void
canbus_setup(void)
{
    uint32_t pio_num = 0;
    uint32_t sys_clock = 125000000;
    uint32_t bitrate = 500000;
    uint32_t gpio_rx = CAN_RX;
    uint32_t gpio_tx = CAN_TX;

    // Setup canbus
    can2040_setup(&cbus, pio_num);
    can2040_callback_config(&cbus, can2040_cb);

    // Enable irqs
    irq_set_exclusive_handler(PIO0_IRQ_0_IRQn, PIOx_IRQHandler);
    NVIC_SetPriority(PIO0_IRQ_0_IRQn, 1);
    NVIC_EnableIRQ(PIO0_IRQ_0_IRQn);

    // Start canbus
    can2040_start(&cbus, sys_clock, bitrate, gpio_rx, gpio_tx);
}

int main() {
    queue_init(&recv_queue, sizeof(struct can2040_msg), 10); // 10 messages should be enough during normal execution
    stdio_usb_init();
    canbus_setup();

    struct can2040_msg msg = {
        .id = 2 | CAN2040_ID_EFF,
        .dlc = 8,
        .data = {
            0xde,
            0xad,
            0xbe,
            0xef,
            0x00,
            0xc0,
            0xff,
            0xee
        }
    };

    
    if (can2040_check_transmit(&cbus)) {
        can2040_transmit(&cbus, &msg);
        printf("Transmit Once\n");
    } else {
        printf("Not Transmitting!\n");
    }

    while (1)
    {   
        struct can2040_msg response;
        queue_remove_blocking(&recv_queue, &response);
        printf("%d|%d|%08X\n", response.id, response.dlc, response.data);
    }
    
}

