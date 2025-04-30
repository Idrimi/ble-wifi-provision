# BLE Wi-Fi Provisioning via C++ with iOS-Pairable BLE (Final)

Diese Version behebt die Anzeige im iOS-Bluetooth-Settings durch:
1. `RegisterApplication` mit Service-Pfad statt root `/`.
2. Korrekte `createProxy`-Aufrufe (3 Argumente).
3. Service-Pfad außerhalb von `/org/bluez`, z.B. `/example/service0`.

## Dateien

- **ble_provision.cpp**  
  Finaler C++-Code mit Agent1, Adapter-Properties, korrektem RegisterApplication.

- **compile.sh**  
  Kompiliert und installiert Binary & Systemd-Service.

- **ble-wifi-provision.service**  
  systemd-Unit.

## Schnellstart

Abhängigkeiten installieren, dann:

```bash
chmod +x compile.sh
./compile.sh
sudo systemctl enable ble-wifi-provision
sudo systemctl start ble-wifi-provision
sudo journalctl -u ble-wifi-provision -f
```

In iOS-Einstellungen unter Bluetooth sollte **Pi-Setup** nun sichtbar sein und nach Pairing Services & Characteristics anzeigen.