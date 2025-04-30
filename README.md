# BLE Wi-Fi Provisioning via C++ for Raspberry Pi Zero 2 W

This repository provides a C++-based BLE peripheral to provision Wi-Fi on Raspberry Pi Zero 2 W using sdbus-c++.

This version adds a short delay after GATT application registration to avoid D-Bus 'No object received' errors.

## Files

- **ble_provision.cpp**: Main C++ source with added sleep before advertisement registration.
- **compile.sh**: Script to compile, install binary and systemd service.
- **ble-wifi-provision.service**: systemd unit for provisioning.

Follow the same Quickstart as before.
