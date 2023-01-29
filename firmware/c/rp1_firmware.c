#include <RP2040.h>
#include <pico/stdlib.h>
#include <can2040.h>
#include <slcan_output.h>
#include <pico/util/queue.h>
#include <stdbool.h>


#define CAN_RX 17;
#define CAN_TX 16;

static struct can2040 cbus;
queue_t recv_queue;
bool on_off;

static void
can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
    // if (notify == CAN2040_NOTIFY_RX)
    {
        // add the msg to the queue
        queue_try_add(&recv_queue, msg); 
    }
    
}

static void handle_queue(void)
{
    // Add message processing code here...
    struct can2040_msg msg;
    while (1)
    {
        queue_remove_blocking(&recv_queue, &msg);
        if(msg.id > 0x2)
        {
            gpio_put(5, on_off);
            on_off = !on_off;
        }

        log_can_message(&msg);
    }
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
    stdio_usb_init();
    canbus_setup();

    gpio_init(5);
    gpio_set_dir(5, GPIO_OUT);
    multicore_launch_core1(handle_queue);

    struct can2040_msg msg = {
        .id = 1,
        .dlc = 1,
        .data32 = {
            0xffffffff, 
            0xffffffff
        }
    };
    while (1)
    {
        can2040_transmit(&cbus, &msg);
        sleep_ms(1000);
    }
    
}

