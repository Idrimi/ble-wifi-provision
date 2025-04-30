#!/usr/bin/env bash
set -euo pipefail

echo "==> Installing dependencies…"
apt-get update
apt-get install -y python3-dbus-next python3-pip bluez bluetooth rfkill network-manager

echo "==> Deploying scripts…"
cp ble-creds-server.py ble-wifi-provision.sh /usr/local/bin/
chmod +x /usr/local/bin/ble-creds-server.py /usr/local/bin/ble-wifi-provision.sh

echo "Setup complete! Run 'sudo /usr/local/bin/ble-wifi-provision.sh' to start provisioning."
