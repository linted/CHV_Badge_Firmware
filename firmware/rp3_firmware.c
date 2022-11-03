#include <RP2040.h>
#include <pico/stdlib.h>
#include <can2040.h>
#include <pico/util/queue.h>

#define CAN_RX 17;
#define CAN_TX 16;

static struct can2040 cbus;
const unsigned int LEDs[] = {5,6,7,10,11};
bool Led_Status[5] = {0};
queue_t recv_queue;

static void
can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
    queue_try_add(&recv_queue, msg); // add the msg to the queue
}

static void
PIOx_IRQHandler(void)
{
    can2040_pio_irq_handler(&cbus);
}


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
    canbus_setup();

    struct can2040_msg msg;
    while (1)
    {
        queue_remove_blocking(&recv_queue, &msg);
        if (msg.id == 2)
        {
            uint8_t Led_In_Message = msg.data[0] % sizeof(Led_Status);
            Led_Status[Led_In_Message] = !Led_Status[Led_In_Message];
            gpio_put(LEDs[Led_In_Message], Led_Status[Led_In_Message]);
        }
        
        struct can2040_msg response = {
            .id = 3,
            .dlc = 1,
            .data32 = {
                0xdeadbeef,
                0xc0ffee
            }
        };
        can2040_transmit(&cbus, &msg);
    }
    
}

