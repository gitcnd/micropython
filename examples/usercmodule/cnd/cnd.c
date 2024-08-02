// Include MicroPython API.
#include "py/runtime.h"

// Used to get the time in the Timer class example.
#include "py/mphal.h"

#include "esp_system.h"

/*

#include "sdkconfig.h"



#include <stddef.h>

#include <bootloader_flash_priv.h>
#include <esp_log.h>
#include <esp_flash_encrypt.h>
#include "soc/soc_caps.h"
#include "hal/efuse_ll.h"
#include "hal/efuse_hal.h"

#ifndef BOOTLOADER_BUILD
#include "spi_flash_mmap.h"
#endif
#include "hal/spi_flash_ll.h"
#include "rom/spi_flash.h"
#if CONFIG_IDF_TARGET_ESP32
#   include "soc/spi_struct.h"
#   include "soc/spi_reg.h"
    // SPI flash controller 
#   define SPIFLASH SPI1
#   define SPI0     SPI0
#else
#   include "hal/spimem_flash_ll.h"
#   include "soc/spi_mem_struct.h"
#   include "soc/spi_mem_reg.h"
    // SPI flash controller 
#   define SPIFLASH SPIMEM1
#   define SPI0     SPIMEM0
#endif



// This dependency will be removed in the future.  IDF-5025
#include "esp_flash.h"

#include "esp_rom_spiflash.h"


*/

#include "esp_sleep.h" // for esp_sleep_get_wakeup_cause

#include "soc/rtc_cntl_reg.h" // for WRITE_PERI_REG
#include "soc/rtc_cntl_struct.h" // for RTC_CNTL_BROWN_OUT_REG





// This is the function which will be called from Python as cnd.reset_reason(a, b).
static mp_obj_t example_reset_reason() {

    mp_int_t  reason = (mp_int_t)esp_reset_reason();
    return mp_obj_new_int(reason);

    // mp_int_t  reason = (mp_int_t)bootloader_execute_flash_command(CMD_RDID, 0, 0, 24); // esp_reset_reason();
    //mp_int_t  reason = esp_reset_reason();
    //reason = ((reason & 0xff) << 16) | ((reason >> 16) & 0xff) | (reason & 0xff00); 
    //return mp_obj_new_int(reason);
}
// Define a Python reference to the function above.
static MP_DEFINE_CONST_FUN_OBJ_0(example_reset_reason_obj, example_reset_reason);


static mp_obj_t example_wake_reason() {
    mp_int_t  reason = (mp_int_t)esp_sleep_get_wakeup_cause();
    return mp_obj_new_int(reason);
    // case ESP_SLEEP_WAKEUP_UNDEFINED:    Serial.println("In case of deep sleep: reset was not caused by exit from deep sleep"); break;
    // case ESP_SLEEP_WAKEUP_ALL:          Serial.println("Not a wakeup cause: used to disable all wakeup sources with esp_sleep_disable_wakeup_source"); break;
    // case ESP_SLEEP_WAKEUP_EXT0:         Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    // case ESP_SLEEP_WAKEUP_EXT1:         Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    // case ESP_SLEEP_WAKEUP_TIMER:        Serial.println("Wakeup caused by timer"); break;
    // case ESP_SLEEP_WAKEUP_TOUCHPAD:     Serial.println("Wakeup caused by touchpad"); break;
    // case ESP_SLEEP_WAKEUP_ULP:          Serial.println("Wakeup caused by ULP program"); break;
    // case ESP_SLEEP_WAKEUP_GPIO:         Serial.println("Wakeup caused by GPIO (light sleep only)"); break;
    // case ESP_SLEEP_WAKEUP_UART:         Serial.println("Wakeup caused by UART (light sleep only)"); break;
}
static MP_DEFINE_CONST_FUN_OBJ_0(example_wake_reason_obj, example_wake_reason);


static mp_obj_t example_brown_out_reg(mp_obj_t a_obj) {
    int a = mp_obj_get_int(a_obj);
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, a); //disable brownout detector   
    return mp_obj_new_int(a);
}
static MP_DEFINE_CONST_FUN_OBJ_1(example_brown_out_reg_obj, example_brown_out_reg);







/*
// This is the function which will be called from Python as cnd.reset_reason(a, b).
static mp_obj_t example_reset_reason_asm() {
    mp_int_t  reason = (mp_int_t)esp_reset_reason();


    asm volatile (
        "addi a1, a1, -16\n"    // Make space on the stack for 4 registers
        "s32i a2, a1, 0\n"      // Push a2
        "s32i a3, a1, 4\n"      // Push a3
        "s32i a4, a1, 8\n"      // Push a4
        "s32i a5, a1, 12\n"     // Push a5

        "movi a2, 0x60002000\n"   // Base address of SPI0
        "addi %0, %0, 1\n"  // Add 1 to the value in the register corresponding to `reason`

        "l32i a2, a1, 0\n"      // Pop a2
        "l32i a3, a1, 4\n"      // Pop a3
        "l32i a4, a1, 8\n"      // Pop a4
        "l32i a5, a1, 12\n"     // Pop a5
        "addi a1, a1, 16\n"     // Restore stack pointer
 	
        : "+r"(reason)      // Output operand: `reason` is both an input and output
    );

    return mp_obj_new_int(reason);
}
// Define a Python reference to the function above.
static MP_DEFINE_CONST_FUN_OBJ_0(example_reset_reason_asm_obj, example_reset_reason_asm);

*/

// Define all attributes of the module.
// Table entries are key/value pairs of the attribute name (a string)
// and the MicroPython object reference.
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
static const mp_rom_map_elem_t example_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_cnd) },
    { MP_ROM_QSTR(MP_QSTR_brown_out_reg), MP_ROM_PTR(&example_brown_out_reg_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset_reason), MP_ROM_PTR(&example_reset_reason_obj) },
    { MP_ROM_QSTR(MP_QSTR_wake_reason), MP_ROM_PTR(&example_wake_reason_obj) },
};
static MP_DEFINE_CONST_DICT(example_module_globals, example_module_globals_table);

// Define module object.
const mp_obj_module_t cnd_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&example_module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_cnd, cnd_user_cmodule);
