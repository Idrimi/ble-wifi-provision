#!/usr/bin/env bash
set -euo pipefail

# Ensure Bluetooth is enabled
rfkill unblock bluetooth
systemctl enable --now bluetooth

# Run Rust provisioning binary
exec /usr/local/bin/ble-rust-provision
