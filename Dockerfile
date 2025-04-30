FROM debian:bookworm-slim
RUN apt-get update && \
    apt-get install -y python3-trio python3-dbus-next bluez bluetooth rfkill network-manager dbus-client && \
    rm -rf /var/lib/apt/lists/*
COPY ble-creds-server.py /usr/local/bin/
COPY ble-wifi-provision.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/ble-creds-server.py /usr/local/bin/ble-wifi-provision.sh
ENTRYPOINT ["/usr/local/bin/ble-wifi-provision.sh"]
