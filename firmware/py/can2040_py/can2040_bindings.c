// Include the header file to get access to the MicroPython API
#include "py/dynruntime.h"

#define CLASS_INIT_SLOT 0

mp_obj_full_type_t can2040_type;

// // This is the function which will be called from Python, as factorial(x)
// STATIC mp_obj_t factorial(mp_obj_t x_obj) {
//     // Extract the integer from the MicroPython input object
//     mp_int_t x = mp_obj_get_int(x_obj);
//     // Calculate the factorial
//     mp_int_t result = factorial_helper(x);
//     // Convert the result to a MicroPython integer object and return it
//     return mp_obj_new_int(result);
// }
// // Define a Python reference to the function above
// STATIC MP_DEFINE_CONST_FUN_OBJ_1(factorial_obj, factorial);

STATIC can2040_init(mp_obj_t self) {
    // Do stuffs here??
}

// This is the entry point and is called when the module is imported
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    // This must be first, it sets up the globals dict and other things
    MP_DYNRUNTIME_INIT_ENTRY

    // regiter the init funciton
    // MP_OBJ_TYPE_SET_SLOT(can2040_type, make_new, )

    // // Make the function available in the module's namespace
    // mp_store_global(MP_QSTR_factorial, MP_OBJ_FROM_PTR(&factorial_obj));

    // This must be last, it restores the globals dict
    MP_DYNRUNTIME_INIT_EXIT
}