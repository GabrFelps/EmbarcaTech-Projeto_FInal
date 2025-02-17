# bmp280_driver

A C library that reads pressure and temperature data from the Bosch BMP280 chip for the Raspberry Pi Pico, using the Pico SDK by Raspberry.

---

## [Build status](https://github.com/MrSinho/bmp280_driver/tree/main/.ShCI)

[![linux_badge](.shci/linux-exit_code.svg)](.shci/linux-log.md)
[![windows_badge](.shci/windows-exit_code.svg)](.shci/windows-log.md)

---

* [Clone repository](#clone-repository)
* [Generate project files and build](#generate-project-files-and-build)
* [Force gcc compiler only](#force-gcc-compiler-only)

---

## Clone repository

```bash
git clone https://github.com/mrsinho/bmp280_driver.git
cd bmp280_driver
```

---

## Generate project files and build

```bash
mkdir build
cd build
```

If you have an environment variable named `PICO_SDK_PATH` which points to a [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk) root directory, do:

```bash
cmake -G"your-generator" -DBMP280_DRIVER_BUILD_EXAMPLE=ON ..
cmake --build .
```

Otherwise build the library by defining the `PICO_SDK_PATH` variable directly from the command line:

```bash
cmake -DPICO_SDK_PATH="your-pico-sdk-path" -G"your-generator" -DBMP280_DRIVER_BUILD_EXAMPLE=ON ..
cmake --build .
```

You can find the example source code at `example/src/example.c`
The binary UF2 file is located at `build/`

---

## Force gcc compiler only

If you don't want to use the C++ features and stick to the arm gcc compiler only, set `ON` the `BMP280_DRIVER_FORCE_C_COMPILER` variable. The command shown below will run successfully only if the line cointaining `new_delete.cpp` is deleted or commented out from `pico-sdk-directory/src/rp2_common/pico_standard_link/CMakeLists.txt`. There's a [fork](https://github.com/mrsinho/pico-sdk-c) of the `pico-sdk` which is already set up for that purpose. Link: [`https://github.com/mrsinho/pico-sdk-c`](https://github.com/mrsinho/pico-sdk-c)

```bash
cmake -DPICO_SDK_PATH="your-pico-sdk-path" -G"your-generator" -DBMP280_DRIVER_BUILD_EXAMPLE=ON -DBMP280_DRIVER_FORCE_C_COMPILER=ON ..
cmake --build .
```