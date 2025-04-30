# BLE Wi-Fi Provisioning via C++ with Pairable BLE for iOS Visibility

Dieses Repository erweitert die C++ BLE-Provisioning-Lösung um:
1. Registrierung eines BLE-Pairing-Agenten (`Agent1`).
2. Adapter auf `Pairable` und `Discoverable` setzen.

Damit erscheint der Raspberry Pi Zero 2 W in iOS Einstellungen → Bluetooth.

## Dateien

- **ble_provision.cpp**  
  Komplettes C++-Programm mit Agent1-Registrierung, Adapter-Properties, GATT-Service und Advertisement.

- **compile.sh**  
  Skript zum Kompilieren und Installieren des Binaries und des Systemd-Services.

- **ble-wifi-provision.service**  
  Systemd-Unit zum Starten des Provisioning-Dienstes.

## Schnellstart

1. Abhängigkeiten installieren:  
   ```bash
   sudo apt update
   sudo apt install -y g++ pkg-config libdbus-1-dev libsystemd-dev libglib2.0-dev libsdbus-c++-dev
   ```

2. Kompilieren & Installieren:  
   ```bash
   chmod +x compile.sh
   ./compile.sh
   ```

3. Service aktivieren & starten:  
   ```bash
   sudo systemctl enable ble-wifi-provision
   sudo systemctl start ble-wifi-provision
   ```

4. In iOS unter **Einstellungen → Bluetooth** das Gerät **Pi-Setup** sehen, pairen und anschließend SSID/PSK schreiben.
