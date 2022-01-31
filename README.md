# BK7231 CMake SDK
A quick project aiming to provide a (finally) usable SDK for Beken chips, commonly found in Tuya IoT modules.

## Supported platforms
(i guess)
:
### BK7231T
Found in Tuya-branded modules, such as 
[WB1S](https://developer.tuya.com/en/docs/iot/wb1s?id=K9duevbj3ol4x),
[WB2S](https://developer.tuya.com/en/docs/iot/wb2s-module-datasheet?id=K9ghecl7kc479),
[WB2L](https://developer.tuya.com/en/docs/iot/wb2l-datasheet?id=K9duegc9bualu),
[WB3S](https://developer.tuya.com/en/docs/iot/wb3s-module-datasheet?id=K9dx20n6hz5n4),
[WB3L](https://developer.tuya.com/en/docs/iot/wb3l-module-datasheet?id=K9duiggw2v8sp) and probably some more.

Tested to run on WB2L. The chip itself is marked as `BK7231TQN32`, the SDK mentions `BK7231U` and the bootloader
says it's `BK7231S`. Yes.

This platform is based on [tuya-iotos-embeded-sdk-wifi-ble-bk7231t](https://github.com/tuya/tuya-iotos-embeded-sdk-wifi-ble-bk7231t).
The bare-bones SDK is used, without Tuya's SDKs and APIs.

The firmware is encrypted using keys present in the source repo.

| File/dir                     | Description                             | Source                                                                                                                                                           |
|------------------------------|-----------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `bk7231t/`                   | BK7231T SDK                             | [beken378](https://github.com/tuya/tuya-iotos-embeded-sdk-wifi-ble-bk7231t/tree/master/platforms/bk7231t/bk7231t_os/beken378)                                    |
| `bk7231t.ld`                 | Linker description                      | [bk7231_ota.ld](https://github.com/tuya/tuya-iotos-embeded-sdk-wifi-ble-bk7231t/blob/master/platforms/bk7231t/bk7231t_os/beken378/build/bk7231_ota.ld)           |
| `bk7231t_bootloader.bin`     | Bootloader (`BK7231S_1.0.5`)            | [bootloader.bin](https://github.com/tuya/tuya-iotos-embeded-sdk-wifi-ble-bk7231t/blob/master/platforms/bk7231t/bk7231t_os/tools/generate/bootloader.bin)         |
| `bk7231t_bootloader_enc.bin` | Bootloader (encrypted for Tuya modules) | [bootloader_enc.bin](https://github.com/tuya/tuya-iotos-embeded-sdk-wifi-ble-bk7231t/blob/master/platforms/bk7231t/bk7231t_os/tools/generate/bootloader_enc.bin) |

## Usage

`add_subdirectory()` in your project's CMakeLists. Then call
```cmake
beken378_executable(app_name 1.0.0 <platform> <sources>)
```
with `<platform>` replaced with one of the supported SoCs, and <sources> with your C sources.

To add include directories, libraries, etc., just call standard CMake functions with your app's name as target.

## Outputs

After a successful compilation, your CMake build directory will contain the following files:
- app_name_1.0.0.asm - disassembly of the ELF
- app_name_1.0.0.bin - raw binary firmware file
- app_name_1.0.0.elf - compiled sources
- app_name_1.0.0.map - address map of the compiled ELF
- app_name_1.0.0.rbl - probably a compressed binary for OTA
- app_name_1.0.0_enc.bin - encrypted binary firmware
- app_name_1.0.0_QIO.bin - complete firmware image for QIO upload (whatever that is)
- **app_name_1.0.0_UA.bin** - firmware image for UART upload - this is probably what you want
- app_name_1.0.0_UG.bin - OTA image, Tuya outputs this file the same way

The `UA` file can be flashed with BkWriter, to address 0x11000.
