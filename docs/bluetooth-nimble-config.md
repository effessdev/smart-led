# Bluetooth and NimBLE Configuration in Menuconfig

This project uses BLE to control the LED wirelessly. Two Menuconfig settings are required:

1. **Enable Bluetooth** (`CONFIG_BT_ENABLED=y`) — activates the Bluetooth subsystem. Without it, NimBLE headers (`host/ble_hs.h`, `nimble/nimble_port.h`) are unavailable and all BLE calls in `ble_server.c` fail to link.

2. **Set the host to NimBLE** (`CONFIG_BT_NIMBLE_ENABLED=y`) — selects NimBLE as the BLE stack. NimBLE uses ~40 KB RAM vs Bluedroid's ~150 KB, making it the right fit for a simple GATT peripheral. The two stacks are mutually exclusive.

Both are set in the project's `sdkconfig`. Find them at:
`Component config → Bluetooth → [*] Enable Bluetooth → Choose BLE Stack → [*] NIMBLE`
