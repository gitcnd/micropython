// Both of these can be set by mpconfigboard.cmake if a BOARD_VARIANT is
// specified.

#ifndef MICROPY_HW_BOARD_NAME
#define MICROPY_HW_BOARD_NAME "Generic ESP32 module FAT"
#endif

#ifndef MICROPY_HW_MCU_NAME
#define MICROPY_HW_MCU_NAME "ESP32"
#endif

#define MICROPY_VFS_LFS2       (0)

#ifndef MICROPY_VFS_FAT
#define MICROPY_VFS_FAT        (1)
#endif
#define MICROPY_VFS_FAT_LABEL  "flash"
#define MICROPY_VFS_FAT_MOUNT_POINT "/"
#define MICROPY_FATFS_ENABLE_LFN (1)
#define MICROPY_FATFS_RPATH    (2)
#define MICROPY_FATFS_NORTC    (0) // Ensure RTC is enabled if timestamps are needed
