# BLE Wi-Fi Provisioning via Bluezero für Raspberry Pi Zero 2 W

Dieses Repository enthält ein komplettes Setup für eine BLE-basierte Wi-Fi-Provisionierung auf dem Raspberry Pi Zero 2 W mit dem **Bluezero**-Framework.

## Inhalt

- **ble-provision.py**  
  Python-Skript (Bluezero) für GATT-Server und Advertisement.

- **ble-wifi-provision.sh**  
  Shell-Wrapper, der den GATT-Server startet.

- **ble-wifi-provision.service**  
  systemd-Unit für automatischen Start.

- **setup-blezero.sh**  
  Installationsskript zum Einrichten der Umgebung und Aktivieren des Dienstes.

## Schnellstart

1. ZIP entpacken und in das Verzeichnis wechseln:  
   ```bash
   unzip blezero-provision.zip
   cd blezero-provision
   ```

2. Installationsskript ausführen:
   ```bash
   sudo chmod +x setup-blezero.sh
   sudo ./setup-blezero.sh
   ```

3. Provisioning starten:
   ```bash
   sudo systemctl start ble-wifi-provision.service
   # oder manuell:
   sudo ./ble-wifi-provision.sh
   ```

4. Mit einer BLE-App (z.B. nRF Connect) **Pi-Setup** scannen, SSID auf Charakteristik `...ef1` schreiben und Passwort auf `...ef2`.

5. Auf der Konsole erscheint:
   ```
   [GATT] SSID gesetzt: MeinNetz
   [GATT] PSK gesetzt: supergeheim
   [GATT] Verbinde zu Wi-Fi: MeinNetz
   [END] Provisioning abgeschlossen.
   ```
