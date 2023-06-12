
// Include MicroPython API.
#include <py/runtime.h>
#include <py/gc.h>


#include <RP2040.h>
#include <pico/stdlib.h>
#include <hardware/pio.h>
#include <hardware/claim.h>
#include <can2040.h>

STATIC const mp_obj_type_t mp_type_caninterface;

typedef struct canbus_internal {
    struct can2040 internal;
    mp_obj_t * recv_queue;
    mp_fun_1_t pop;
    mp_fun_2_t push;
} canbus_internal_t;

typedef struct {
    mp_obj_base_t base;
    canbus_internal_t bus;
} mp_obj_can_interface_t;

// PRIVATE
// TODO Trying to decide what the best way of initing the device and wether we want to support
//      multiple interfaces or a more intelligent structuring of the code
STATIC mp_obj_can_interface_t *mp_can_obj_0;
STATIC mp_obj_can_interface_t *mp_can_obj_1;

static void
can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
    canbus_internal_t* bus = (canbus_internal_t*)cd; 
    if (notify == CAN2040_NOTIFY_RX)
    {
        // We shouldn't need to worry about packing because it is all multiples of 64.
        // So no matter the architecture it will be on cache lines
        // Also we probably don't need to care about an exception being thrown since 
        // we don't set the flags which would cause the exception
        bus->push(bus->recv_queue, mp_obj_new_bytes((byte*)msg, sizeof(struct can2040_msg)));
        mp_printf(MICROPY_ERROR_PRINTER, "RX!!!!\n");
    }
    if (notify == CAN2040_NOTIFY_ERROR)
    {
        mp_printf(MICROPY_ERROR_PRINTER, "Error...\n");
    }

}

static void can2040_internal_pio0_irq_handler(void)
{
    // lock the python internals while we run our irq to make sure we aren't interupted!
    mp_sched_lock();
    gc_lock();

    // handle the irq
    // TODO: how?
    can2040_pio_irq_handler(&(mp_can_obj_0->bus.internal));

    // unlock in reverse order!
    gc_unlock();
    mp_sched_unlock();
}

static void can2040_internal_pio1_irq_handler(void)
{
    // lock the python internals while we run our irq to make sure we aren't interupted!
    mp_sched_lock();
    gc_lock();

    // handle the irq
    // TODO: how?
    can2040_pio_irq_handler(&(mp_can_obj_0->bus.internal));

    // unlock in reverse order!
    gc_unlock();
    mp_sched_unlock();
}

STATIC mp_obj_t can_init_helper(mp_obj_can_interface_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_bitrate, ARG_sysclock, ARG_gpiorx, ARG_gpiotx, ARG_pionum};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bitrate,  MP_ARG_INT,   {.u_int = 500000} },
        { MP_QSTR_sysclock, MP_ARG_INT,   {.u_int = 125000000} },
        { MP_QSTR_gpiorx,   MP_ARG_INT,   {.u_int = 11} },
        { MP_QSTR_gpiotx,   MP_ARG_INT,   {.u_int = 12} },
        { MP_QSTR_pionum,   MP_ARG_INT,   {.u_int = 1}  },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint32_t pio_num = args[ARG_pionum].u_int;
    if ((pio_num != 0) && (pio_num != 1)) {
        mp_raise_ValueError("Invalid PIO number. Must be 0 or 1");
    }

    uint32_t gpio_rx = args[ARG_gpiorx].u_int;
    uint32_t gpio_tx = args[ARG_gpiotx].u_int;
    uint32_t sys_clock = args[ARG_sysclock].u_int;
    uint32_t bitrate = args[ARG_bitrate].u_int;

    pio_hw_t * pio = (pio_num == 0) ? pio0_hw : pio1_hw;
    uint pio_irq = (pio_num == 0) ? PIO0_IRQ_0 : PIO1_IRQ_0;
    if (pio_num == 0) {
        mp_can_obj_0 = self;
    } else {
        mp_can_obj_1 = self;
    }

    // Setup canbus internal structure
    can2040_setup(&self->bus.internal, pio_num);
    can2040_callback_config(&self->bus.internal, can2040_cb);

    // disable the irq while configuring it
    irq_set_enabled(pio_irq, false);

    // claim all the pio sm irqs
    for (uint8_t i=0; i < 4; i ++) {
        if (pio_sm_is_claimed(pio, i)) {
            irq_set_enabled(pio_irq, true); // reenable since we don't own it
            mp_raise_ValueError("StateMachine claimed by external resource!");
        }
        pio_sm_claim(pio, i);
    }


    irq_handler_t handler = irq_get_exclusive_handler(pio_irq);
    if (handler != NULL) {
        // this is the default MicroPython irq handler... hopefully
        irq_remove_handler(pio_irq, handler);
    }

    // configure with our IRQ handler
    
    irq_set_exclusive_handler(pio_irq, 
        (pio_num == 0) ? can2040_internal_pio0_irq_handler : can2040_internal_pio1_irq_handler);
    irq_set_priority(pio_irq, PICO_HIGHEST_IRQ_PRIORITY);
    irq_set_enabled(pio_irq, true);


    // uint32_t save = hw_claim_lock();
    // if (pio_can_add_program(pio, &prog)) {
    //     pio_add_program(pio, &prog);
    //     // mp_printf(MICROPY_ERROR_PRINTER, "Could insert program\n");
    // } else {
    //     mp_printf(MICROPY_ERROR_PRINTER, "Couldn't insert program\n");
    // }
    // hw_claim_unlock(save);

    // Start canbus
    can2040_start(&self->bus.internal, sys_clock, bitrate, gpio_rx, gpio_tx);

    return MP_OBJ_FROM_PTR(self);
}

// STATIC mp_obj_t mp_can_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
//     mp_printf(MICROPY_ERROR_PRINTER, "in __init__\n");
//     return can_init_helper(MP_OBJ_TO_PTR(pos_args[0]), n_args - 1, pos_args + 1, kw_args);
// }
// STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mp_can_init_obj, 1 , mp_can_init);

STATIC mp_obj_t mp_can_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // parse args
    mp_printf(MICROPY_ERROR_PRINTER, "In can_make_new\n");
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);

    // setup the object
    // TODO: change to use the global copy if it exists
    mp_obj_can_interface_t *self = m_new_obj(mp_obj_can_interface_t);
    self->base.type = &mp_type_caninterface;

    // create a new deque and call it's make_new (__init__)
    mp_obj_t deque_args[2] = {mp_const_empty_tuple, mp_obj_new_int(10)};
    self->bus.recv_queue = MP_OBJ_TYPE_GET_SLOT(&mp_type_deque, make_new)(&mp_type_deque, 2, 0, deque_args);

    mp_map_t *locals_map = &MP_OBJ_TYPE_GET_SLOT(type, locals_dict)->map;
    self->bus.pop = (mp_fun_1_t)mp_map_lookup(locals_map, MP_ROM_QSTR(MP_QSTR_popleft), MP_MAP_LOOKUP);
    self->bus.push = (mp_fun_2_t)mp_map_lookup(locals_map, MP_ROM_QSTR(MP_QSTR_append), MP_MAP_LOOKUP);


    // Need to setup here and set the PIO interface
    can_init_helper(self, n_args, all_args, &kw_args);

    return MP_OBJ_FROM_PTR(self);
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

    can2040_transmit(&self->bus.internal, &response);

    return mp_const_none;
}
STATIC mp_obj_t mp_can_send(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_can_send_helper(MP_OBJ_TO_PTR(pos_args[0]), n_args - 1, pos_args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mp_can_send_obj, 1 , mp_can_send);

STATIC mp_obj_t mp_can_recv_helper(mp_obj_can_interface_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    struct can2040_msg msg;

    mp_obj_t res = mp_call_function_1_protected(self->bus.pop,self->bus.recv_queue);
    if (res == MP_OBJ_NULL) {
        return MP_OBJ_NULL; // TODO: Do we want to keep waiting instead?
    }
    
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
    // { MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&mp_can_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&mp_can_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv), MP_ROM_PTR(&canbus_recv_obj) },
};
STATIC MP_DEFINE_CONST_DICT(canhack_caninterface_locals_dict, canhack_caninterface_locals_dict_table);

STATIC MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_caninterface,
    MP_QSTR_INTERFACE,
    MP_TYPE_FLAG_NONE,
    make_new, mp_can_make_new,
    locals_dict, &canhack_caninterface_locals_dict
    );

STATIC const mp_rom_map_elem_t can_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_canhack) },
    { MP_ROM_QSTR(MP_QSTR_INTERFACE), MP_ROM_PTR(&mp_type_caninterface) },
};
STATIC MP_DEFINE_CONST_DICT(can_module_globals, can_module_globals_table);

const mp_obj_module_t mp_module_canhack = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&can_module_globals,
};

// Register the module 'can' and make it available in Python
MP_REGISTER_MODULE(MP_QSTR_canhack, mp_module_canhack);
