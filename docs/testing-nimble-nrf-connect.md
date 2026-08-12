# Testing ESP32 NimBLE LED Control with nRF Connect

This guide outlines the step-by-step process for testing your modularized ESP32 BLE firmware using the **nRF Connect for Mobile** app (available on iOS and Android).

---

## 1. Prerequisites

- **ESP32 Hardware:** Board connected via USB, running the updated `ble_server`, `led`, and `main` modules.
- **Smartphone Settings:** Bluetooth and Location services turned **ON**.
- **App Installed:** [nRF Connect for Mobile](https://www.nordicsemi.com/Products/Development-tools/nRF-Connect-for-mobile).

---

## 2. Step-by-Step Testing Procedure

### Step 1: Start the Serial Monitor
Run the following command in your terminal to monitor live C logs from the ESP32:

```bash
idf.py flash monitor
```

Look for the initialization log confirming Bluetooth advertising:
> `I (xxx) BLE_SERVER: Advertising started as 'ESP32-LED'`

---

### Step 2: Discover & Connect in nRF Connect

1. Open **nRF Connect** on your mobile phone.
2. In the **Scanner** tab, tap **Scan** (or pull down to refresh).
3. Look for the device named **ESP32-LED**.
4. Tap the **CONNECT** button next to `ESP32-LED`.

---

### Step 3: Locate GATT Service & Characteristic

Once connected, nRF Connect will display the GATT attribute table:

1. Scroll down to locate your custom Service UUID:
   - **Service UUID:** `12345678-1234-5678-1234-56789abcdef0`
2. Tap the service to expand its attributes.
3. Find the custom Characteristic:
   - **Characteristic UUID:** `12345678-1234-5678-1234-56789abcdef1`
   - **Properties:** `READ`, `WRITE`

---

### Step 4: Write LED Intensity Values

1. Tap the **Up Arrow icon** ($\uparrow$) next to the Characteristic `...abcdef1`.
2. In the pop-up window:
   - Select **UINT8** (or **BYTE**) as the format type.
   - Enter a numerical value between **0** and **100** (e.g., `100` for full brightness, `25` for dim, `0` for off).
3. Tap **SEND** or **WRITE**.

---

## 3. Verification

- **Hardware:** The LED on GPIO 18 will instantly adjust its PWM brightness to match the value sent.
- **Serial Output:** Your ESP-IDF terminal will print:
  ```text
  I (12450) BLE_SERVER: BLE Intensity Write: 75%
  ```

---

## 4. Troubleshooting Checklist

| Symptom | Cause | Solution |
| :--- | :--- | :--- |
| **Device not listed in scanner** | Advertising failed or stack halted. | Check serial logs for NVS init errors or `nimble_port_init()` failures. Reset ESP32. |
| **Write button disabled/error** | Connection lost or wrong characteristic. | Confirm connected state; verify UUID matches `ble_server.c`. |
| **LED does not change state** | `led_init` pin mismatch or channel error. | Ensure hardware PWM timer/GPIO number in `main.c` matches physical wiring. |