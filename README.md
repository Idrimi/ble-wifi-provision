# BLE Wi-Fi Provisioning via C++ for Raspberry Pi Zero 2 W (Adapter Power Fix)

This repository provides a C++ implementation using sdbus-c++ v2 for BLE Wi-Fi provisioning.

This version ensures the Bluetooth adapter is powered on via D-Bus before registering advertisement.

## Files

- **ble_provision.cpp**: Main C++ source with adapter power-on logic.
- **compile.sh**: Script to compile, install binary, and register the systemd service.
- **ble-wifi-provision.service**: systemd unit for provisioning.

## Quickstart

Install dependencies, compile, enable, and start as described below.
