# BLE Wi-Fi Provisioning via Go for Raspberry Pi Zero 2 W

This repository provides a Go-based BLE peripheral to provision Wi-Fi on Raspberry Pi Zero 2 W.

## Components

- **main.go**: Go program using [go-ble/ble](https://github.com/go-ble/ble) to advertise and handle GATT writes for SSID & password.
- **go.mod**: Go module file.
- **ble-wifi-provision.sh**: Wrapper script to run the binary.
- **ble-wifi-provision.service**: systemd unit to start provisioning on boot.
- **setup-go.sh**: Installer script to install Go via apt, build the binary, and deploy files.

## Quickstart

1. Unzip and enter directory:
   ```bash
   unzip ble-golang-provision-update.zip
   cd ble-golang-provision-update
   ```
2. Run installer:
   ```bash
   sudo chmod +x setup-go.sh
   sudo ./setup-go.sh
   ```
3. Start provisioning:
   ```bash
   sudo systemctl start ble-wifi-provision.service
   ```
4. In your BLE app, scan for **Pi-Setup**, write SSID to characteristic `12345678-1234-5678-1234-56789abcdef1`, and password to `...ef2`.
5. On success, the Pi connects and stops advertising.
