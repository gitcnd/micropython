# Location of top-level MicroPython directory
MPY_DIR = ../..

# Name of module
MOD = cnd

# Source files (.c or .py)
SRC = cnd.c

# Architecture to build for (x86, x64, armv6m, armv7m, xtensa, xtensawin)
#ARCH = x64
# esp32:-
ARCH = xtensawin

CFLAGS_USERMOD += -I/home/cnd/Downloads/repos/micropython/ports/esp32/build-ESP32_GENERIC/config/
CFLAGS_USERMOD += -I/home/cnd/Downloads/repos/circuitpython/ports/espressif/esp-idf/components/esp_common/include/
CFLAGS_USERMOD += -I/home/cnd/Downloads/repos/circuitpython/ports/espressif/esp-idf/components/bootloader_support/bootloader_flash/include/

# Include to get the rules for compiling and linking the module
include $(MPY_DIR)/py/dynruntime.mk

# /home/cnd/Downloads/repos/micropython/ports/esp32/build-ESP32_GENERIC/config/sdkconfig.h

