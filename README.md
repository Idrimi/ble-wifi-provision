# BLE Wi-Fi Provisioning via Rust for Raspberry Pi Zero 2 W

This repository provides a Rust-based BLE peripheral to provision Wi-Fi on Raspberry Pi Zero 2 W using the `bluer` crate.

## Components

- **Cargo.toml**: Rust project configuration.
- **src/main.rs**: BLE GATT server with advertisement.
- **setup-rust.sh**: Installer script to set up Rust toolchain and build the binary.
- **ble-wifi-provision.sh**: Wrapper script to run the binary.
- **ble-wifi-provision.service**: systemd unit to start provisioning.

## Quickstart

1. Unzip and enter directory:
   ```bash
   unzip ble-rust-provision.zip
   cd ble-rust-provision
   ```
2. Run installer:
   ```bash
   sudo chmod +x setup-rust.sh
   sudo ./setup-rust.sh
   ```
3. Start provisioning:
   ```bash
   sudo systemctl start ble-wifi-provision.service
   ```
4. In your BLE app, scan for **Pi-Setup**, write SSID to characteristic `12345678-1234-5678-1234-56789abcdef1`, and password to `...ef2`.
5. On success, the Pi connects and stops advertising.
