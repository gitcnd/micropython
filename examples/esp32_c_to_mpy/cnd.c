// Include the header file to get access to the MicroPython API
#include "py/dynruntime.h"

//#include "/home/cnd/Downloads/repos/micropython/ports/esp32/build-ESP32_GENERIC/config/sdkconfig.h"
//#include "/home/cnd/Downloads/repos/circuitpython/ports/espressif/esp-idf/components/esp_common/include/esp_err.h"
//#include "/home/cnd/Downloads/repos/circuitpython/ports/espressif/esp-idf/components/bootloader_support/bootloader_flash/include/bootloader_flash.h"

#include "sdkconfig.h"
#include "esp_err.h"
#include "bootloader_flash.h"
#include "bootloader_common.h"


// This is the function which will be called from Python as cnd.reset_reason(a, b).
static mp_obj_t reset_reason() {
    mp_int_t  reason = (mp_int_t)bootloader_execute_flash_command(CMD_RDID, 0, 0, 24); // esp_reset_reason();
    reason = ((reason & 0xff) << 16) | ((reason >> 16) & 0xff) | (reason & 0xff00); 
    return mp_obj_new_int(reason);
}
// Define a Python reference to the function above.
static MP_DEFINE_CONST_FUN_OBJ_0(reset_reason_obj, reset_reason);


// This is the entry point and is called when the module is imported
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    // This must be first, it sets up the globals dict and other things
    MP_DYNRUNTIME_INIT_ENTRY

    // Make the function available in the module's namespace
    mp_store_global(MP_QSTR_reset_reason, MP_OBJ_FROM_PTR(&reset_reason_obj));

    // This must be last, it restores the globals dict
    MP_DYNRUNTIME_INIT_EXIT
}
