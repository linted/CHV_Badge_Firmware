// #include <RP2040.h>
#include "pico/stdlib.h"

#include <stdint.h>



typedef enum {
    LED_HIGH,
    LED_LOW,
    LED_OFF
} led_states_t;

typedef struct led {
    led_states_t lines[4];
} led_t;

const unsigned int LED_LINES[] = {6,5,4,3};

led_t LEDS[] = {
    {LED_LOW, LED_HIGH, LED_OFF, LED_OFF}, //18
    {LED_HIGH, LED_LOW, LED_OFF, LED_OFF}, //15
    {LED_OFF, LED_LOW, LED_HIGH, LED_OFF}, //22
    {LED_OFF, LED_HIGH, LED_LOW, LED_OFF}, //19
    {LED_HIGH, LED_OFF, LED_LOW, LED_OFF}, //16
    {LED_OFF, LED_OFF, LED_HIGH, LED_LOW}, //23
    {LED_OFF, LED_HIGH, LED_OFF, LED_LOW}, //20
    {LED_HIGH, LED_OFF, LED_OFF, LED_LOW}, //17
    {LED_LOW, LED_OFF, LED_HIGH, LED_OFF}, //21
};


void do_led(int led_num) {
    printf("LED[%d] == (", led_num);
    for (int i =0; i < 4; i++) {
        if (LEDS[led_num].lines[i] == LED_HIGH) {
            printf("%d:LED_HIGH", LED_LINES[i]);
            // gpio_set_pulls (LED_LINES[i], 1, 0);
            gpio_set_dir(LED_LINES[i], GPIO_OUT);
            gpio_put(LED_LINES[i], 1);
        } else if (LEDS[led_num].lines[i] == LED_LOW) {
            printf("%d:LED_LOW",LED_LINES[i]);
            gpio_set_dir(LED_LINES[i], GPIO_OUT);
            gpio_put(LED_LINES[i], 0);
            // gpio_pull_down(LED_LINES[i]);
            // gpio_set_oeover(LED_LINES[i], 1);
            // gpio_set_outover(LED_LINES[i], 2);
        } else {
            printf("%d:LED_OFF",LED_LINES[i]);
            gpio_set_dir(LED_LINES[i], GPIO_IN);
            gpio_put(LED_LINES[i], 0);
            // gpio_pull_up(LED_LINES[i]);
            // gpio_set_oeover(LED_LINES[i], 0);
            // gpio_set_outover(LED_LINES[i], 2);
        }
        printf(",");
    }
    printf(")\n");
}


int main() {
    stdio_usb_init();

    for(int i =0; i < 4; i++) {
        gpio_init(LED_LINES[i]);
        gpio_set_dir(LED_LINES[i], GPIO_IN);
    }

    while (1) {
        for(int i =0; i < 9; i++) {
            printf("Doing led %d\n", i);
            do_led(i);
            sleep_ms(1000);
        }
    }
}