# BLE → Wi-Fi Provisioning for Raspberry Pi Zero 2 W

This repo provides:

- **ble-creds-server.py**  
  A Python GATT server (dbus-next + trio) exposing SSID/password characteristics.

- **ble-wifi-provision.sh**  
  BLE advertising wrapper that runs the GATT server, reads creds, then calls `nmcli`.

- **setup-ble-wifi-bookworm.sh**  
  One-shot installer for Debian Bookworm Lite.

- **ble-wifi-provision.service**  
  Optional systemd unit.

- **Dockerfile**  
  Containerized version (requires privileged + host networking).

---

## Quickstart

1. Run installer on your Pi:
   ```bash
   sudo ./setup-ble-wifi-bookworm.sh
   ```
2. On your phone/laptop, scan for **Pi-Setup**, write SSID to UUID `…ef1`, password to `…ef2`.
3. Watch the Pi console: it will join your network automatically.
