#!/usr/bin/env python3
import sys
import subprocess
from bluezero import adapter, peripheral
from gi.repository import GLib

# UUIDs
SERVICE_UUID = '12345678-1234-5678-1234-56789abcdef0'
CHAR1_UUID   = SERVICE_UUID[:-1] + '1'  # SSID
CHAR2_UUID   = SERVICE_UUID[:-1] + '2'  # PSK

# Credentials storage
creds = {'ssid': None, 'psk': None}

def on_ssid_write(value, options):
    creds['ssid'] = bytes(value).decode()
    print(f"[GATT] SSID gesetzt: {creds['ssid']}")

def on_psk_write(value, options):
    creds['psk'] = bytes(value).decode()
    print(f"[GATT] PSK gesetzt: {creds['psk']}")
    if creds['ssid'] and creds['psk']:
        print(f"[GATT] Verbinde zu Wi-Fi: {creds['ssid']}")
        subprocess.run(['nmcli', 'device', 'wifi', 'connect',
                        creds['ssid'], 'password', creds['psk']], check=False)
        loop.quit()

# Adapter und Peripheral einrichten
adapter_hci0 = adapter.Adapter('hci0')
periph = peripheral.Peripheral(adapter_addr=adapter_hci0.address,
                               local_name='Pi-Setup')

# Service und Characteristics hinzufügen
periph.add_service(srv_id=1, uuid=SERVICE_UUID, primary=True)
periph.add_characteristic(srv_id=1, chr_id=1,
                          uuid=CHAR1_UUID,
                          flags=['write'],
                          write_callback=on_ssid_write)
periph.add_characteristic(srv_id=1, chr_id=2,
                          uuid=CHAR2_UUID,
                          flags=['write'],
                          write_callback=on_psk_write)

print("[START] Werbe als Pi-Setup und warte auf SSID + PSK...")
periph.publish()

# GLib-Mainloop starten
loop = GLib.MainLoop()
loop.run()

print("[END] Provisioning abgeschlossen.")
sys.exit(0)
