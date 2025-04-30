# BLE → Wi-Fi Provisioning for Raspberry Pi Zero 2 W

This repository provides a headless BLE-based Wi-Fi provisioning flow for the Raspberry Pi Zero 2 W.

## Contents

- **ble-creds-server.py**  
  A Python GATT server (dbus-next + asyncio) that advertises over BLE and exposes two write-only characteristics for SSID & password.

- **ble-wifi-provision.sh**  
  A shell wrapper that runs the GATT server, captures credentials, and uses `nmcli` to connect to Wi-Fi.

- **setup-ble-wifi-bookworm.sh**  
  Installer script for Debian Bookworm Lite that installs dependencies and deploys the two main scripts.

- **ble-wifi-provision.service**  
  Optional systemd unit for automatic provisioning at boot.

- **Dockerfile**  
  Containerized version (requires privileged + host networking).

## Quickstart

1. Run the installer on your Pi:
   ```bash
   sudo ./setup-ble-wifi-bookworm.sh
   ```
2. On your phone/laptop (e.g., nRF Connect), scan for **Pi-Setup**, write SSID to characteristic ending in `...ef1`, write password to `...ef2`.
3. The Pi will automatically join the network headlessly.
