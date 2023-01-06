void test(void)
{
    return;
}

'''

#include "py/runtime.h"

#include <RP2040.h>
#include <pico/stdlib.h>
#include <pico/util/queue.h>
#include <pico/multicore.h>
#include <can2040.h>
#include <slcan_output.h>

// STATIC machine_can_obj_t machine_can_obj[] = {
//     {{&machine_can_type}, },
//     {{&machine_can_type}, },
// };

typedef struct {
    mp_obj_base_t base;
} mp_obj_can_handle_t;


#define CAN_RX 17;
#define CAN_TX 16;

#define PIO_NUM 0;


// static struct can2040 cbus;
// const unsigned int LEDs[] = {5,6,7,10,11};
// bool Led_Status[5] = {0};

queue_t recv_queue;

static void
can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
    if (notify == CAN2040_NOTIFY_RX)
    {
        // add the msg to the queue
        queue_try_add(&recv_queue, msg); 
    }
}


static void PIOx_IRQHandler(void)
{
    can2040_pio_irq_handler(&cbus);
}


STATIC mp_obj_t canbus_init_helper(can2040 *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    uint32_t pio_num = PIO_NUM;
    uint32_t gpio_rx = CAN_RX;
    uint32_t gpio_tx = CAN_TX;

    uint32_t sys_clock = 125000000;
    uint32_t bitrate = 500000;

    // setup queue and what the hell is the usb thing doing?
    queue_init(&recv_queue, sizeof(struct can2040_msg), 10); // 10 messages should be enough during normal execution
    stdio_usb_init();

    // Setup canbus
    can2040_setup(self, pio_num);
    can2040_callback_config(self, can2040_cb);

    // Enable irqs
    irq_set_exclusive_handler(PIO0_IRQ_0_IRQn, PIOx_IRQHandler);
    NVIC_SetPriority(PIO0_IRQ_0_IRQn, 1);
    NVIC_EnableIRQ(PIO0_IRQ_0_IRQn);

    // Start canbus
    can2040_start(self, sys_clock, bitrate, gpio_rx, gpio_tx);

    return mp_const_none;
}

STATIC mp_obj_t canbus_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return canbus_init_helper(MP_OBJ_TO_PTR(args[0]), n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(canbus_init_obj, 1 , canbus_init);


STATIC mp_obj_t can_make_new(const mp_objtype_t *type, size_t n_args, size_t n_kw, const mpasdfasf_obj_t *all_args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    can2040 *self;

    canbus_init_helper(self, n_args -1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t mp_can_send(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return();
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(canbus_send_obj, 1 , mp_can_send);

STATIC mp_obj_t mp_can_recv(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return();
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(canbus_recv_obj, 1 , mp_can_recv);

STATIC const mp_rom_map_elem_t can_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&canbus_init_obj)}
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&canbus_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv), MP_ROM_PTR(&canbus_recv_obj) },
};
STATIC MP_DEFINE_CONST_DICT(can_module_globals, can_module_globals_table)

MP_DEFINE_CONST_OBJ_TYPE(
    pyb_caniface_tpye,
    MP_QSTR_CANIFACE,
    MP_TYPE_FLAG_NONE,
    make_new, can_make_new,
    // print, ???,
    locals_dict, &can_module_globals
    );

// Register the module 'can' and make it available in Python
MP_REGISTER_MODULE(MP_QSTR_can, can_cmodule);

'''