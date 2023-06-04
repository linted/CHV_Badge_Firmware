
// Include MicroPython API.
#include "py/runtime.h"

#include <RP2040.h>
#include <can2040.h>
#include <pico/stdlib.h>
#include <pico/stdio.h>
#include <pico/util/queue.h>

STATIC const mp_obj_type_t mp_caninterface_type;

typedef struct {
    mp_obj_base_t base;
    queue_t recv_queue;
    struct can2040 internal;
} mp_obj_can_interface_t;

// PRIVATE
// TODO Trying to decide what the best way of initing the device and wether we want to support
//      multiple interfaces or a more intelligent structuring of the code
STATIC mp_obj_can_interface_t mp_can_obj_0 = {.internal.pio_num = 0};

static void
can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
    if (notify == CAN2040_NOTIFY_RX)
    {
        // add the msg to the queue
        queue_try_add(&mp_can_obj_0.recv_queue, msg); 
    }
    if (notify == CAN2040_NOTIFY_ERROR)
    {
        mp_printf(MICROPY_ERROR_PRINTER, "Error...\n");
    }

}

static void PIOx_IRQHandler(void)
{
    can2040_pio_irq_handler(&mp_can_obj_0.internal);
}

STATIC mp_obj_t can_init_helper(mp_obj_can_interface_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_bitrate, ARG_sysclock, ARG_gpiorx, ARG_gpiotx, ARG_pionum};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bitrate,  MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = 500000} },
        { MP_QSTR_sysclock, MP_ARG_INT,   {.u_int = 125000000} },
        { MP_QSTR_gpiorx,   MP_ARG_INT,   {.u_int = 11} },
        { MP_QSTR_gpiotx,   MP_ARG_INT,   {.u_int = 12} },
        { MP_QSTR_pionum,   MP_ARG_INT,   {.u_int = 1}  },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint32_t pio_num = args[ARG_pionum].u_int;
    uint32_t gpio_rx = args[ARG_gpiorx].u_int;
    uint32_t gpio_tx = args[ARG_gpiotx].u_int;

    uint32_t sys_clock = args[ARG_sysclock].u_int;
    uint32_t bitrate = args[ARG_bitrate].u_int;

    // setup queue and what the hell is the usb thing doing?
    queue_init(&self->recv_queue, sizeof(struct can2040_msg), 10); // 10 messages should be enough during normal execution
    // stdio_usb_init();

    // Setup canbus
    can2040_setup(&self->internal, pio_num);
    can2040_callback_config(&self->internal, can2040_cb);

    // Enable irqs
    irq_set_exclusive_handler(PIO1_IRQ_0_IRQn, PIOx_IRQHandler);
    irq_set_priority(PIO1_IRQ_0_IRQn, PICO_HIGHEST_IRQ_PRIORITY);
    irq_set_enabled(PIO1_IRQ_0_IRQn, true);

    // Start canbus
    can2040_start(&self->internal, sys_clock, bitrate, gpio_rx, gpio_tx);

    return mp_const_none;
}

STATIC mp_obj_t mp_can_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return can_init_helper(MP_OBJ_TO_PTR(pos_args[0]), n_args - 1, pos_args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mp_can_init_obj, 1 , mp_can_init);

STATIC mp_obj_t mp_can_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // parse args
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);

    // setup the object
    mp_obj_can_interface_t *self = &mp_can_obj_0;
    self->base.type = &mp_caninterface_type;

    // Need to setup here and set the PIO interface
    can_init_helper(self, n_args, all_args, &kw_args);

    return (mp_obj_t)self;
}

STATIC mp_obj_t mp_can_send_helper(mp_obj_can_interface_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_id, ARG_dlc, ARG_data, ARG_extframe};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = 0} },
        { MP_QSTR_dlc,      MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = 0} },
        { MP_QSTR_data,     MP_ARG_REQUIRED | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_extframe, MP_ARG_BOOL,                    {.u_bool = false} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    struct can2040_msg response;

    if (args[ARG_extframe].u_bool == true) {
        response.id = args[ARG_id].u_int & 0x1FFFFFFF;
    } else {
        response.id = args[ARG_id].u_int & 0x7FF;
    }

    response.dlc = args[ARG_dlc].u_int & 0xFF;

    mp_buffer_info_t data;
    mp_get_buffer_raise(args[ARG_data].u_obj, &data, MP_BUFFER_READ);
    for(int i = 0; i < data.len || i < 8; i++) {
        response.data[i] = ((uint8_t*)data.buf)[i];
    }

    // Debugging
    // mp_printf(MICROPY_ERROR_PRINTER, "%d - %d\n", response.id, response.dlc);
    // for(int i = 0; i < 8; i++) {
    //     mp_printf(MICROPY_ERROR_PRINTER, "%d\n", ((uint8_t*)data.buf)[i]);
    // }

    can2040_transmit(&self->internal, &response);

    return mp_const_none;
}
STATIC mp_obj_t mp_can_send(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_can_send_helper(MP_OBJ_TO_PTR(pos_args[0]), n_args - 1, pos_args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mp_can_send_obj, 1 , mp_can_send);

STATIC mp_obj_t mp_can_recv_helper(mp_obj_can_interface_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    struct can2040_msg msg;

    queue_remove_blocking(&self->recv_queue, &msg);
    
    mp_obj_t *items;

    mp_obj_t ret_obj = mp_obj_new_tuple(3, NULL);
    items = ((mp_obj_tuple_t *)MP_OBJ_TO_PTR(ret_obj))->items;
    items[2] = mp_obj_new_bytes(msg.data, 8);

    items[0] = MP_OBJ_NEW_SMALL_INT(msg.id);
    items[1] = MP_OBJ_NEW_SMALL_INT(msg.dlc);

    // Return the result
    return ret_obj;
}
STATIC mp_obj_t mp_can_recv(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_can_recv_helper(MP_OBJ_TO_PTR(pos_args[0]), n_args - 1, pos_args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(canbus_recv_obj, 1 , mp_can_recv);

STATIC const mp_rom_map_elem_t canhack_caninterface_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&mp_can_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&mp_can_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv), MP_ROM_PTR(&canbus_recv_obj) },
};
STATIC MP_DEFINE_CONST_DICT(canhack_caninterface_locals_dict, canhack_caninterface_locals_dict_table);

STATIC MP_DEFINE_CONST_OBJ_TYPE(
    mp_caninterface_type,
    MP_QSTR_INTERFACE,
    MP_TYPE_FLAG_NONE,
    make_new, mp_can_make_new,
    // print, ???,
    locals_dict, &canhack_caninterface_locals_dict
    );

STATIC const mp_rom_map_elem_t can_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_canhack) },
    { MP_ROM_QSTR(MP_QSTR_INTERFACE), MP_ROM_PTR(&mp_caninterface_type) },
};
STATIC MP_DEFINE_CONST_DICT(can_module_globals, can_module_globals_table);

const mp_obj_module_t mp_module_canhack = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&can_module_globals,
};

// Register the module 'can' and make it available in Python
MP_REGISTER_MODULE(MP_QSTR_canhack, mp_module_canhack);
