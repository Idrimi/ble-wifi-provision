use bluer::{adapter::Adapter, gatt::local::{Application, Service, Characteristic}, Session};
use bluer::Address;
use futures::stream::StreamExt;
use std::process::Command;
use std::error::Error;

const SERVICE_UUID: &str = "12345678-1234-5678-1234-56789abcdef0";
const CHAR1_UUID: &str = "12345678-1234-5678-1234-56789abcdef1";
const CHAR2_UUID: &str = "12345678-1234-5678-1234-56789abcdef2";

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    let session = Session::new().await?;
    let adapter = session.default_adapter().await?;
    adapter.set_powered(true).await?;

    // Create GATT application
    let mut app = Application::new(&session).await?;
    let svc = app.service(SERVICE_UUID.parse()?, true).await?;
    let ssid_ch = svc.characteristic(CHAR1_UUID.parse()?).await?
        .with_write(true).build().await?;
    let psk_ch = svc.characteristic(CHAR2_UUID.parse()?).await?
        .with_write(true).build().await?;
    app.serve().await?;

    // Start advertising
    adapter.advertisement().await?
        .name("Pi-Setup")
        .service_uuids(vec![SERVICE_UUID.parse()?])
        .register().await?;
    println!("Advertising as Pi-Setup...");

    // Wait for SSID
    let mut ssid_stream = ssid_ch.write().await?;
    let ssid = ssid_stream.next().await
        .ok_or("No SSID write")?
        .value;
    let ssid = String::from_utf8_lossy(&ssid).to_string();
    println!("Received SSID: {}", ssid);

    // Wait for PSK
    let mut psk_stream = psk_ch.write().await?;
    let psk = psk_stream.next().await
        .ok_or("No PSK write")?
        .value;
    let psk = String::from_utf8_lossy(&psk).to_string();
    println!("Received PSK: {}", psk);

    // Connect to Wi-Fi
    println!("Connecting to Wi-Fi...");
    let status = Command::new("nmcli")
        .args(&["device", "wifi", "connect", &ssid, "password", &psk])
        .status()?;
    if status.success() {
        println!("Connected to {}", ssid);
    } else {
        eprintln!("Failed to connect");
    }

    Ok(())
}
