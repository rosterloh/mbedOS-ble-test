# mbedOS ble test

## Installing mbed-cli

Follow the documented instructions [here](https://github.com/ARMmbed/mbed-cli/blob/master/README.md#installing-mbed-cli)

## Building
```bash
$ mbed update
$ mbed build -m NRF52_DK -t GCC_ARM
```
You will find the resulting binary in .build/NRF52_DK/GCC_ARM/mbedos-ble-test.bin. You can drag and drop it onto your board USB drive.

## Debugging

When a debugger is connected, you can observe debug output from uVisor. Please note that these messages are sent through semihosting, which halts the program execution if a debugger is not connected. For more information please read the [Debugging uVisor on mbed OS](https://github.com/ARMmbed/uvisor/blob/master/docs/api/DEBUGGING.md) guide. To build a debug version of the program:

```bash
$ mbed compile -m NRF52_DK -t GCC_ARM -o "debug-info"
```
