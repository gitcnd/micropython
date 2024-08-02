#define MICROPY_HW_BOARD_NAME "FeatherS2"
#define MICROPY_HW_MCU_NAME "ESP32-S2"
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "FeatherS2"

#define MICROPY_PY_BLUETOOTH                (0)
#define MICROPY_HW_ENABLE_SDCARD            (0)

#define MICROPY_HW_I2C0_SCL (9)
#define MICROPY_HW_I2C0_SDA (8)

#define MICROPY_HW_SPI1_MOSI (35)  // SDO
#define MICROPY_HW_SPI1_MISO (37)  // SDI
#define MICROPY_HW_SPI1_SCK (36)

#define MICROPY_PY_DEFLATE_COMPRESS (1)
