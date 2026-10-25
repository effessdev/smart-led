# Smart LED

An LED powered by an ESP32 chip that you can control using your phone. It uses Bluetooth Low Energy for communication.

## Supported configurations

1. Brightness control (using PWM)
2. Blink pattern

## How to control

You can control the LED using nRF Connect:

1. Connect to the device named "Smart LED"
2. Upload the values to the desired characteristics

## Required Menuconfig Configurations

- Partition Table: Two large size OTA partitions
- Component config > Bluetooth:
  - Bluetooth (box): Checked
  - Host: NimBLE (BLE only)
- WiFi SSID and password
- Serial flasher config > Flash size: 4MB (or more)

## The MCU I Used

The following information were obtained using the `esptool.py` command.

```
Chip type:          ESP32-D0WD-V3 (revision v3.1)
Features:           Wi-Fi, BT, Dual Core + LP Core, 240MHz, Vref calibration in eFuse, Coding Scheme None
Crystal frequency:  40MHz
MAC:                8c:94:df:6d:7b:38

Flash Memory Information:
=========================
Manufacturer: 5e
Device: 4016
Detected flash size: 4MB
Flash voltage set by a strapping pin: 3.3V
```

## OTA Updates

To trigger an OTA update, upload a UNIT8 value to the characteristic with the UUID `0x2A5A` using nRF Connect.

## Tutorials

I put together an ESP-IDF course that focuses on the fundamentals of the framework. Check it out here:

- [ESP-IDF Course](https://effessdev.github.io/esp-idf/)

Also, the following tutorial helped me implement the OTA update functionality:

- [How to Implement OTA Firmware Updates on ESP32](https://hubble.com/community/guides/how-to-implement-ota-firmware-updates-on-esp32/)
