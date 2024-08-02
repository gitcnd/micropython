// Include MicroPython API.
#include "py/runtime.h"

// Used to get the time in the Timer class example.
#include "py/mphal.h"

#include "esp_system.h"





#include <stddef.h>

#include <bootloader_flash_priv.h>
#include <esp_log.h>
#include <esp_flash_encrypt.h>
#include "sdkconfig.h"
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
    /* SPI flash controller */
#   define SPIFLASH SPI1
#   define SPI0     SPI0
#else
#   include "hal/spimem_flash_ll.h"
#   include "soc/spi_mem_struct.h"
#   include "soc/spi_mem_reg.h"
    /* SPI flash controller */
#   define SPIFLASH SPIMEM1
#   define SPI0     SPIMEM0
#endif

// This dependency will be removed in the future.  IDF-5025
#include "esp_flash.h"

#include "esp_rom_spiflash.h"






// This is the function which will be called from Python as updatefw.add_ints(a, b).
static mp_obj_t example_add_ints(mp_obj_t a_obj, mp_obj_t b_obj) {
    // Extract the ints from the micropython input objects.
    int a = mp_obj_get_int(a_obj);
    int b = mp_obj_get_int(b_obj);

    // Calculate the addition and convert to MicroPython object.
    return mp_obj_new_int(a + b);
}
// Define a Python reference to the function above.
static MP_DEFINE_CONST_FUN_OBJ_2(example_add_ints_obj, example_add_ints);



// This is the function which will be called from Python as updatefw.cnd_reset(a, b).
static mp_obj_t example_cnd_reset() {
    mp_int_t  reason = (mp_int_t)bootloader_execute_flash_command(CMD_RDID, 0, 0, 24); // esp_reset_reason();
    reason = ((reason & 0xff) << 16) | ((reason >> 16) & 0xff) | (reason & 0xff00); 
    return mp_obj_new_int(reason);
}
// Define a Python reference to the function above.
static MP_DEFINE_CONST_FUN_OBJ_0(example_cnd_reset_obj, example_cnd_reset);


/*

// This is the function which will be called from Python as updatefw.read_flashid(a, b).
static mp_obj_t example_read_flashid() {
    mp_int_t  id = (mp_int_t)esp_reset_reason();
    id = ((id & 0xff) << 16) | ((id >> 16) & 0xff) | (id & 0xff00);
    return mp_obj_new_int(id);
}
// Define a Python reference to the function above.
static MP_DEFINE_CONST_FUN_OBJ_0(example_read_flashid_obj, example_read_flashid);

*/


/*

static mp_obj_t example_read_flash_id() {
{
    uint32_t id = bootloader_execute_flash_command(CMD_RDID, 0, 0, 24);
    id = ((id & 0xff) << 16) | ((id >> 16) & 0xff) | (id & 0xff00);
    return mp_obj_new_int(id);
}
static MP_DEFINE_CONST_FUN_OBJ_0(example_read_flash_id_obj, example_read_flash_id);

*/


// This is the function which will be called from Python as updatefw.cnd_reset(a, b).
static mp_obj_t example_cnd_reset_asm() {
    mp_int_t  reason = (mp_int_t)esp_reset_reason();

    asm volatile (
        "addi %0, %0, 1\n"  // Add 1 to the value in the register corresponding to `reason`
        : "+r"(reason)      // Output operand: `reason` is both an input and output
    );

    /*
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
    */

    return mp_obj_new_int(reason);
}
// Define a Python reference to the function above.
static MP_DEFINE_CONST_FUN_OBJ_0(example_cnd_reset_asm_obj, example_cnd_reset_asm);




// This is the function which will be called from Python as updatefw.cnd_reset(a, b).
static mp_obj_t example_read_flash( mp_obj_t buffer_obj, mp_obj_t base_addr_obj, mp_obj_t start_read_obj ) {
    // Get a pointer to the buffer
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buffer_obj, &bufinfo, MP_BUFFER_WRITE);
    
    if (bufinfo.len < 4096) {
        mp_raise_ValueError("Buffer size must be at least 4096 bytes");
    }

    uint8_t *buffer = (uint8_t *)bufinfo.buf;
    uint32_t base_addr = mp_obj_get_int(base_addr_obj);
    uint32_t start_read = mp_obj_get_int(start_read_obj);



// This is a GPT-confused mess.  See https://www.espressif.com/sites/default/files/documentation/esp32-s2_technical_reference_manual_en.pdf?page=561 to fix it...
/*

    asm volatile (
        "addi a1, a1, -4\n"           // Make space on the stack for 1 register
        "s32i %1, a1, 0\n"            // Push base_addr

        //"movi a15, %2\n"              // Flash memory address to read from (start_read)
        "movi a14, 4096\n"            // Number of bytes to read
        "movi a13, 0x1000\n"          // Set data length in bytes (4096)
        "movi a12, 0x0B\n"            // Flash read command (0x0B for fast read)

        // Set up the SPI command
        "read_loop:\n"                // Named label for the start of the loop
        "s32i %2, %1, 0x0004\n"       // Write start address to SPI_ADDR_REG
        "s32i a13, %1, 0x0010\n"      // Write data length to SPI_CTRL2_REG
        "s32i a12, %1, 0x0008\n"      // Write flash read command to SPI_CMD_REG

        "l32i a12, %1, 0x000C\n"      // Load SPI_RD_STATUS_REG
        "movi a11, 1\n"               // Move immediate value 1 to a11
        "and a11, a12, a11\n"         // Mask to the read command done bit
        "bnez a11, read_loop\n"       // Wait until the read command is done

        // Read data from SPI_W0_REG to SPI_W15_REG
        "l32i a12, %1, 0x0050\n"      // Load data from SPI_W0_REG
        "s32i a12, %0, 0\n"           // Store the word into the buffer
        "addi %0, %0, 4\n"            // Increment the buffer pointer by 4
        "addi a14, a14, -4\n"         // Decrement the number of bytes left to read by 4
        "bnez a14, read_loop\n"       // Repeat until all bytes are read

        "l32i %1, a1, 0\n"            // Pop base_addr
        "addi a1, a1, 4\n"            // Restore stack pointer
        : "+r"(buffer)                // Output operand
        : "r"(base_addr), "r"(start_read)  // Input operands
        : "a11", "a12", "a13", "a14"
    );

*/

/*

// SPI_ADDR_REG, SPI_CTRL2_REG, SPI_CMD_REG, SPI_RD_STATUS_REG, SPI_W0_REG

    asm volatile (
        "addi a1, a1, -4\n"           // Make space on the stack for 1 register
        "s32i %1, a1, 0\n"            // Push base_addr

        //"movi a15, %2\n"              // Flash memory address to read from (start_read)
        "movi a14, 4096\n"            // Number of bytes to read
        "movi a13, 0x1000\n"          // Set data length in bytes (4096)
        "movi a12, 0x0B\n"            // Flash read command (0x0B for fast read)

        // Set up the SPI command
        "read_loop:\n"                // Named label for the start of the loop
        "s32i %2, %1, 0x0004\n"       // Write start address to SPI_ADDR_REG
        "s32i a13, %1, 0x0010\n"      // Write data length to SPI_CTRL2_REG
        "s32i a12, %1, 0x0008\n"      // Write flash read command to SPI_CMD_REG

        "l32i a12, %1, 0x000C\n"      // Load SPI_RD_STATUS_REG
        "movi a11, 1\n"               // Move immediate value 1 to a11
        "and a11, a12, a11\n"         // Mask to the read command done bit
        "bnez a11, read_loop\n"       // Wait until the read command is done

        // Read data from SPI_W0_REG to SPI_W15_REG
        "l32i a12, %1, 0x0050\n"      // Load data from SPI_W0_REG
        "s32i a12, %0, 0\n"           // Store the word into the buffer
        "addi %0, %0, 4\n"            // Increment the buffer pointer by 4
        "addi a14, a14, -4\n"         // Decrement the number of bytes left to read by 4
        "bnez a14, read_loop\n"       // Repeat until all bytes are read

        "l32i %1, a1, 0\n"            // Pop base_addr
        "addi a1, a1, 4\n"            // Restore stack pointer
        : "+r"(buffer)                // Output operand
        : "r"(base_addr), "r"(start_read)  // Input operands
        : "a11", "a12", "a13", "a14"
    );




    asm volatile (
        "addi a1, a1, -4\n"           // Make space on the stack for 1 register
        "s32i %1, a1, 0\n"            // Push base_addr

        //"movi a15, %2\n"              // Flash memory address to read from (start_read)
        "movi a14, 4096\n"            // Number of bytes to read
        "movi a13, 0x0B\n"            // Flash read command (0x0B for fast read)

        // Set up the SPI command
        "s32i %2, %1, 0x0004\n"      // Write start address to SPI_ADDR_REG (0x0004)
        "s32i a14, %1, 0x0010\n"      // Write data length to SPI_CTRL2_REG (0x0010)
        "s32i a13, %1, 0x0000\n"      // Write flash read command to SPI_CMD_REG (0x0000)

        // Set up DMA
        "movi a11, 0x3FF40000\n"      // DMA base address
        "movi a10, 0x0001\n"          // Enable DMA channel
        "s32i a10, a11, 0x004\n"      // DMA channel config register

        "movi a10, %0\n"              // DMA buffer address
        "s32i a10, a11, 0x008\n"      // DMA buffer address register

        "movi a10, 4096\n"            // DMA transfer size
        "s32i a10, a11, 0x00C\n"      // DMA transfer size register

        // Start DMA transfer
        "movi a10, 0x0001\n"
        "s32i a10, a11, 0x000\n"      // DMA control register

        // Wait for DMA transfer to complete
        "dma_wait:\n"
        "l32i a10, a11, 0x010\n"      // DMA status register
        "movi a9, 0x0001\n"
        "and a10, a10, a9\n"
        "bnez a10, dma_wait\n"

        // Restore stack pointer
        "l32i %1, a1, 0\n"            // Pop base_addr
        "addi a1, a1, 4\n"            // Restore stack pointer
        : "+r"(buffer)                // Output operand
        : "r"(base_addr), "r"(start_read)  // Input operands
        : "a9", "a10", "a11", "a13", "a14"
    );


    asm volatile (
        "addi a1, a1, -8\n"          // Make space on the stack for 1 register
        "s32i %1, a1, 0\n"           // Push 1
        "s32i %2, a1, 4\n"           // Push 2

        "movi a15, %2\n"             // Flash memory address to read from (start_read)
        "movi a14, 4096\n"           // Number of bytes to read
        "movi a13, 0x1000\n"         // Set data length in bytes (4096)
        "movi a12, 0x0B\n"           // Flash read command (0x0B for fast read)

        // Set up the SPI command
        "s32i a13, %1, 0x004\n"      // Write data length to SPI_CTRL_REG
        "s32i a12, %1, 0x008\n"      // Write flash read command to SPI_CMD_REG

        "read_loop:\n"               // Named label for the start of the loop
        "l32i a12, %1, 0x00C\n"      // Load SPI_RD_STATUS_REG
        "movi a11, 1\n"              // Move immediate value 1 to register a11
        "and a11, a12, a11\n"        // Mask to the read command done bit
        "bnez a11, read_loop\n"      // Wait until the read command is done

        // Read data from SPI_W0_REG to SPI_W15_REG
        "l32i a12, %1, 0x0050\n"     // Load data from SPI_W0_REG
        "s32i a12, %0, 0\n"          // Store the word into the buffer
        "addi %0, %0, 4\n"           // Increment the buffer pointer by 4
        "addi a15, a15, 4\n"         // Increment the flash memory address by 4
        "addi a14, a14, -4\n"        // Decrement the number of bytes left to read by 4
        "bnez a14, read_loop\n"      // Repeat until all bytes are read

        "l32i %2, a1, 4\n"           // Pop 2
        "l32i %1, a1, 0\n"           // Pop 1
        "addi a1, a1, 8\n"           // Restore stack pointer
        : "+r"(buffer)               // Output operand
        : "r"(base_addr), "r"(start_read)  // Input operands
        : "a10", "a11", "a12", "a13", "a14", "a15"
    );




    // broke
    asm volatile (
        "addi a1, a1, -4\n"    // Make space on the stack for 1 registers
        "s32i %1, a1, 0\n"      // Push a2

        //"movi a2, %1\n"   // Base address of SPI0
        "movi a3, %2\n"            // Flash memory address to read from (0x0)
        "movi a4, 4096\n"         // Number of bytes to read
        "movi a5, 0x1000\n"       // Set data length in bytes (4096)
        "movi a6, 0x0B\n"         // Flash read command (0x0B for fast read)

        // Set up the SPI command
        "s32i a5, %1, 0x004\n"    // Write data length to SPI_CTRL_REG
        "s32i a6, %1, 0x008\n"    // Write flash read command to SPI_CMD_REG

        "read_loop:\n"            // Named label for the start of the loop
        "l32i a6, %1, 0x00C\n"    // Load SPI_RD_STATUS_REG
        "movi a11, 1\n"            // Move immediate value 1 to register a11
        "and a11, a6, a11\n"        // Mask to the read command done bit
        "bnez a11, read_loop\n"    // Wait until the read command is done

        // Read data from SPI_W0_REG to SPI_W15_REG
        "l32i a6, %1, 0x0050\n"   // Load data from SPI_W0_REG
        "s32i a6, %0, 0\n"        // Store the word into the buffer
        "addi %0, %0, 4\n"        // Increment the buffer pointer by 4
        "addi a3, a3, 4\n"        // Increment the flash memory address by 4
        "addi a4, a4, -4\n"       // Decrement the number of bytes left to read by 4
        "bnez a4, read_loop\n"    // Repeat until all bytes are read

        "l32i %1, a1, 0\n"      // Pop a2
        "addi a1, a1, 4\n"     // Restore stack pointer
        : "+r"(buffer)             // Output operand
        : "r"(base_addr), "r"(start_read)          // Input operand
        : "a2", "a3", "a4", "a5", "a6", "a11"
    );


    // works (does not crash)
    asm volatile (
        "movi a2, 0x60002000\n"   // Base address of SPI0
        "movi a3, 0\n"            // Flash memory address to read from (0x0)
        "movi a4, 4096\n"         // Number of bytes to read
        "movi a5, 0x1000\n"       // Set data length in bytes (4096)
        "movi a6, 0x0B\n"         // Flash read command (0x0B for fast read)

        // Set up the SPI command
        "s32i a5, a2, 0x004\n"    // Write data length to SPI_CTRL_REG
        "s32i a6, a2, 0x008\n"    // Write flash read command to SPI_CMD_REG

        "read_loop:\n"            // Named label for the start of the loop
        "l32i a6, a2, 0x00C\n"    // Load SPI_RD_STATUS_REG
        "movi a8, 1\n"            // Move immediate value 1 to register a8
        "and a8, a6, a8\n"        // Mask to the read command done bit
        "bnez a8, read_loop\n"    // Wait until the read command is done

        // Read data from SPI_W0_REG to SPI_W15_REG
        "l32i a6, a2, 0x0050\n"   // Load data from SPI_W0_REG
        "s32i a6, %0, 0\n"        // Store the word into the buffer
        "addi %0, %0, 4\n"        // Increment the buffer pointer by 4
        "addi a3, a3, 4\n"        // Increment the flash memory address by 4
        "addi a4, a4, -4\n"       // Decrement the number of bytes left to read by 4
        "bnez a4, read_loop\n"    // Repeat until all bytes are read

        :
        : "r"(buffer)
        : "a2", "a3", "a4", "a5", "a6", "a8"
    );



    asm volatile (
        "addi a1, a1, -4\n"    // Make space on the stack for 1 registers
        "s32i %1, a1, 0\n"      // Push a2
        "movi a3, 0\n"            // Flash memory address to read from (0x0)
        "movi a4, 4096\n"         // Number of bytes to read

        "read_loop:\n"            // Named label for the start of the loop
        "l32i a6, %1, 0x004\n"    // Read SPI_CTRL_REG to set the read length
        "or a6, a6, a4\n"         // Set the length of bytes to read
        "s32i a6, %1, 0x004\n"    // Write back to SPI_CTRL_REG

        "movi a6, 0x8000\n"       // Set the flash read command (0x8000)
        "s32i a6, %1, 0x008\n"    // Write the command to SPI_CMD_REG

        "wait_read_done:\n"
        "l32i a6, %1, 0x00C\n"    // Load SPI_RD_STATUS_REG
        "bbci a6, 0, wait_read_done\n" // Wait until the read command is done

        // Read data from SPI_W0_REG to SPI_W15_REG
        "l32i a6, %1, 0x0050\n"   // Load data from SPI_W0_REG
        "s32i a6, %0, 0\n"        // Store the word into the buffer
        "addi %0, %0, 4\n"        // Increment the buffer pointer by 4
        "addi a3, a3, 4\n"        // Increment the flash memory address by 4
        "addi a4, a4, -4\n"       // Decrement the number of bytes left to read by 4
        "bnez a4, read_loop\n"    // Repeat until all bytes are read

        "l32i %1, a1, 0\n"      // Pop a2
        "addi a1, a1, 4\n"     // Restore stack pointer

        : "+r"(buffer)             // Output operand
        : "r"(base_addr)           // Input operand
        : "a3", "a4", "a6", "a8"

    );


 
    asm volatile (
        "movi a2, %1\n"           // Base address of SPI0
        "movi a2, 0x3F403000\n"   // Base address of SPI0 (PeriBUS1)
        "movi a3, 0\n"            // Flash memory address to read from (0x0)
        "movi a4, 4096\n"         // Number of bytes to read

        "read_loop:\n"            // Named label for the start of the loop
        "l32i a6, a2, 0x0000\n"   // Load SPI_CMD_REG
        "movi a8, 1\n"            // Move immediate value 1 to register a8
        "or a6, a6, a8\n"         // Set the LSB to issue the read command
        "s32i a6, a2, 0x0000\n"   // Write back to SPI_CMD_REG

        "wait_read_done:\n"
        "l32i a6, a2, 0x000C\n"   // Load SPI_CTRL1_REG (status register)
        "slli a8, a6, 31\n"       // Shift left logical to move LSB to MSB
        "srai a8, a8, 31\n"       // Shift right arithmetic to restore LSB to position
        "bnez a8, wait_read_done\n" // Wait until the read command is done

        // Read data from SPI_W0_REG to SPI_W15_REG
        "l32i a6, a2, 0x0098\n"   // Load data from SPI_W0_REG
        "s32i a6, %0, 0\n"        // Store the word into the buffer
        "addi %0, %0, 4\n"        // Increment the buffer pointer by 4
        "addi a3, a3, 4\n"        // Increment the flash memory address by 4
        "addi a4, a4, -4\n"       // Decrement the number of bytes left to read by 4
        "bnez a4, read_loop\n"    // Repeat until all bytes are read

        : "+r"(buffer)             // Output operand
        : "r"(base_addr)           // Input operand
        : "a2", "a3", "a4", "a6", "a8"
    );




    asm volatile (
        "movi a2, 0x3F403000\n"   // Base address of SPI0 (PeriBUS1)
        "movi a3, 0\n"            // Flash memory address to read from (0x0)
        "movi a4, 4096\n"         // Number of bytes to read

        "read_loop:\n"            // Named label for the start of the loop
        "l32i a6, a2, 0x0000\n"   // Load SPI_CMD_REG
        "movi a8, 1\n"            // Move immediate value 1 to register a8
        "or a6, a6, a8\n"         // Set the LSB to issue the read command
        "s32i a6, a2, 0x0000\n"   // Write back to SPI_CMD_REG

        "wait_read_done:\n"
        "l32i a6, a2, 0x000C\n"   // Load SPI_CTRL1_REG (status register)
        "slli a8, a6, 31\n"       // Shift left logical to move LSB to MSB
        "srai a8, a8, 31\n"       // Shift right arithmetic to restore LSB to position
        "bnez a8, wait_read_done\n" // Wait until the read command is done

        // Read data from SPI_W0_REG to SPI_W15_REG
        "l32i a6, a2, 0x0098\n"   // Load data from SPI_W0_REG
        "s32i a6, %0, 0\n"        // Store the word into the buffer
        "addi %0, %0, 4\n"        // Increment the buffer pointer by 4
        "addi a3, a3, 4\n"        // Increment the flash memory address by 4
        "addi a4, a4, -4\n"       // Decrement the number of bytes left to read by 4
        "bnez a4, read_loop\n"    // Repeat until all bytes are read

        :
        : "r"(buffer)
        : "a2", "a3", "a4", "a6", "a8"
    );

// crashes:-

    asm volatile (
        "movi a3, 0\n"            // Flash memory address to read from (0x0)
        "movi a4, 4096\n"         // Number of bytes to read

        "read_loop:\n"            // Named label for the start of the loop
        "l32i a6, %1, 0x004\n"    // Read SPI_CTRL_REG to set the read length
        "or a6, a6, a4\n"         // Set the length of bytes to read
        "s32i a6, %1, 0x004\n"    // Write back to SPI_CTRL_REG

        "movi a6, 0x8000\n"       // Set the flash read command (0x8000)
        "s32i a6, %1, 0x008\n"    // Write the command to SPI_CMD_REG

        "wait_read_done:\n"
        "l32i a6, %1, 0x00C\n"    // Load SPI_RD_STATUS_REG
        "bbci a6, 0, wait_read_done\n" // Wait until the read command is done

        // Read data from SPI_W0_REG to SPI_W15_REG
        "l32i a6, %1, 0x0050\n"   // Load data from SPI_W0_REG
        "s32i a6, %0, 0\n"        // Store the word into the buffer
        "addi %0, %0, 4\n"        // Increment the buffer pointer by 4
        "addi a3, a3, 4\n"        // Increment the flash memory address by 4
        "addi a4, a4, -4\n"       // Decrement the number of bytes left to read by 4
        "bnez a4, read_loop\n"    // Repeat until all bytes are read

        : "+r"(buffer)             // Output operand
        : "r"(base_addr)           // Input operand
        : "a3", "a4", "a6", "a8"

    );







    // Perform the flash read operation
    asm volatile (
        "movi a2, 0x60002000\n"   // Base address of SPI0
        "movi a3, 0\n"            // Flash memory address to read from (0x0)
        "movi a4, 4096\n"         // Number of bytes to read
        "movi a5, 0x1000\n"       // Set data length in bytes (4096)
        "movi a6, 0x0B\n"         // Flash read command (0x0B for fast read)

        // Set up the SPI command
        "s32i a5, a2, 0x004\n"    // Write data length to SPI_CTRL_REG
        "s32i a6, a2, 0x008\n"    // Write flash read command to SPI_CMD_REG

        "read_loop:\n"            // Named label for the start of the loop
        "l32i a6, a2, 0x00C\n"    // Load SPI_RD_STATUS_REG
        "movi a8, 1\n"            // Move immediate value 1 to register a8
        "and a8, a6, a8\n"        // Mask to the read command done bit
        "bnez a8, read_loop\n"    // Wait until the read command is done

        // Read data from SPI_W0_REG to SPI_W15_REG
        "l32i a6, a2, 0x0050\n"   // Load data from SPI_W0_REG
        "s32i a6, %0, 0\n"        // Store the word into the buffer
        "addi %0, %0, 4\n"        // Increment the buffer pointer by 4
        "addi a3, a3, 4\n"        // Increment the flash memory address by 4
        "addi a4, a4, -4\n"       // Decrement the number of bytes left to read by 4
        "bnez a4, read_loop\n"    // Repeat until all bytes are read

        :
        : "r"(buffer)
        : "a2", "a3", "a4", "a5", "a6", "a8"
    );



    asm volatile (

        "movi a2, 0x60002000\n"   // Base address of SPI0
        "movi a3, 0\n"            // Flash memory address to read from (0x0)
        "movi a4, 4096\n"         // Number of bytes to read

        "read_loop:\n"            // Named label for the start of the loop
        "l32i a6, a2, 0x0008\n"   // Load SPI_CMD_REG
        "movi a8, 1\n"            // Move immediate value 1 to register a8
        "or a6, a6, a8\n"         // Set the LSB to issue the read command
        "s32i a6, a2, 8\n"        // Write back to SPI_CMD_REG

        "wait_read_done:\n"
        "l32i a6, a2, 0x000C\n"   // Load SPI_RD_STATUS_REG
        "slli a8, a6, 31\n"       // Shift left logical to move LSB to MSB
        "srai a8, a8, 31\n"       // Shift right arithmetic to restore LSB to position
        "bnez a8, wait_read_done\n" // Wait until the read command is done

        "l32i a6, a2, 0x0050\n"   // Load SPI_W0_REG
        "s32i a6, %0, 0\n"        // Store the word into the buffer
        "addi %0, %0, 4\n"        // Increment the buffer pointer by 4
        "addi a3, a3, 4\n"        // Increment the flash memory address by 4
        "addi a4, a4, -4\n"       // Decrement the number of bytes left to read by 4
        "bnez a4, read_loop\n"    // Repeat until all bytes are read

        :
        : "r"(buffer)
        : "a2", "a3", "a4", "a5", "a6", "a8"
    );

*/


    return mp_obj_new_int(base_addr);
}
// Define a Python reference to the function above.
static MP_DEFINE_CONST_FUN_OBJ_3(example_read_flash_obj, example_read_flash);



// This structure represents Timer instance objects.
typedef struct _example_Timer_obj_t {
    // All objects start with the base.
    mp_obj_base_t base;
    // Everything below can be thought of as instance attributes, but they
    // cannot be accessed by MicroPython code directly. In this example we
    // store the time at which the object was created.
    mp_uint_t start_time;
} example_Timer_obj_t;

// This is the Timer.time() method. After creating a Timer object, this
// can be called to get the time elapsed since creating the Timer.
static mp_obj_t example_Timer_time(mp_obj_t self_in) {
    // The first argument is self. It is cast to the *example_Timer_obj_t
    // type so we can read its attributes.
    example_Timer_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Get the elapsed time and return it as a MicroPython integer.
    mp_uint_t elapsed = mp_hal_ticks_ms() - self->start_time;
    return mp_obj_new_int_from_uint(elapsed);
}
static MP_DEFINE_CONST_FUN_OBJ_1(example_Timer_time_obj, example_Timer_time);

// This represents Timer.__new__ and Timer.__init__, which is called when
// the user instantiates a Timer object.
static mp_obj_t example_Timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // Allocates the new object and sets the type.
    example_Timer_obj_t *self = mp_obj_malloc(example_Timer_obj_t, type);

    // Initializes the time for this Timer instance.
    self->start_time = mp_hal_ticks_ms();

    // The make_new function always returns self.
    return MP_OBJ_FROM_PTR(self);
}

// This collects all methods and other static class attributes of the Timer.
// The table structure is similar to the module table, as detailed below.
static const mp_rom_map_elem_t example_Timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_time), MP_ROM_PTR(&example_Timer_time_obj) },
};
static MP_DEFINE_CONST_DICT(example_Timer_locals_dict, example_Timer_locals_dict_table);

// This defines the type(Timer) object.
MP_DEFINE_CONST_OBJ_TYPE(
    example_type_Timer,
    MP_QSTR_Timer,
    MP_TYPE_FLAG_NONE,
    make_new, example_Timer_make_new,
    locals_dict, &example_Timer_locals_dict
    );

// Define all attributes of the module.
// Table entries are key/value pairs of the attribute name (a string)
// and the MicroPython object reference.
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
static const mp_rom_map_elem_t example_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_updatefw) },
    { MP_ROM_QSTR(MP_QSTR_add_ints), MP_ROM_PTR(&example_add_ints_obj) },
    { MP_ROM_QSTR(MP_QSTR_cnd_reset), MP_ROM_PTR(&example_cnd_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_cnd_reset_asm), MP_ROM_PTR(&example_cnd_reset_asm_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_flash), MP_ROM_PTR(&example_read_flash_obj) },
    { MP_ROM_QSTR(MP_QSTR_Timer),    MP_ROM_PTR(&example_type_Timer) },
};
static MP_DEFINE_CONST_DICT(example_module_globals, example_module_globals_table);

// Define module object.
const mp_obj_module_t updatefw_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&example_module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_updatefw, updatefw_user_cmodule);
