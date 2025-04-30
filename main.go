package main

import (
    "fmt"
    "os"
    "os/exec"
    "time"

    "github.com/go-ble/ble"
    "github.com/go-ble/ble/linux"
)

var (
    ssidChan = make(chan string)
    pskChan  = make(chan string)
)

func main() {
    // Initialize BLE device
    d, err := linux.NewDevice()
    if err != nil {
        fmt.Fprintf(os.Stderr, "Failed to init device: %v\n", err)
        os.Exit(1)
    }
    ble.SetDefaultDevice(d)

    // Define service and characteristics
    serviceUUID := ble.MustParse("12345678-1234-5678-1234-56789abcdef0")
    ssidChar := ble.NewCharacteristic(ble.MustParse("12345678-1234-5678-1234-56789abcdef1"))
    pskChar := ble.NewCharacteristic(ble.MustParse("12345678-1234-5678-1234-56789abcdef2"))

    ssidChar.HandleWrite(ble.WriteHandlerFunc(func(req ble.Request, rsp ble.ResponseWriter) {
        ssid := string(req.Data())
        fmt.Printf("Received SSID: %s\n", ssid)
        ssidChan <- ssid
    }))
    pskChar.HandleWrite(ble.WriteHandlerFunc(func(req ble.Request, rsp ble.ResponseWriter) {
        psk := string(req.Data())
        fmt.Printf("Received PSK: %s\n", psk)
        pskChan <- psk
    }))

    service := ble.NewService(serviceUUID)
    service.AddCharacteristic(ssidChar)
    service.AddCharacteristic(pskChar)
    ble.AddService(service)

    // Advertise
    go func() {
        advOpts := []ble.AdvertisementOption{
            ble.WithLocalName("Pi-Setup"),
            ble.WithServices(serviceUUID),
        }
        fmt.Println("Advertising as Pi-Setup...")
        ble.AdvertiseNameAndServices(advOpts...)
    }()

    // Wait for credentials
    ssid := <-ssidChan
    psk := <-pskChan
    fmt.Printf("Credentials received - SSID: %s, PSK: %s\n", ssid, psk)

    // Connect to Wi-Fi
    fmt.Println("Connecting to Wi-Fi...")
    cmd := exec.Command("nmcli", "device", "wifi", "connect", ssid, "password", psk)
    out, err := cmd.CombinedOutput()
    if err != nil {
        fmt.Fprintf(os.Stderr, "nmcli error: %v, output: %s\n", err, string(out))
        os.Exit(1)
    }
    fmt.Println("Connected successfully, stopping advertisement.")
    d.Stop()

    // Wait a bit before exit
    time.Sleep(2 * time.Second)
}