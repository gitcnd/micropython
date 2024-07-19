#define MICROPY_HW_BOARD_NAME               "LOLIN_C3_MINI_FAT"
#define MICROPY_HW_MCU_NAME                 "ESP32-C3FH4"
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "mpy-c3-mini"

#define MICROPY_HW_ENABLE_SDCARD            (0)
#define MICROPY_PY_MACHINE_I2S              (0)

#define MICROPY_HW_I2C0_SCL                 (10)
#define MICROPY_HW_I2C0_SDA                 (8)

#define MICROPY_HW_SPI1_MOSI                (4)
#define MICROPY_HW_SPI1_MISO                (3)
#define MICROPY_HW_SPI1_SCK                 (2)

#define MICROPY_VFS_LFS2       (0)

#ifndef MICROPY_VFS_FAT
#define MICROPY_VFS_FAT        (1)
#endif
#define MICROPY_VFS_FAT_LABEL  "flash"
#define MICROPY_VFS_FAT_MOUNT_POINT "/"
#define MICROPY_FATFS_ENABLE_LFN (1)
#define MICROPY_FATFS_RPATH    (2)
#define MICROPY_FATFS_NORTC    (0) // Ensure RTC is enabled if timestamps are needed
