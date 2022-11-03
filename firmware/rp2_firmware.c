#include <pico/stdlib.h>
#include <can2040.h>

#define CAN_RX 17;
#define CAN_TX 16;

static struct can2040 cbus;
const unsigned int LEDs[] = {5,6,7,10,11};
bool Led_Status[5] = {0};

static void
can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
    if (notify == CAN2040_NOTIFY_RX)
    {
        // we got a message!
        if (msg->id == 1) {
            // it's a message from RP1!
            // flip the light they said
            Led_In_Message = msg->data[0] % sizeof(Led_Status);
            Led_Status[Led_In_Message] = !Led_Status[Led_In_Message];
            gpio_put(LEDs[Led_In_Message], Led_Status[Led_In_Message]);
        }
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
    uint32_t sys_clock = 125000000
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
    canbus_setup();

    for(int i =0; i < (sizeof(LEDs)/sizeof(LEDs[0])); i++) {
        gpio_init(LEDs[i]);
        gpio_set_dir(LEDs[i], GPIO_OUT);
    }

    uint8_t counter = 0;
    while (1)
    {
        struct can2040_msg msg = {
            .id = 2,
            .dlc = 1,
            .data = {
                counter
            }
        }
        can2040_transmit(&cbus, &msg);
        sleep_ms(1000);
        counter = (counter + 1) % UINT8_MAX;
    }
    
}

