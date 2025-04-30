package main

import (
    "fmt"
    "os"
    "os/exec"

    "github.com/go-ble/ble"
    "github.com/go-ble/ble/linux"
)

func main() {
    // Initialize BLE device
    d, err := linux.NewDevice()
    if err != nil {
        fmt.Fprintf(os.Stderr, "Failed to init device: %v\n", err)
        os.Exit(1)
    }
    ble.SetDefaultDevice(d)

    serviceUUID := ble.MustParse("12345678-1234-5678-1234-56789abcdef0")
    ssidChar := ble.NewCharacteristic(ble.MustParse("12345678-1234-5678-1234-56789abcdef1"))
    pskChar := ble.NewCharacteristic(ble.MustParse("12345678-1234-5678-1234-56789abcdef2"))

    creds := make(map[string]string)

    ssidChar.HandleWrite(ble.WriteHandlerFunc(func(req ble.Request, rsp ble.ResponseWriter) {
        ssid := string(req.Data())
        fmt.Printf("Received SSID: %s\n", ssid)
        creds["ssid"] = ssid
    }))
    pskChar.HandleWrite(ble.WriteHandlerFunc(func(req ble.Request, rsp ble.ResponseWriter) {
        psk := string(req.Data())
        fmt.Printf("Received PSK: %s\n", psk)
        creds["psk"] = psk
    }))

    service := ble.NewService(serviceUUID)
    service.AddCharacteristic(ssidChar)
    service.AddCharacteristic(pskChar)
    ble.AddService(service)

    // Advertise
    fmt.Println("Advertising as Pi-Setup...")
    go ble.AdvertiseNameAndServices("Pi-Setup", serviceUUID)

    // Wait for creds
    for {
        if creds["ssid"] != "" && creds["psk"] != "" {
            fmt.Printf("Credentials received - SSID: %s, PSK: %s\n", creds["ssid"], creds["psk"])
            break
        }
        // sleep
        exec.Command("sleep", "1").Run()
    }

    // Connect to Wi-Fi
    fmt.Println("Connecting to Wi-Fi...")
    cmd := exec.Command("nmcli", "device", "wifi", "connect", creds["ssid"], "password", creds["psk"])
    out, err := cmd.CombinedOutput()
    if err != nil {
        fmt.Fprintf(os.Stderr, "nmcli error: %v, output: %s\n", err, string(out))
        os.Exit(1)
    }
    fmt.Println("Connected successfully, stopping advertisement.")
    d.Stop()
}
