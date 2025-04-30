#!/usr/bin/env bash
set -euo pipefail

echo "==> Installiere Go über apt..."
apt update
apt install -y golang-go git

echo "==> Baue das BLE-Provisioning-Programm..."
GOPATH=$(mktemp -d)
export GOPATH
export PATH=$PATH:/usr/local/go/bin:$GOPATH/bin
cd "$(dirname "$0")"
go mod tidy
go build -o bin/ble-golang-provision main.go

echo "==> Deployen..."
sudo cp bin/ble-golang-provision /usr/local/bin/ble-golang-provision
sudo cp ble-wifi-provision.sh /usr/local/bin/ble-wifi-provision.sh
sudo cp ble-wifi-provision.service /etc/systemd/system/
sudo chmod +x /usr/local/bin/ble-golang-provision /usr/local/bin/ble-wifi-provision.sh
sudo systemctl daemon-reload
sudo systemctl enable ble-wifi-provision.service

echo "Setup abgeschlossen! Starte Provisioning mit:"
echo "  sudo systemctl start ble-wifi-provision.service"
