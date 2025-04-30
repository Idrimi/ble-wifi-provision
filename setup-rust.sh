#!/usr/bin/env bash
set -euo pipefail

echo "==> Installing Rust toolchain..."
if ! command -v cargo &>/dev/null; then
  curl https://sh.rustup.rs -sSf | sh -s -- -y
  source $HOME/.cargo/env
fi

echo "==> Building BLE provisioning binary..."
cd "$(dirname "$0")"
cargo build --release

echo "==> Deploying..."
sudo cp target/release/ble_rust_provision /usr/local/bin/ble-rust-provision
sudo cp ble-wifi-provision.sh /usr/local/bin/ble-wifi-provision.sh
sudo cp ble-wifi-provision.service /etc/systemd/system/
sudo chmod +x /usr/local/bin/ble-rust-provision /usr/local/bin/ble-wifi-provision.sh
sudo systemctl daemon-reload
sudo systemctl enable ble-wifi-provision.service

echo "Setup complete! Start provisioning with:"
echo "  sudo systemctl start ble-wifi-provision.service"
