#include "pico/stdlib.h"

const unsigned int LEDs[] = {5,6,7,10,11};

int main() {
    for(int i =0; i < (sizeof(LEDs)/sizeof(LEDs[0])); i++) {
        gpio_init(LEDs[i]);
        gpio_set_dir(LEDs[i], GPIO_OUT);
    }

    while (1) {
        for(int i =0; i < (sizeof(LEDs)/sizeof(LEDs[0])); i++) {
            gpio_put(LEDs[i], 1);
            sleep_ms(1000);
            gpio_put(LEDs[i], 0);
        }
    }
}