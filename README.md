# BLE → Wi-Fi Provisioning for Raspberry Pi Zero 2 W

This repository provides a headless BLE Wi-Fi provisioning flow for the Raspberry Pi Zero 2 W.

## Contents

- **ble-creds-server.py**  
  Python GATT server (dbus-next + asyncio) advertising over BLE with fixed method calls.

- **ble-wifi-provision.sh**  
  Shell wrapper to run the GATT server and connect Wi-Fi via nmcli.

- **setup-ble-wifi-bookworm.sh**  
  Installer script for Debian Bookworm Lite.

- **ble-wifi-provision.service**  
  Optional systemd unit.

- **Dockerfile**  
  Containerized version.

## Quickstart

1. Run the installer:
   ```bash
   sudo ./setup-ble-wifi-bookworm.sh
   ```
2. In your BLE app, connect to **Pi-Setup**, write SSID to `CHAR1_UUID`, and password to `CHAR2_UUID`.
3. Pi joins network automatically.
