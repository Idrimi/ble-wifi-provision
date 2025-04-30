#include <sdbus-c++/sdbus-c++.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <map>
#include <cstdlib>

// Paths and UUIDs
static constexpr auto SERVICE_PATH = "/org/bluez/example/service0";
static constexpr auto CHAR1_PATH   = "/org/bluez/example/service0/char1";
static constexpr auto CHAR2_PATH   = "/org/bluez/example/service0/char2";
static constexpr auto ADV_PATH     = "/org/bluez/example/advertisement0";

static const std::string SERVICE_UUID = "12345678-1234-5678-1234-56789abcdef0";
static const std::string CHAR1_UUID   = SERVICE_UUID.substr(0, SERVICE_UUID.size()-1) + "1";
static const std::string CHAR2_UUID   = SERVICE_UUID.substr(0, SERVICE_UUID.size()-1) + "2";

static std::string ssid, psk;

int main()
{
    // Connect to system bus
    auto connection = sdbus::createSystemBusConnection();

    // Create GATT Service object
    auto svcObj = sdbus::createObject(*connection, SERVICE_PATH);
    svcObj->registerProperty("UUID")
          .onInterface("org.bluez.GattService1")
          .withGetter([&] { return SERVICE_UUID; });
    svcObj->registerProperty("Primary")
          .onInterface("org.bluez.GattService1")
          .withGetter([&] { return true; });
    svcObj->registerProperty("Characteristics")
          .onInterface("org.bluez.GattService1")
          .withGetter([&] {
              return std::vector<sdbus::ObjectPath>{
                  sdbus::ObjectPath(CHAR1_PATH),
                  sdbus::ObjectPath(CHAR2_PATH)
              };
          });
    svcObj->finishRegistration();

    // Create SSID Characteristic
    auto char1Obj = sdbus::createObject(*connection, CHAR1_PATH);
    char1Obj->registerProperty("UUID")
            .onInterface("org.bluez.GattCharacteristic1")
            .withGetter([&] { return CHAR1_UUID; });
    char1Obj->registerProperty("Service")
            .onInterface("org.bluez.GattCharacteristic1")
            .withGetter([&] { return sdbus::ObjectPath(SERVICE_PATH); });
    char1Obj->registerProperty("Flags")
            .onInterface("org.bluez.GattCharacteristic1")
            .withGetter([&] { return std::vector<std::string>{"write"}; });
    char1Obj->registerMethod("WriteValue")
            .onInterface("org.bluez.GattCharacteristic1")
            .implementedAs([&](std::vector<uint8_t> value,
                              std::map<std::string, sdbus::Variant>){
        ssid = std::string(value.begin(), value.end());
        std::cout << "[GATT] SSID: " << ssid << std::endl;
    });
    char1Obj->finishRegistration();

    // Create PSK Characteristic
    auto char2Obj = sdbus::createObject(*connection, CHAR2_PATH);
    char2Obj->registerProperty("UUID")
            .onInterface("org.bluez.GattCharacteristic1")
            .withGetter([&] { return CHAR2_UUID; });
    char2Obj->registerProperty("Service")
            .onInterface("org.bluez.GattCharacteristic1")
            .withGetter([&] { return sdbus::ObjectPath(SERVICE_PATH); });
    char2Obj->registerProperty("Flags")
            .onInterface("org.bluez.GattCharacteristic1")
            .withGetter([&] { return std::vector<std::string>{"write"}; });
    char2Obj->registerMethod("WriteValue")
            .onInterface("org.bluez.GattCharacteristic1")
            .implementedAs([&](std::vector<uint8_t> value,
                              std::map<std::string, sdbus::Variant>){
        psk = std::string(value.begin(), value.end());
        std::cout << "[GATT] PSK: " << psk << std::endl;
    });
    char2Obj->finishRegistration();

    // Register GATT application
    auto gattMgr = sdbus::createProxy(*connection, "org.bluez", "/org/bluez/hci0");
    gattMgr->callMethod("RegisterApplication")
           .onInterface("org.bluez.GattManager1")
           .withArguments(sdbus::ObjectPath("/"), std::map<std::string, sdbus::Variant>{});

    // Create LE Advertisement object
    auto advObj = sdbus::createObject(*connection, ADV_PATH);
    advObj->registerProperty("Type")
          .onInterface("org.bluez.LEAdvertisement1")
          .withGetter([&] { return std::string("peripheral"); });
    advObj->registerProperty("ServiceUUIDs")
          .onInterface("org.bluez.LEAdvertisement1")
          .withGetter([&] { return std::vector<std::string>{SERVICE_UUID}; });
    advObj->registerProperty("LocalName")
          .onInterface("org.bluez.LEAdvertisement1")
          .withGetter([&] { return std::string("Pi-Setup"); });
    advObj->registerProperty("Includes")
          .onInterface("org.bluez.LEAdvertisement1")
          .withGetter([&] { return std::vector<std::string>{"tx-power"}; });
    advObj->registerMethod("Release")
          .onInterface("org.bluez.LEAdvertisement1")
          .implementedAs([&]{ std::cout<<"Advertisement released\n"; });
    advObj->finishRegistration();

    // Register Advertisement
    advMgr->callMethod("RegisterAdvertisement")
           .onInterface("org.bluez.LEAdvertisingManager1")
           .withArguments(sdbus::ObjectPath(ADV_PATH), std::map<std::string, sdbus::Variant>{});

    std::cout << "🟢 Advertising as Pi-Setup, waiting for credentials..." << std::endl;

    // Start event loop
    connection->enterEventLoopAsync();

    // Wait for both SSID and PSK
    while(ssid.empty() || psk.empty())
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Connect to Wi-Fi
    std::string cmd = "nmcli device wifi connect \"" + ssid + "\" password \"" + psk + "\"";
    std::cout << "🔌 Running: " << cmd << std::endl;
    std::system(cmd.c_str());

    // Unregister advertisement
    advMgr->callMethod("UnregisterAdvertisement")
           .onInterface("org.bluez.LEAdvertisingManager1")
           .withArguments(sdbus::ObjectPath(ADV_PATH));

    std::cout << "✅ Provisioning complete, exiting." << std::endl;
    return 0;
}
