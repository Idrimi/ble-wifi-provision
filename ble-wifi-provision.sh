#!/usr/bin/env bash
set -euo pipefail

echo "🟢 Unblocking Bluetooth..."
rfkill unblock bluetooth
systemctl enable --now bluetooth

echo "🔷 Starting BLE-GATT server + advertiser..."
/usr/bin/env python3 /usr/local/bin/ble-creds-server.py | tee /tmp/ble-creds.log

SSID=$(grep --color=never "Got SSID" /tmp/ble-creds.log | sed -E "s/.*'(.+)'/\1/")
PSK=$(grep --color=never "PSK"   /tmp/ble-creds.log | sed -E "s/.*'(.+)'/\1/")

echo "💻 Connecting to Wi-Fi: $SSID"
nmcli device wifi connect "$SSID" password "$PSK"

echo "✅ Provisioned Wi-Fi: $SSID"
