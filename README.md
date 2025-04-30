# BLE Wi-Fi Provisioning via C++ for Raspberry Pi Zero 2 W

This repository provides a C++-based BLE peripheral to provision Wi-Fi on Raspberry Pi Zero 2 W using sdbus-c++.

## Files

- **ble_provision.cpp**  
  Main C++ source with GATT server and LE advertisement via D-Bus.

- **compile.sh**  
  Script to compile, install binary and systemd service.

- **ble-wifi-provision.service**  
  systemd unit to run the provisioning binary at boot or on demand.

## Quickstart

1. **Install dependencies**:
   ```bash
   sudo apt update
   sudo apt install -y g++ pkg-config libdbus-1-dev libsystemd-dev libglib2.0-dev libsdbus-c++-dev
   ```

2. **Compile & install**:
   ```bash
   chmod +x compile.sh
   ./compile.sh
   ```

3. **Enable and start service**:
   ```bash
   sudo systemctl enable ble-wifi-provision
   sudo systemctl start ble-wifi-provision
   ```

4. **Provision Wi-Fi**:
   - On boot (or service start), the Pi advertises **Pi-Setup**.
   - Connect with a BLE app (e.g. nRF Connect).
   - Write your SSID to characteristic `...ef1`, password to `...ef2`.
   - The Pi runs `nmcli` to connect and stops advertising.
