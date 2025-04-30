#!/usr/bin/env bash
set -euo pipefail
echo "Compiling ble_provision..."
g++ -std=c++17 ble_provision.cpp -o ble_provision $(pkg-config --cflags --libs sdbus-c++)
chmod +x ble_provision
echo "Installing binary and service..."
sudo cp ble_provision /usr/local/bin/ble_provision
sudo cp ble-wifi-provision.service /etc/systemd/system/
sudo systemctl daemon-reload
echo "Done. Enable with: sudo systemctl enable ble-wifi-provision && sudo systemctl start ble-wifi-provision"  
