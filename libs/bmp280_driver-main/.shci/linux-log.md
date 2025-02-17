
# bmp280_driver

![linux-badge](linux-exit_code.svg)

## [linux build logs:](https://github.com/mrsinho/shci)

        

Build ran for `105.37s`

---

```bash
sudo apt install -y cmake gcc-arm-none-eabi make
```

```bash
Reading package lists...
Building dependency tree...
Reading state information...
cmake is already the newest version (3.18.4-2+deb11u1).
gcc-arm-none-eabi is already the newest version (15:8-2019-q3-1+b1).
make is already the newest version (4.3-4.1).
0 upgraded, 0 newly installed, 0 to remove and 11 not upgraded.

```

---

    
```bash
cd bmp280_driver
mkdir build
cd build 
cmake -DPICO_SDK_PATH="/pico-sdk" -DBMP280_DRIVER_BUILD_EXAMPLE=ON ..
cmake --build .
```

```bash
-- Defaulting build type to 'Release' since not specified.
-- The C compiler identification is GNU 8.3.1
-- The CXX compiler identification is GNU 8.3.1
-- The ASM compiler identification is GNU
-- Found assembler: /usr/bin/arm-none-eabi-gcc
-- Found Python3: /usr/bin/python3.9 (found version "3.9.2") found components: Interpreter 
-- Configuring done
-- Generating done
-- Build files have been written to: /mnt/c/Users/sino3/Desktop/shci-test/bmp280_driver/bmp280_driver/build
Scanning dependencies of target ELF2UF2Build
[  1%] Creating directories for 'ELF2UF2Build'
[  1%] No download step for 'ELF2UF2Build'
[  2%] No update step for 'ELF2UF2Build'
[  3%] No patch step for 'ELF2UF2Build'
[  3%] Performing configure step for 'ELF2UF2Build'
-- The C compiler identification is GNU 10.2.1
-- The CXX compiler identification is GNU 10.2.1
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done
-- Generating done
-- Build files have been written to: /mnt/c/Users/sino3/Desktop/shci-test/bmp280_driver/bmp280_driver/build/elf2uf2
[  4%] Performing build step for 'ELF2UF2Build'
Scanning dependencies of target elf2uf2
[ 50%] Building CXX object CMakeFiles/elf2uf2.dir/main.cpp.o
[100%] Linking CXX executable elf2uf2
[100%] Built target elf2uf2
[  5%] No install step for 'ELF2UF2Build'
[  5%] Completed 'ELF2UF2Build'
[  5%] Built target ELF2UF2Build
Scanning dependencies of target bs2_default
[  5%] Building ASM object pico-sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default.dir/compile_time_choice.S.obj
[  6%] Linking ASM executable bs2_default.elf
[  6%] Built target bs2_default
Scanning dependencies of target bs2_default_padded_checksummed_asm
[  7%] Generating bs2_default.bin
[  7%] Generating bs2_default_padded_checksummed.S
[  7%] Built target bs2_default_padded_checksummed_asm
Scanning dependencies of target bmp280_driver
[  7%] Building C object CMakeFiles/bmp280_driver.dir/bmp280_driver/src/bmp280_driver.c.obj
[  8%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/pico_stdlib/stdlib.c.obj
[  9%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/hardware_gpio/gpio.c.obj
[ 10%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/hardware_claim/claim.c.obj
[ 10%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/pico_platform/platform.c.obj
[ 11%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/hardware_sync/sync.c.obj
[ 12%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/hardware_irq/irq.c.obj
[ 12%] Building ASM object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/hardware_irq/irq_handler_chain.S.obj
[ 13%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/common/pico_sync/sem.c.obj
[ 14%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/common/pico_sync/lock_core.c.obj
[ 15%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/common/pico_time/time.c.obj
[ 15%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/common/pico_time/timeout_helper.c.obj
[ 16%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/hardware_timer/timer.c.obj
[ 17%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/common/pico_util/datetime.c.obj
[ 17%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/common/pico_util/pheap.c.obj
[ 18%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/common/pico_util/queue.c.obj
[ 19%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/common/pico_sync/mutex.c.obj
[ 19%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/common/pico_sync/critical_section.c.obj
[ 20%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/hardware_uart/uart.c.obj
[ 21%] Building ASM object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/hardware_divider/divider.S.obj
[ 22%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/pico_runtime/runtime.c.obj
[ 22%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/hardware_clocks/clocks.c.obj
[ 23%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/hardware_pll/pll.c.obj
[ 24%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/hardware_vreg/vreg.c.obj
[ 24%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/hardware_watchdog/watchdog.c.obj
[ 25%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/hardware_xosc/xosc.c.obj
[ 26%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/pico_printf/printf.c.obj
[ 26%] Building ASM object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/pico_bit_ops/bit_ops_aeabi.S.obj
[ 27%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/pico_bootrom/bootrom.c.obj
[ 28%] Building ASM object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/pico_divider/divider.S.obj
[ 29%] Building ASM object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/pico_double/double_aeabi.S.obj
[ 29%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/pico_double/double_init_rom.c.obj
[ 30%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/pico_double/double_math.c.obj
[ 31%] Building ASM object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/pico_double/double_v1_rom_shim.S.obj
[ 31%] Building ASM object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/pico_int64_ops/pico_int64_ops_aeabi.S.obj
[ 32%] Building ASM object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/pico_float/float_aeabi.S.obj
[ 34%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/pico_float/float_init_rom.c.obj
[ 35%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/pico_float/float_math.c.obj
[ 35%] Building ASM object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/pico_float/float_v1_rom_shim.S.obj
[ 36%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/pico_malloc/pico_malloc.c.obj
[ 37%] Building ASM object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/pico_mem_ops/mem_ops_aeabi.S.obj
[ 37%] Building ASM object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/pico_standard_link/crt0.S.obj
[ 38%] Building CXX object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/pico_standard_link/new_delete.cpp.obj
[ 39%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/pico_standard_link/binary_info.c.obj
[ 39%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/pico_stdio/stdio.c.obj
[ 40%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/pico_stdio_uart/stdio_uart.c.obj
[ 41%] Building C object CMakeFiles/bmp280_driver.dir/pico-sdk/src/rp2_common/hardware_i2c/i2c.c.obj
[ 42%] Linking CXX static library ../bin/libbmp280_driver.a
[ 42%] Built target bmp280_driver
Scanning dependencies of target bmp280_example
[ 42%] Building C object CMakeFiles/bmp280_example.dir/example/src/example.c.obj
[ 43%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_stdlib/stdlib.c.obj
[ 44%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/hardware_gpio/gpio.c.obj
[ 44%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/hardware_claim/claim.c.obj
[ 45%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_platform/platform.c.obj
[ 46%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/hardware_sync/sync.c.obj
[ 47%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/hardware_irq/irq.c.obj
[ 47%] Building ASM object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/hardware_irq/irq_handler_chain.S.obj
[ 48%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/common/pico_sync/sem.c.obj
[ 49%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/common/pico_sync/lock_core.c.obj
[ 49%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/common/pico_time/time.c.obj
[ 50%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/common/pico_time/timeout_helper.c.obj
[ 51%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/hardware_timer/timer.c.obj
[ 51%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/common/pico_util/datetime.c.obj
[ 52%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/common/pico_util/pheap.c.obj
[ 53%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/common/pico_util/queue.c.obj
[ 54%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/common/pico_sync/mutex.c.obj
[ 54%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/common/pico_sync/critical_section.c.obj
[ 55%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/hardware_uart/uart.c.obj
[ 56%] Building ASM object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/hardware_divider/divider.S.obj
[ 56%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_runtime/runtime.c.obj
[ 57%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/hardware_clocks/clocks.c.obj
[ 58%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/hardware_pll/pll.c.obj
[ 58%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/hardware_vreg/vreg.c.obj
[ 59%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/hardware_watchdog/watchdog.c.obj
[ 60%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/hardware_xosc/xosc.c.obj
[ 61%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_printf/printf.c.obj
[ 61%] Building ASM object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_bit_ops/bit_ops_aeabi.S.obj
[ 62%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_bootrom/bootrom.c.obj
[ 63%] Building ASM object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_divider/divider.S.obj
[ 63%] Building ASM object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_double/double_aeabi.S.obj
[ 64%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_double/double_init_rom.c.obj
[ 65%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_double/double_math.c.obj
[ 67%] Building ASM object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_double/double_v1_rom_shim.S.obj
[ 67%] Building ASM object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_int64_ops/pico_int64_ops_aeabi.S.obj
[ 68%] Building ASM object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_float/float_aeabi.S.obj
[ 69%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_float/float_init_rom.c.obj
[ 69%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_float/float_math.c.obj
[ 70%] Building ASM object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_float/float_v1_rom_shim.S.obj
[ 71%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_malloc/pico_malloc.c.obj
[ 71%] Building ASM object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_mem_ops/mem_ops_aeabi.S.obj
[ 72%] Building ASM object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_standard_link/crt0.S.obj
[ 73%] Building CXX object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_standard_link/new_delete.cpp.obj
[ 74%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_standard_link/binary_info.c.obj
[ 74%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_stdio/stdio.c.obj
[ 75%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_stdio_usb/reset_interface.c.obj
[ 76%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_stdio_usb/stdio_usb.c.obj
[ 76%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_stdio_usb/stdio_usb_descriptors.c.obj
[ 77%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/lib/tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.c.obj
[ 78%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/lib/tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.c.obj
[ 78%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/lib/tinyusb/src/device/usbd.c.obj
[ 79%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/lib/tinyusb/src/device/usbd_control.c.obj
[ 80%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/lib/tinyusb/src/class/audio/audio_device.c.obj
[ 81%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/lib/tinyusb/src/class/cdc/cdc_device.c.obj
[ 81%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/lib/tinyusb/src/class/dfu/dfu_device.c.obj
[ 82%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/lib/tinyusb/src/class/dfu/dfu_rt_device.c.obj
[ 83%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/lib/tinyusb/src/class/hid/hid_device.c.obj
[ 83%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/lib/tinyusb/src/class/midi/midi_device.c.obj
[ 84%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/lib/tinyusb/src/class/msc/msc_device.c.obj
[ 85%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/lib/tinyusb/src/class/net/ecm_rndis_device.c.obj
[ 86%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/lib/tinyusb/src/class/net/ncm_device.c.obj
[ 86%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/lib/tinyusb/src/class/usbtmc/usbtmc_device.c.obj
[ 87%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/lib/tinyusb/src/class/vendor/vendor_device.c.obj
[ 88%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/lib/tinyusb/src/class/video/video_device.c.obj
[ 88%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/lib/tinyusb/src/tusb.c.obj
[ 89%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/lib/tinyusb/src/common/tusb_fifo.c.obj
[ 90%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_fix/rp2040_usb_device_enumeration/rp2040_usb_device_enumeration.c.obj
[ 90%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/pico_unique_id/unique_id.c.obj
[ 91%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/hardware_flash/flash.c.obj
[ 92%] Building C object CMakeFiles/bmp280_example.dir/pico-sdk/src/rp2_common/hardware_i2c/i2c.c.obj
[ 93%] Linking CXX executable bmp280_example.elf
[ 93%] Built target bmp280_example
Scanning dependencies of target PioasmBuild
[ 93%] Creating directories for 'PioasmBuild'
[ 94%] No download step for 'PioasmBuild'
[ 95%] No update step for 'PioasmBuild'
[ 95%] No patch step for 'PioasmBuild'
[ 96%] Performing configure step for 'PioasmBuild'
loading initial cache file /mnt/c/Users/sino3/Desktop/shci-test/bmp280_driver/bmp280_driver/build/pico-sdk/src/rp2_common/cyw43_driver/pioasm/tmp/PioasmBuild-cache-Release.cmake
-- The CXX compiler identification is GNU 10.2.1
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done
-- Generating done
-- Build files have been written to: /mnt/c/Users/sino3/Desktop/shci-test/bmp280_driver/bmp280_driver/build/pioasm
[ 97%] Performing build step for 'PioasmBuild'
Scanning dependencies of target pioasm
[ 10%] Building CXX object CMakeFiles/pioasm.dir/main.cpp.o
[ 20%] Building CXX object CMakeFiles/pioasm.dir/pio_assembler.cpp.o
[ 30%] Building CXX object CMakeFiles/pioasm.dir/pio_disassembler.cpp.o
[ 40%] Building CXX object CMakeFiles/pioasm.dir/gen/lexer.cpp.o
[ 50%] Building CXX object CMakeFiles/pioasm.dir/gen/parser.cpp.o
[ 60%] Building CXX object CMakeFiles/pioasm.dir/c_sdk_output.cpp.o
[ 70%] Building CXX object CMakeFiles/pioasm.dir/python_output.cpp.o
[ 80%] Building CXX object CMakeFiles/pioasm.dir/hex_output.cpp.o
[ 90%] Building CXX object CMakeFiles/pioasm.dir/ada_output.cpp.o
[100%] Linking CXX executable pioasm
[100%] Built target pioasm
[ 98%] No install step for 'PioasmBuild'
[100%] Completed 'PioasmBuild'
[100%] Built target PioasmBuild

```

---

    

Build terminated with exit code 0

---

