#!/usr/bin/env bash
set -euo pipefail

echo "==> Installiere erforderliche Pakete..."
apt update
apt install -y python3-pip python3-gi python3-dev libglib2.0-dev bluetooth bluez

echo "==> Installiere Bluezero via pip..."
pip3 install bluezero

echo "==> Kopiere Skripte nach /usr/local/bin..."
cp ble-provision.py ble-wifi-provision.sh /usr/local/bin/
chmod +x /usr/local/bin/ble-provision.py /usr/local/bin/ble-wifi-provision.sh

echo "==> Erstelle systemd-Service..."
cp ble-wifi-provision.service /etc/systemd/system/
systemctl daemon-reload
systemctl enable ble-wifi-provision.service

echo "Setup abgeschlossen! Starte Provisioning mit:"
echo "  sudo systemctl start ble-wifi-provision.service"
