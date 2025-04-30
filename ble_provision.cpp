#include <sdbus-c++/sdbus-c++.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <cstdlib>

static constexpr auto BLUEZ_SERVICE        = "org.bluez";
static constexpr auto ADAPTER_PATH         = "/org/bluez/hci0";
static constexpr auto GATT_MANAGER_IFACE   = "org.bluez.GattManager1";
static constexpr auto ADVERT_MGR_IFACE     = "org.bluez.LEAdvertisingManager1";
static constexpr auto GATT_SERVICE_IFACE   = "org.bluez.GattService1";
static constexpr auto GATT_CHAR_IFACE      = "org.bluez.GattCharacteristic1";
static constexpr auto ADVERT_IFACE         = "org.bluez.LEAdvertisement1";

static constexpr auto SERVICE_PATH         = "/org/bluez/example/service0";
static constexpr auto CHAR1_PATH           = "/org/bluez/example/service0/char1";
static constexpr auto CHAR2_PATH           = "/org/bluez/example/service0/char2";
static constexpr auto ADV_PATH             = "/org/bluez/example/advertisement0";

static const std::string SERVICE_UUID      = "12345678-1234-5678-1234-56789abcdef0";
static const std::string CHAR1_UUID        = SERVICE_UUID.substr(0, SERVICE_UUID.size()-1) + "1";
static const std::string CHAR2_UUID        = SERVICE_UUID.substr(0, SERVICE_UUID.size()-1) + "2";

int main()
{
    std::string ssid, psk;

    // Connect to system bus & start event loop
    auto connection = sdbus::createSystemBusConnection();
    connection->enterEventLoopAsync();

    // Create GATT service
    auto service = sdbus::createObject(*connection, SERVICE_PATH);
    service->registerProperty("UUID")
           .onInterface(GATT_SERVICE_IFACE)
           .withGetter([] { return SERVICE_UUID; });
    service->registerProperty("Primary")
           .onInterface(GATT_SERVICE_IFACE)
           .withGetter([] { return true; });
    service->registerProperty("Characteristics")
           .onInterface(GATT_SERVICE_IFACE)
           .withGetter([] {
               return std::vector<sdbus::ObjectPath>{{CHAR1_PATH}, {CHAR2_PATH}};
           });
    service->finishRegistration();

    // SSID characteristic
    auto char1 = sdbus::createObject(*connection, CHAR1_PATH);
    char1->registerProperty("UUID")
         .onInterface(GATT_CHAR_IFACE)
         .withGetter([] { return CHAR1_UUID; });
    char1->registerProperty("Service")
         .onInterface(GATT_CHAR_IFACE)
         .withGetter([] { return sdbus::ObjectPath{SERVICE_PATH}; });
    char1->registerProperty("Flags")
         .onInterface(GATT_CHAR_IFACE)
         .withGetter([] { return std::vector<std::string>{"write"}; });
    char1->registerMethod("WriteValue")
         .onInterface(GATT_CHAR_IFACE)
         .implementedAs([&](const std::vector<uint8_t>& value, const sdbus::VariantMap&) {
        ssid = std::string(value.begin(), value.end());
        std::cout << "[GATT] SSID: " << ssid << "\n";
    });
    char1->finishRegistration();

    // PSK characteristic
    auto char2 = sdbus::createObject(*connection, CHAR2_PATH);
    char2->registerProperty("UUID")
         .onInterface(GATT_CHAR_IFACE)
         .withGetter([] { return CHAR2_UUID; });
    char2->registerProperty("Service")
         .onInterface(GATT_CHAR_IFACE)
         .withGetter([] { return sdbus::ObjectPath{SERVICE_PATH}; });
    char2->registerProperty("Flags")
         .onInterface(GATT_CHAR_IFACE)
         .withGetter([] { return std::vector<std::string>{"write"}; });
    char2->registerMethod("WriteValue")
         .onInterface(GATT_CHAR_IFACE)
         .implementedAs([&](const std::vector<uint8_t>& value, const sdbus::VariantMap&) {
        psk = std::string(value.begin(), value.end());
        std::cout << "[GATT] PSK: " << psk << "\n";
    });
    char2->finishRegistration();

    // Register GATT application (fire-and-forget)
    auto gattMgr = sdbus::createProxy(*connection, BLUEZ_SERVICE, ADAPTER_PATH, GATT_MANAGER_IFACE);
    gattMgr->callMethod("RegisterApplication")
           .onInterface(GATT_MANAGER_IFACE)
           .withArguments(sdbus::ObjectPath{"/"}, sdbus::VariantMap{})
           .dontExpectReply();

    // Short delay
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Advertising manager proxy
    auto advMgr = sdbus::createProxy(*connection, BLUEZ_SERVICE, ADAPTER_PATH, ADVERT_MGR_IFACE);

    // Create advertisement
    auto adv = sdbus::createObject(*connection, ADV_PATH);
    adv->registerProperty("Type")
        .onInterface(ADVERT_IFACE)
        .withGetter([] { return std::string("peripheral"); });
    adv->registerProperty("ServiceUUIDs")
        .onInterface(ADVERT_IFACE)
        .withGetter([] { return std::vector<std::string>{SERVICE_UUID}; });
    adv->registerProperty("LocalName")
        .onInterface(ADVERT_IFACE)
        .withGetter([] { return std::string("Pi-Setup"); });
    adv->registerProperty("Includes")
        .onInterface(ADVERT_IFACE)
        .withGetter([] { return std::vector<std::string>{"tx-power"}; });
    adv->registerMethod("Release")
        .onInterface(ADVERT_IFACE)
        .implementedAs([] { std::cout << "Advertisement released\n"; });
    adv->finishRegistration();

    // Register advertisement
    advMgr->callMethod("RegisterAdvertisement")
          .onInterface(ADVERT_MGR_IFACE)
          .withArguments(sdbus::ObjectPath{ADV_PATH}, sdbus::VariantMap{})
          .dontExpectReply();

    std::cout << "🟢 Advertising as Pi-Setup, waiting for credentials…\n";

    // Wait for writes
    while(ssid.empty() || psk.empty())
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Connect to Wi-Fi
    std::string cmd = "nmcli device wifi connect \"" + ssid + "\" password \"" + psk + "\"";
    std::cout << "🔌 " << cmd << "\n";
    std::system(cmd.c_str());

    // Unregister advertisement
    advMgr->callMethod("UnregisterAdvertisement")
          .onInterface(ADVERT_MGR_IFACE)
          .withArguments(sdbus::ObjectPath{ADV_PATH})
          .dontExpectReply();

    std::cout << "✅ Provisioning complete, exiting.\n";
    return 0;
}
