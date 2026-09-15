# Smart LED

An LED powered by an ESP32 chip that you can control using your phone. It uses Bluetooth Low Energy for communication.

## Supported configurations

1. Brightness control (using PWM)
2. Blink pattern

## How to control

You can control the LED using nRF Connect:

1. Connect to the device named "Smart LED"
2. Upload the values to the desired characteristics

## The MCU I Used

The following information were read using the `esptool.py` command.

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

## Tutorials

I put together a few quick tutorials sharing everything I learned while building this project. Check them out on my website:

- [ESP-IDF VS Code Setup Tutorial](https://effessdev.github.io/read/esp-idf-vscode-setup-guide/)
- [ESP-IDF Bluetooth Low Energy Tutorial for Beginners](https://effessdev.github.io/read/ble-basics-in-esp-idf/)

I have also posted them on DEV.to. Here are the links:

- [Beyond Arduino: Getting Started With ESP-IDF in VS Code](https://dev.to/effessdev/beyond-arduino-getting-started-with-esp-idf-in-vs-code-for-esp32-3ed4)
- [ESP-IDF Bluetooth LE Beginner Tutorial](https://dev.to/effessdev/esp-idf-bluetooth-le-tutorial-53c)