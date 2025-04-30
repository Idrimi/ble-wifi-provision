#!/usr/bin/env bash
set -euo pipefail

# Install Go if not present
if ! command -v go &>/dev/null; then
  wget https://golang.org/dl/go1.20.5.linux-armv6l.tar.gz -O /tmp/go.tar.gz
  sudo tar -C /usr/local -xzf /tmp/go.tar.gz
  export PATH=$PATH:/usr/local/go/bin
fi

# Build binary
cd "$(dirname "$0")"
go mod tidy
GOARCH=arm GOARM=6 GOOS=linux go build -o bin/ble-golang-provision main.go

# Deploy
sudo cp bin/ble-golang-provision /usr/local/bin/ble-golang-provision
sudo cp ble-wifi-provision.sh /usr/local/bin/ble-wifi-provision.sh
sudo cp ble-wifi-provision.service /etc/systemd/system/
sudo chmod +x /usr/local/bin/ble-golang-provision /usr/local/bin/ble-wifi-provision.sh
sudo systemctl daemon-reload
sudo systemctl enable ble-wifi-provision.service

echo "Setup complete! Use 'sudo systemctl start ble-wifi-provision.service' to provision."
