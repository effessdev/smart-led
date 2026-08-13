# GATT (Generic Attribute Profile)

- **What it is:** GATT is the BLE application-level protocol that organizes data into services and characteristics.
- **Service:** a container identified by a UUID (here `gatt_svc_uuid`) grouping related characteristics.
- **Characteristic:** a named value with properties (read/write/notify) identified by a UUID (here `gatt_chr_uuid`).
- **Access callback:** `gatt_access_cb` is called by NimBLE when a client reads or writes the characteristic; it handles incoming data and invokes the application callback.
- **Server setup:** `gatt_svcs` describes the service/characteristic layout; `ble_gatts_count_cfg()` and `ble_gatts_add_svcs()` register them with the NimBLE GATT server.

This repository's GATT usage: a single custom service (128-bit UUID) with one read/write characteristic used to set LED intensity.
