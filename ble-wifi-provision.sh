#!/usr/bin/env bash
set -euo pipefail

echo "🟢 Unblocking Bluetooth..."
rfkill unblock bluetooth
systemctl enable --now bluetooth

echo "🔷 Starte BL EProvisioning via Bluezero..."
exec /usr/local/bin/ble-provision.py
