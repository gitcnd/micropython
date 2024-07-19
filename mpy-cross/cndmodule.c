#include "py/obj.h"
#include "py/runtime.h"

// Define a function in C
STATIC mp_obj_t cndmodule_hello_world() {
    printf("Hello, World!\n");
    return mp_const_none;
}

// Define a reference to the function
STATIC MP_DEFINE_CONST_FUN_OBJ_0(cndmodule_hello_world_obj, cndmodule_hello_world);

// Create a table of global functions
STATIC const mp_rom_map_elem_t cndmodule_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_hello_world), MP_ROM_PTR(&cndmodule_hello_world_obj) },
};

STATIC MP_DEFINE_CONST_DICT(cndmodule_globals, cndmodule_globals_table);

// Define the module
const mp_obj_module_t cndmodule_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&cndmodule_globals,
};

// Register the module
MP_REGISTER_MODULE(MP_QSTR_cndmodule, cndmodule_module, MODULE_CNDMODULE_ENABLED);
