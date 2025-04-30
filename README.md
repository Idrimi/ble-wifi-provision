# BLE → Wi-Fi Provisioning for Raspberry Pi Zero 2 W

This repository provides a headless BLE Wi-Fi provisioning flow for the Raspberry Pi Zero 2 W.

## Contents

- **ble-creds-server.py**  
  Python GATT server (dbus-next + asyncio) advertising over BLE with a fixed SERVICE_UUID constant.

- **ble-wifi-provision.sh**  
  Shell wrapper to run the GATT server and connect Wi-Fi via nmcli.

- **setup-ble-wifi-bookworm.sh**  
  Installer script for Debian Bookworm Lite.

## Quickstart

1. Run the installer:
   ```bash
   sudo ./setup-ble-wifi-bookworm.sh
   ```
2. In your BLE app, connect to **Pi-Setup**, write SSID to `SERVICE_UUID.char1`, and password to `SERVICE_UUID.char2`.
3. Pi joins network automatically.
