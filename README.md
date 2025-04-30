# BLE Wi-Fi Provisioning via C++ for Raspberry Pi Zero 2 W (Write Flags Fixed)

This version adds `write-without-response` flag to characteristics so iOS apps like LightBlue can write.

## Changes
- Characteristic Flags now include both "write" and "write-without-response".

Compile and deploy as before.
