#!/usr/bin/env bash
set -euo pipefail

# Ensure Bluetooth is enabled
rfkill unblock bluetooth
systemctl enable --now bluetooth

# Run BLE provisioning binary
exec /usr/local/bin/ble-golang-provision
