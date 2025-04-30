# BLE Wi-Fi Provisioning via C++ for Raspberry Pi Zero 2 W (Modern sdbus-c++)

This repository provides a modern C++ implementation using sdbus-c++ v2 to set up a BLE peripheral for headless Wi-Fi provisioning on a Raspberry Pi Zero 2 W.

## Files

- **ble_provision.cpp**  
  Main C++ source with GATT server & LE advertisement via modern sdbus-c++ v2 API.

- **compile.sh**  
  Script to compile, install the binary, and register the systemd service.

- **ble-wifi-provision.service**  
  systemd unit to launch the provisioning binary on boot or on demand.

## Quickstart

1. Install dependencies:
   ```bash
   sudo apt update
   sudo apt install -y g++ pkg-config libdbus-1-dev libsystemd-dev libglib2.0-dev libsdbus-c++-dev
   ```

2. Compile & install:
   ```bash
   chmod +x compile.sh
   ./compile.sh
   ```

3. Enable & start the service:
   ```bash
   sudo systemctl enable ble-wifi-provision
   sudo systemctl start ble-wifi-provision
   ```

4. With a BLE client (e.g., nRF Connect), scan for **Pi-Setup**, write your SSID to characteristic `...ef1`, and password to `...ef2`.  
5. The Pi will connect via `nmcli` and stop advertising upon success.
