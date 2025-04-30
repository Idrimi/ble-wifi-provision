#!/usr/bin/env bash
set -euo pipefail

# Unblock & start Bluetooth
rfkill unblock bluetooth
systemctl enable --now bluetooth

# Advertise as Pi-Setup
bluetoothctl <<EOS
power on
discoverable on
pairable on
system-alias Pi-Setup
agent NoInputNoOutput
default-agent
EOS
bluetoothctl advertise on || true

# Run GATT server, capture creds
/usr/bin/env python3 /usr/local/bin/ble-creds-server.py | tee /tmp/ble-creds.log

# Parse & connect
SSID=$(grep "Got SSID" /tmp/ble-creds.log | sed -E "s/.*'(.+)'/\1/")
PSK=$(grep "PSK"   /tmp/ble-creds.log | sed -E "s/.*'(.+)'/\1/")
nmcli device wifi connect "$SSID" password "$PSK"

# Stop advertising
bluetoothctl <<EOS
advertise off
discoverable off
EOS

echo "✅ Provisioned Wi-Fi: $SSID"
